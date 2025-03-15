/**
 * @file network.h
 * @brief Header file for network-related functions and data structures.
 *
 * This header file contains the declarations of functions and structures used
 * for managing network configuration, socket setup, and communication. It includes
 * definitions for network parameters, socket management, checksum calculation, 
 * and IP address handling.
 *
 * @author Samuel Stefanik (xstefas00)
 * @date   2025-02-17
 */
#ifndef NETWORK_H
#define NETWORK_H

#include "list.h"
#include "args.h"

#include <ifaddrs.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

#define SOCKET_COUNT 3      /**< The number of socket types. */
#define SOURCE_PORT 32132   /**< Randomly chosen source port number. */

/**
 * @brief Enum representing the different socket types.
 * 
 */
typedef enum sockets {
    TCP,
    UDP,
    ICMP
} sockets_t;

/**
 * @brief Structure representing the network configuration.
 * 
 */
typedef struct network {
    struct addrinfo *list;                /**< Pointer to the address list for destination addresses */
    struct addrinfo *active;              /**< Pointer to the active address being processed */
    struct sockaddr_storage src;          /**< Source address storage (IPv4 or IPv6) */
    struct sockaddr_storage dst;          /**< Destination address storage (IPv4 or IPv6) */
    int family;                           /**< Address family (IPv4 or IPv6) */
    int sockets[SOCKET_COUNT];            /**< Array holding the socket file descriptors for each socket type */
    list_t scanned_ip_adresses;           /**< List of scanned IP addresses */
    char ip_str[INET6_ADDRSTRLEN];        /**< IP address in string format */
} network_t;

/**
 * @brief Structure representing the pseudo-header for IPv4 checksum calculation.
 * 
 */
typedef struct ipv4_psh {
    uint32_t src;                         /**< Source IP address */
    uint32_t dst;                         /**< Destination IP address */
    uint8_t zero;                         /**< Reserved field */
    uint8_t protocol;                     /**< Protocol type */
    uint16_t header_len;                  /**< Length of the header */
} ipv4_psh_t;

/**
 * @brief Structure representing the pseudo-header for IPv6 checksum calculation.
 * 
 */
typedef struct ipv6_psh {
    struct in6_addr src;                  /**< Source IPv6 address */
    struct in6_addr dst;                  /**< Destination IPv6 address */
    uint32_t header_len;                  /**< Length of the IPv6 header */
    uint8_t zero[3];                      /**< Reserved fields */
    uint8_t next_header;                  /**< Next header type */
} ipv6_psh_t;

extern network_t network;

/**
 * @brief Lists all active network interfaces.
 * 
 * This function retrieves the list of network interfaces on the system using the 
 * `getifaddrs` function and filters for interfaces that are up (`IFF_UP`) and not 
 * previously printed.
 * 
 * @return EXIT_SUCCESS if the list of active interfaces is successfully printed, 
 *         or EXIT_FAILURE if an error occurs.
 */
int list_active_interfaces(void);

/**
 * @brief Initializes the network setup for scanning.
 * 
 * This function sets up the network configuration, including determining the destination 
 * addresses to scan and setting up raw sockets for communication.
 * 
 * @param first_setup A boolean flag indicating whether this is the first setup (initial scan).
 *                    If true, the network setup will include initializing the list of scanned IPs.
 * 
 * @return EXIT_SUCCESS if the network setup is completed successfully and sockets are set up,
 *         or EXIT_FAILURE if an error occurs during the setup.
 */
int network_setup(bool first_setup);

/**
 * @brief Cleans up network resources.
 * 
 * This function is responsible for cleaning up all resources used by the network setup, 
 * including freeing the list of scanned IP addresses, freeing the address information list, 
 * and closing any open network sockets.
 */
void network_clean_up(void);

/**
 * @brief Proceeds to the next IP address in the scan.
 * 
 * This function closes the current network sockets and then reinitializes the network setup 
 * for scanning the next IP address.
 * 
 * @return EXIT_SUCCESS on success, or EXIT_FAILURE on error.
 */
int next_ip(void);

/**
 * @brief Calculates the checksum for the given header based on the network family.
 * 
 * This function determines whether the network family is IPv4 or IPv6 and then calls 
 * the appropriate checksum function to compute 
 * the checksum for the provided header.
 * 
 * @param header The header data for which the checksum will be calculated.
 * @param header_size The size of the header in bytes.
 * @param protocol The protocol used in the header.
 * @return The calculated checksum value.
 */
uint16_t calculate_checksum(uint16_t *header, int header_size, uint8_t protocol);

#endif
