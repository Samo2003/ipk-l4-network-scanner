#include "tcp.h"

static void prepare_tcp_header(struct tcphdr *header, int port) {
    header->th_dport = htons(port);
    header->th_flags = TH_SYN;
    header->th_off = sizeof(struct tcphdr) / 4;
    header->th_sport = htons(SOURCE_PORT);
    header->th_win = htons(0xffff);
}

int send_tcp_msg(int port) {
    struct tcphdr segment = {0};
    prepare_tcp_header(&segment, port);
    segment.th_sum = calculate_checksum((uint16_t *)&segment, sizeof(struct tcphdr), IPPROTO_TCP);
    return sendto(network.sockets[TCP], (void *)&segment, sizeof(struct tcphdr), 0, (struct sockaddr *)&network.dst, sizeof(struct sockaddr));
}

int handle_tcp_msg(char *packet, int packet_len) {
    return 0;
}
