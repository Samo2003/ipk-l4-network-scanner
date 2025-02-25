#include "udp.h"

static void prepare_udp_header(struct udphdr *header, int port) {
    header->uh_dport = htons(port);
    header->uh_sport = htons(SOURCE_PORT);
    header->uh_ulen = htons(sizeof(struct udphdr));
}

int send_udp_msg(int port) {
    struct udphdr datagram = {0};
    prepare_udp_header(&datagram, port);
    datagram.uh_sum = calculate_checksum((uint16_t *)&datagram, sizeof(struct udphdr), IPPROTO_UDP);
    return sendto(network.sockets[UDP], (void *)&datagram, sizeof(struct udphdr), 0, (struct sockaddr *)&network.dst, sizeof(struct sockaddr_storage));
}

int handle_udp_msg(char *datagram, size_t datagram_len, int port) {
    if (network.family == AF_INET) {
        struct icmp *header = (struct icmp *)(datagram + sizeof(struct ip));
        if (header->icmp_type == ICMP_UNREACH && header->icmp_code == ICMP_UNREACH_PORT) {              
            struct udphdr *udp_header = (struct udphdr *)(datagram + datagram_len - 8);
            
            if(udp_header->uh_sport != htons(port)) {
                return EXIT_FAILURE;
            }

            fprintf(stdout, "%d/udp\tclosed\n", port);
            return EXIT_SUCCESS;
        }
    } else {
        struct icmp6_hdr *header = (struct icmp6_hdr *)datagram;
        if (header->icmp6_type == ICMP6_DST_UNREACH && header->icmp6_code == ICMP6_DST_UNREACH_NOPORT) {

            uint8_t *packet = header->icmp6_dataun.icmp6_un_data8 + 4;
            struct udphdr *udp_header = (struct udphdr *)(packet + sizeof(struct ip6_hdr));

            if(udp_header->uh_dport != htons(port)) {
                return EXIT_FAILURE;
            }
            
            fprintf(stdout, "%d/udp\tclosed\n", port);
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}
