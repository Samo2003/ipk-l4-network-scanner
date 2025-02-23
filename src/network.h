#ifndef NETWORK_H
#define NETWORK_H

#include "list.h"
#include "args.h"

#include <ifaddrs.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>


typedef enum sockets {
    TCP,
    UDP,
    ICMP
} sockets_t;

#define SOCKET_COUNT 3
#define SOURCE_PORT 32132   // randomly choosen

typedef struct network {
    struct addrinfo *list, *active;
    struct sockaddr_storage src;
    struct sockaddr_storage dst;
    int family;
    int sockets[SOCKET_COUNT];
    list_t scanned_ip_adresses;
} network_t;

extern network_t network;

int list_active_interfaces(void);
int network_setup(bool first_setup);
void network_clean_up(void);
int next_ip(void);
uint16_t calculate_checksum(uint16_t *header, int header_size, int protocol);

#endif
