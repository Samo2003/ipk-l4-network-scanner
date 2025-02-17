#ifndef ARGS
#define ARGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_PORTS 65536
#define LIST_INTERFACES 2

typedef struct parameters {
    char *interface;
    uint16_t tcp_ports[MAX_PORTS];
    int tcp_ports_count;
    uint16_t udp_ports[MAX_PORTS];
    int udp_ports_count;
    int timeout;
    char *ip_or_domain;
} parameters_t;

extern parameters_t parameters;

int process_args(int argc, char **argv);
bool port_in_range(int port);
bool parse_ports(bool udp);
int test_args(int argc, char **argv);
void parameters_setup(void);

#endif
