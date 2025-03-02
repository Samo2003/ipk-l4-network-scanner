/**
 * @file udp.c
 * @brief Implements UDP packet sending and response handling for port scanning.
 *
 * This module contains functions for constructing, sending, and processing
 * UDP packets used in the network scanner. It includes checksum calculations
 * and handling of ICMP responses to determine port statuses.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date   2025-02-19
 */
#include "udp.h"

/**
 * @brief Prepares a UDP header for sending a UDP packet.
 *
 * This function initializes a UDP header with the given destination port.
 *
 * @param header Pointer to the UDP header structure to be initialized.
 * @param port Destination port for the UDP packet.
 */
static void prepare_udp_header(struct udphdr *header, int port) {
    header->uh_dport = htons(port);
    header->uh_sport = htons(SOURCE_PORT);
    header->uh_ulen = htons(sizeof(struct udphdr));
}

/**
 * @brief Sends a UDP packet to the specified port.
 *
 * @param port The destination port for the UDP packet.
 * @return On success, returns the number of bytes sent. On failure, returns -1 and sets errno.
 */
int send_udp_msg(int port) {
    struct udphdr datagram = {0};
    prepare_udp_header(&datagram, port);
    datagram.uh_sum = calculate_checksum((uint16_t *)&datagram, sizeof(struct udphdr), IPPROTO_UDP);
    return sendto(network.sockets[UDP], (void *)&datagram, sizeof(struct udphdr), 0, (struct sockaddr *)&network.dst, sizeof(struct sockaddr_storage));
}

/**
 * @brief Handles an incoming UDP response and determines if the port is closed.
 *
 * @param datagram Pointer to the received datagram.
 * @param datagram_len Length of the received datagram.
 * @param port The port that was scanned.
 * @return EXIT_SUCCESS if the port is determined to be closed, EXIT_FAILURE otherwise.
 */
int handle_udp_msg(char *datagram, size_t datagram_len, int port) {
    struct udphdr *udp_header;
    if (network.family == AF_INET) {
        struct icmp *header = (struct icmp *)(datagram + sizeof(struct ip));
        if (header->icmp_type == ICMP_UNREACH && header->icmp_code == ICMP_UNREACH_PORT) {              
            udp_header = (struct udphdr *)(datagram + datagram_len - 8);
        } else {
            return EXIT_FAILURE;
        }
    } else {
        struct icmp6_hdr *header = (struct icmp6_hdr *)datagram;
        if (header->icmp6_type == ICMP6_DST_UNREACH && header->icmp6_code == ICMP6_DST_UNREACH_NOPORT) {
            uint8_t *packet = header->icmp6_dataun.icmp6_un_data8 + 4;
            udp_header = (struct udphdr *)(packet + sizeof(struct ip6_hdr));            
        } else {
            return EXIT_FAILURE;
        }
    }
    if(udp_header->uh_dport != htons(port)) {
        return EXIT_FAILURE;
    }
    
    fprintf(stdout, "%d/udp\tclosed\n", port);
    return EXIT_SUCCESS;
}
