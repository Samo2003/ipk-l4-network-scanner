#ifndef NETWORK_H
#define NETWORK_H

#include <ifaddrs.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include "list.h"
#include "args.h"

typedef enum sockets {
    TCP,
    UDP,
    ICMP
} sockets_t;

#define SOCKET_COUNT 3
#define SOURCE_PORT 32132   // randomly choosen

typedef struct network {
    struct sockaddr src;
    struct sockaddr dst;
    int family;
    int sockets[SOCKET_COUNT];
} network_t;

extern network_t network;

int network_setup(void);
int setup_sockets(void);
void network_clean_up(void);
void close_sockets(void);
int get_interface_ip(void);
int list_active_interfaces(void);
struct addrinfo *get_dst_addresses(void);

#endif
