#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <HOSTNAME> <PORT>\n", argv[0]);
        return 1;
    }

    char *hostname = argv[1];
    int port = atoi(argv[2]);

    printf("--- RouteX Secure Client (HTTPS) ---\n");
    printf("Target: https://%s:%d\n", hostname, port);

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        return 1;
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    SSL_CTX_set_default_verify_paths(ctx);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("0.0.0.1");

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        return 1;
    }
    printf("✅ TCP Tunnel Established via Tor.\n");

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    SSL_set_tlsext_host_name(ssl, hostname);
    SSL_set1_host(ssl, hostname);

    printf("🔒 Performing SSL Handshake...\n");
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    long verify_result = SSL_get_verify_result(ssl);
    if (verify_result != X509_V_OK) {
        fprintf(stderr, "Certificate verification failed: %ld\n", verify_result);
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        return 1;
    }

    printf("🔐 SSL Connected using %s\n", SSL_get_cipher(ssl));

    char req[1024];
    snprintf(req, sizeof(req), 
        "GET / HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7)\r\n"
        "Connection: close\r\n"
        "\r\n", hostname);

    SSL_write(ssl, req, strlen(req));

    printf("\n--- Secure Response ---\n");
    char buffer[4096];
    int bytes;
    while ((bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
    }
    printf("\n");

    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    return 0;
}