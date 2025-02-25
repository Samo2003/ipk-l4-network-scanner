#include "wait.h"

struct timeval start_time, timeout;

void set_time(void) {
    gettimeofday(&start_time, NULL);
}

bool out_of_time(void) {
    struct timeval time_now = {0};
    gettimeofday(&time_now, NULL);
    long time_passed = parameters.timeout - ((time_now.tv_sec - start_time.tv_sec) * 1000 + (time_now.tv_usec - start_time.tv_usec) / 1000);
    timeout.tv_sec = time_passed / 1000;
    timeout.tv_usec = (time_passed % 1000) * 1000;
    return time_passed <= 0;
}

int wait_response(sockets_t socket) {
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(network.sockets[socket], &fd);
    return select(FD_SETSIZE, &fd, NULL, NULL, &timeout);
}
