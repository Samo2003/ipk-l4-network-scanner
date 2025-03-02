/**
 * @file tcp.c
 * @brief Implementation of TCP packet crafting and response handling for port scanning.
 *
 * This file contains functions for constructing, sending, and analyzing TCP segments
 * in the context of a network scanner. It includes functions for sending SYN packets,
 * calculating checksums, and interpreting TCP responses.
 *
 * @author Samuel Stefanik (xstefas00)
 * @date   2025-02-19
 */
#include "tcp.h"

/**
 * @brief Prepares a TCP header for sending a SYN packet.
 *
 * This function initializes the provided TCP header structure with the necessary
 * values for sending a SYN request.
 *
 * @param header Pointer to the TCP header structure to be initialized.
 * @param port Destination port to which the TCP packet will be sent.
 */
static void prepare_tcp_header(struct tcphdr *header, int port) {
    header->th_dport = htons(port);
    header->th_flags = TH_SYN;
    header->th_off = sizeof(struct tcphdr) / 4;
    header->th_sport = htons(SOURCE_PORT);
    header->th_win = htons(0xffff);
}

/**
 * @brief Sends a TCP SYN message to the specified port.
 *
 * @param port Destination port to which the TCP SYN message will be sent.
 * @return On success, returns the number of bytes sent. On failure, returns -1 and sets errno.
 */
int send_tcp_msg(int port) {
    struct tcphdr segment = {0};
    prepare_tcp_header(&segment, port);
    segment.th_sum = calculate_checksum((uint16_t *)&segment, sizeof(struct tcphdr), IPPROTO_TCP);
    return sendto(network.sockets[TCP], (void *)&segment, sizeof(struct tcphdr), 0, (struct sockaddr *)&network.dst, sizeof(struct sockaddr_storage));
}

/**
 * @brief Processes a received TCP segment and determines the port state.
 *
 * @param segment Pointer to the received TCP segment.
 * @param segment_len Length of the received segment in bytes.
 * @param port The destination port that was scanned.
 * @return EXIT_SUCCESS if the port state is determined (open or closed), EXIT_FAILURE otherwise.
 */
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
        fprintf(stdout, "%d/tcp\tclosed\n", port);
        return EXIT_SUCCESS;
    } else if (header->th_flags & TH_SYN && header->th_flags & TH_ACK) {
        fprintf(stdout, "%d/tcp\topen\n", port);
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
