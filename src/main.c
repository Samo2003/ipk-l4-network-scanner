#include "scan.h"
#include <signal.h>

parameters_t parameters = {0};
network_t network = {0};

void interupt(int num) {
    fprintf(stdout, "Keyboard interupt: %d\n", num);
    network_clean_up();
    exit(1);
}

int main (int argc, char **argv) {
    signal(SIGINT, interupt);

    int res = process_args(argc, argv);
    if (res == EXIT_FAILURE) {
        return EXIT_FAILURE;
    } else if (res == LIST_INTERFACES) {
        return list_active_interfaces();
    }

    if (network_setup(true) != EXIT_SUCCESS) {
        network_clean_up();
        return EXIT_FAILURE;
    }

    res = scan_ports();
    network_clean_up();    
    return res;
}
