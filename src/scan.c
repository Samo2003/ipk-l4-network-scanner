/**
 * @file scan.c
 * @brief Network scanning functions.
 *
 * This file contains functions for scanning ports and sending network messages 
 * using different protocols. It includes the main scanning logic 
 * for scanning the specified ports on a given network and handling responses 
 * from those ports.
 *
 * @author Samuel Stefanik (xstefas00)
 * @date   2025-02-19
 */
#include "scan.h"

/**
 * @brief Scans a specified port using either TCP or UDP protocol.
 *
 * This function sends a message to a given port using either the TCP or UDP protocol, waits for a response, 
 * and processes the received message. It handles retries for TCP messages in case of no response 
 * and outputs the port status.
 * 
 * @param port The port number to scan.
 * @param tcp A boolean flag indicating whether to use TCP (true) or UDP (false).
 * @return EXIT_SUCCESS if the scan completes successfully, or EXIT_FAILURE if an error occurs.
 */
static int scan(int port, bool tcp) {
    if ((tcp ? send_tcp_msg(port) : send_udp_msg(port)) < 0) {
        perror("sendto");
        return EXIT_FAILURE;
    }

    set_time();
    char rcv_buffer[BUFFER_SIZE];
    struct sockaddr_storage place_holder = {0};
    socklen_t place_holder_length = sizeof(struct sockaddr_storage);
    sockets_t socket = tcp ? TCP : ICMP;

    bool tcp_retry = tcp;
    while(true) {
        if (out_of_time()) {
            if (tcp_retry) {
                tcp_retry = false;
                set_time();
                continue;
            }
            break;
        }
        if (wait_response(socket) > 0) {
            size_t b_rcv = recvfrom(network.sockets[socket], rcv_buffer, BUFFER_SIZE, 0, (struct sockaddr *)&place_holder, &place_holder_length);
            if (b_rcv > 0) {
                if ((tcp ? handle_tcp_msg(rcv_buffer, b_rcv, port) : handle_udp_msg(rcv_buffer, b_rcv, port)) == EXIT_SUCCESS) {
                    return EXIT_SUCCESS;
                }
            }
        }
    }

    tcp ? fprintf(stdout, "%d/tcp\tfiltered\n", port) : fprintf(stdout, "%d/udp\topen\n", port);
    return EXIT_SUCCESS;
}

/**
 * @brief Scans a list of TCP and UDP ports.
 * 
 * @return EXIT_SUCCESS if all port scans complete successfully, or EXIT_FAILURE if any scan fails.
 */
int scan_ports(void) {
    do {
        for(int i = 0; i < parameters.tcp_ports_count; i++) {
            if (scan(parameters.tcp_ports[i], true) != EXIT_SUCCESS) {
                return EXIT_FAILURE;
            }
        }

        for(int i = 0; i < parameters.udp_ports_count; i++) {
            if (scan(parameters.udp_ports[i], false) != EXIT_SUCCESS) {
                return EXIT_FAILURE;
            }
        }
    } while(next_ip() != EXIT_FAILURE);
    return EXIT_SUCCESS;
}
