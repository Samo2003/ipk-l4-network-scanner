#include "udp.h"

static void prepare_udp_header(struct udphdr *header, int port) {
    header->uh_dport = htons(port);
    header->uh_sport = htons(SOURCE_PORT);
    header->uh_ulen = htons(sizeof(struct udphdr));
}

int send_udp_msg(int port) {
    struct udphdr datagram = {0};
    prepare_udp_header(&datagram, port);
    datagram.uh_sum = calculate_checksum((uint16_t *)&datagram, sizeof(struct udphdr), IPPROTO_TCP);
    return sendto(network.sockets[UDP], (void *)&datagram, sizeof(struct udphdr), 0, (struct sockaddr *)&network.dst, sizeof(struct sockaddr));
}

int handle_udp_msg(char *packet, int packet_len) {
    return 0;
}
