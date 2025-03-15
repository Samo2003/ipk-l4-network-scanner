#include "../lib/wait.h"

struct timeval start_time, timeout;

/**
 * @brief Sets the start time for measuring timeouts.
 *
 */
void set_time(void) {
    gettimeofday(&start_time, NULL);
}

/**
 * @brief Checks if the allocated timeout period has expired.
 *
 * @return true if the timeout has expired, false otherwise.
 */
bool out_of_time(void) {
    struct timeval time_now = {0};
    gettimeofday(&time_now, NULL);
    long time_passed = parameters.timeout - ((time_now.tv_sec - start_time.tv_sec) * 1000 + (time_now.tv_usec - start_time.tv_usec) / 1000);
    timeout.tv_sec = time_passed / 1000;
    timeout.tv_usec = (time_passed % 1000) * 1000;
    return time_passed <= 0;
}

/**
 * @brief Waits for a response on the specified socket until the timeout expires.
 *
 * @param socket The socket type (TCP, UDP, ICMP) to monitor.
 * @return The result of `select()`, where:
 *         - >0 indicates data is available on the socket.
 *         - 0 indicates a timeout.
 *         - -1 indicates an error.
 */
int wait_response(sockets_t socket) {
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(network.sockets[socket], &fd);
    return select(FD_SETSIZE, &fd, NULL, NULL, &timeout);
}
