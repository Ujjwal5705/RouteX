#ifndef TORD_H
#define TORD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>

#define USERNAME "RouteX"
#define PROXY    "127.0.0.1"
#define PORT     9050

#pragma pack(push, 1)

struct proxy_request {
    uint8_t  vn;
    uint8_t  cd;
    uint16_t dstport;
    uint32_t dstip;
};
typedef struct proxy_request Req;

struct proxy_response{
    uint8_t vn;
    uint8_t cd;
    uint16_t dstport;
    uint32_t dstip;
};
typedef struct proxy_response Res;

#pragma pack(pop)

#endif