/**
 * @file scan.h
 * @brief Header file for network scanning functionality.
 *
 * This header file defines the interface for scanning network ports.
 *
 * @author Samuel Stefanik (xstefas00)
 * @date   2025-02-19
 */
#ifndef SCAN_H
#define SCAN_H

#include "tcp.h"
#include "udp.h"
#include "wait.h"

#define BUFFER_SIZE 1500    /**< Buffer size for responses. */

/**
 * @brief Scans a list of TCP and UDP ports.
 *
 * This function iterates through the list of specified TCP and UDP ports and performs a scan for each one.
 * It calls the `scan` function for each port and protocol type (TCP or UDP). If any scan fails, 
 * the function will immediately return `EXIT_FAILURE`. The process continues for each IP address 
 * until there are no more IP addresses to scan, at which point it returns `EXIT_SUCCESS`.
 * 
 * @return EXIT_SUCCESS if all port scans complete successfully, or EXIT_FAILURE if any scan fails.
 */
int scan_ports(void);

#endif
