/**
 * @file tcp.h
 * @brief Header file for TCP-related functions in the network scanner.
 *
 * This file declares functions for sending and handling TCP packets in a port scanner.
 * It provides functionality for constructing TCP segments, sending SYN requests, and
 * processing responses to determine if a port is open, closed, or filtered.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date   2025-02-19
 */
#ifndef TCP_H
#define TCP_H

#include "network.h"

#include <netinet/tcp.h>

/**
 * @brief Sends a TCP SYN message to the specified port.
 *
 * This function constructs a TCP segment with a SYN flag, calculates its checksum,
 * and sends it to the destination address stored in the `network` structure.
 *
 * @param port Destination port to which the TCP SYN message will be sent.
 * @return On success, returns the number of bytes sent. On failure, returns -1 and sets errno.
 */
int send_tcp_msg(int port);

/**
 * @brief Processes a received TCP segment and determines the port state.
 *
 * This function extracts the TCP header from the received segment and checks
 * the flags to determine if the port is open or closed.
 *
 * @param segment Pointer to the received TCP segment.
 * @param segment_len Length of the received segment in bytes.
 * @param port The destination port that was scanned.
 * @return EXIT_SUCCESS if the port state is determined (open or closed), EXIT_FAILURE otherwise.
 */
int handle_tcp_msg(char *segment, size_t segment_len, int port);

#endif
