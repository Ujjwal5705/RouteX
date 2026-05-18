#define _GNU_SOURCE
#include "tord.h"

static int (*real_connect)(int, const struct sockaddr *, socklen_t) = NULL;
static pthread_once_t connect_init_once = PTHREAD_ONCE_INIT;

static void init_real_connect() {
    real_connect = dlsym(RTLD_NEXT, "connect");
}

int connect(int s2, const struct sockaddr *sock2, socklen_t address_len) {
    
    pthread_once(&connect_init_once, init_real_connect);

    if (!real_connect) return -1;

    if (sock2->sa_family != AF_INET)
        return real_connect(s2, sock2, address_len);

    struct sockaddr_in *dest_addr = (struct sockaddr_in *)sock2;

    char *remote_host = getenv("ROUTEX_HOSTNAME");
    
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return -1;

    struct sockaddr_in proxy = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = inet_addr(PROXY)
    };

    if (real_connect(s, (struct sockaddr *)&proxy, sizeof(proxy)) < 0) {
        perror("Could not connect to proxy");
        close(s);
        return -1;
    }

    char greet[] = { 0x05, 0x01, 0x00 };
    if (write(s, greet, 3) < 0) {
        close(s); return -1;
    }

    char resp[2];
    if (read(s, resp, 2) < 2 || resp[1] != 0x00) {
        close(s); return -1;
    }

    char req[512];
    int req_len;

    req[0] = 0x05;
    req[1] = 0x01;
    req[2] = 0x00;

    if (remote_host) {
        int hlen = strlen(remote_host);
        if (hlen > 255) hlen = 255;

        req[3] = 0x03;
        req[4] = (char)hlen;
        memcpy(&req[5], remote_host, hlen);
        
        memcpy(&req[5 + hlen], &dest_addr->sin_port, 2);
        
        req_len = 5 + hlen + 2;
        printf("🔒 Remote DNS: Requesting Tor to visit '%s' directly...\n", remote_host);

    } else {
        req[3] = 0x01;
        memcpy(&req[4], &dest_addr->sin_addr.s_addr, 4);
        memcpy(&req[8], &dest_addr->sin_port, 2);
        req_len = 10;
    }

    write(s, req, req_len);

    char result[10];
    if (read(s, result, 10) < 4) {
        perror("Proxy read failed");
        close(s);
        return -1;
    }

    if (result[1] != 0x00) {
        fprintf(stderr, "Proxy rejected connection: 0x%02x\n", result[1]);
        close(s);
        errno = ECONNREFUSED;
        return -1;
    }

    printf("✅ Connection Established via Tor.\n");

    dup2(s, s2);
    close(s);

    return 0;
}