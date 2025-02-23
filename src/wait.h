#ifndef WAIT_H
#define WAIT_H

#include "network.h"

#include <sys/time.h>

void set_time(void);
bool out_of_time(void);
int wait_response(sockets_t socket);

#endif
