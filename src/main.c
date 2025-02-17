#include "main.h"

parameters_t parameters;

int main (int argc, char **argv) {
    int res = process_args(argc, argv);
    if (res == EXIT_FAILURE) {
        return 1;
    } else if (res == LIST_INTERFACES) {
        printf("interfaces\n");
        // list_active_interfaces();
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
