#include "main.h"

parameters_t parameters = {0};
network_t network = {0};

int main (int argc, char **argv) {
    int res = process_args(argc, argv);
    if (res == EXIT_FAILURE) {
        return EXIT_FAILURE;
    } else if (res == LIST_INTERFACES) {
        return list_active_interfaces();
    }

    if (network_setup() != EXIT_SUCCESS) {
        network_clean_up();
        return EXIT_FAILURE;
    }

    res = scan_ports();
    network_clean_up();    
    return res;
}
