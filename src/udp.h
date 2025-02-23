#ifndef UDP_H
#define UDP_H

#include "network.h"

#include <netinet/udp.h>

int send_udp_msg(int port);
int handle_udp_msg(char *packet, int packet_len);

#endif
