#ifndef WAIT_H
#define WAIT_H

#include "network.h"

#include <sys/time.h>

/**
 * @brief Sets the start time for measuring timeouts.
 *
 * Captures the current time and stores it in the global `start_time` variable
 * to be used for timeout calculations.
 */
void set_time(void);

/**
 * @brief Checks if the allocated timeout period has expired.
 *
 * Compares the current time with `start_time` and calculates the remaining time.
 * Updates the `timeout` structure accordingly.
 *
 * @return true if the timeout has expired, false otherwise.
 */
bool out_of_time(void);

/**
 * @brief Waits for a response on the specified socket until the timeout expires.
 *
 * Uses the `select` system call to monitor the socket for incoming data.
 *
 * @param socket The socket type (TCP, UDP, ICMP) to monitor.
 * @return The result of `select()`, where:
 *         - >0 indicates data is available on the socket.
 *         - 0 indicates a timeout.
 *         - -1 indicates an error.
 */
int wait_response(sockets_t socket);

#endif
