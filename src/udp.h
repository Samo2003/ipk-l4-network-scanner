/**
 * @file udp.h
 * @brief Declares functions for UDP packet operations in the port scanner.
 *
 * This header defines functions for sending UDP packets, preparing headers,
 * and handling received responses to determine if a port is open or closed.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date   2025-02-19
 */
#ifndef UDP_H
#define UDP_H

#include "network.h"

#include <netinet/udp.h>
#include <netinet/icmp6.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip6.h>

/**
 * @brief Sends a UDP packet to the specified port.
 *
 * This function prepares a UDP header, calculates its checksum, and sends 
 * the packet to the target destination using the appropriate network socket.
 *
 * @param port The destination port for the UDP packet.
 * @return On success, returns the number of bytes sent. On failure, returns -1 and sets errno.
 */
int send_udp_msg(int port);

/**
 * @brief Handles an incoming UDP response and determines if the port is closed.
 *
 * This function processes the received datagram to check if it contains an 
 * ICMP "port unreachable" message, which indicates that the scanned UDP port 
 * is closed.
 *
 * @param datagram Pointer to the received datagram.
 * @param datagram_len Length of the received datagram.
 * @param port The port that was scanned.
 * @return EXIT_SUCCESS if the port is determined to be closed, EXIT_FAILURE otherwise.
 */
int handle_udp_msg(char *datagram, size_t datagram_len, int port);

#endif
