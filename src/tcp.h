#ifndef TCP_H
#define TCP_H

#include "network.h"

#include <netinet/tcp.h>

int send_tcp_msg(int port);
int handle_tcp_msg(char *packet, int packet_len);

#endif
