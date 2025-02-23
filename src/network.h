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

typedef struct ipv4_psh {
    uint32_t src;
    uint32_t dst;
    uint8_t zero;
    uint8_t protocol;
    uint16_t header_len;
} ipv4_psh_t;

typedef struct ipv6_psh {
    struct in6_addr src;
    struct in6_addr dst;
    uint32_t header_len;
    uint8_t zero[3];
    uint8_t next_header;
} ipv6_psh_t;

extern network_t network;

int list_active_interfaces(void);
int network_setup(bool first_setup);
void network_clean_up(void);
int next_ip(void);
uint16_t calculate_checksum(uint16_t *header, int header_size, uint8_t protocol);

#endif
