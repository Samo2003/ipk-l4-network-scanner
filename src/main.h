#ifndef MAIN_H
#define MAIN_H

#include <ifaddrs.h>
#include <net/if.h>

#include "args.h"
#include "list.h"


int main(int argc, char **argv);
int list_active_interfaces(void);

#endif
