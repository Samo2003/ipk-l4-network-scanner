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
    return sendto(network.sockets[TCP], (void *)&segment, sizeof(struct tcphdr), 0, (struct sockaddr *)&network.dst, sizeof(struct sockaddr_storage));
}

int handle_tcp_msg(char *segment, size_t segment_len, int port) {
    int offset = network.family == AF_INET ? sizeof(struct ip) : 0;
    if (segment_len < sizeof(struct tcphdr) + offset) {
        return EXIT_FAILURE;
    }
    struct tcphdr *header = (struct tcphdr *)(segment + offset);
    if (header->th_sport != htons(port)) {
        return EXIT_FAILURE;
    }

    if (header->th_flags & TH_RST) {
        fprintf(stdout, "%d/tcp closed\n", port);
        return EXIT_SUCCESS;
    } else if (header->th_flags & TH_SYN && header->th_flags & TH_ACK) {
        fprintf(stdout, "%d/tcp open\n", port);
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
