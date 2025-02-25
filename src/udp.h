#ifndef UDP_H
#define UDP_H

#include "network.h"

#include <netinet/udp.h>
#include <netinet/icmp6.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip6.h>

int send_udp_msg(int port);
int handle_udp_msg(char *datagram, size_t datagram_len, int port);

#endif
