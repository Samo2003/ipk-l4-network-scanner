#include "main.h"

parameters_t parameters;

int main (int argc, char **argv) {
    int res = process_args(argc, argv);
    if (res == EXIT_FAILURE) {
        return 1;
    } else if (res == LIST_INTERFACES) {
        return list_active_interfaces();
    }

    return EXIT_SUCCESS;
}

int list_active_interfaces(void) {
    struct ifaddrs *ifa_list, *ifa;
    list_t printed_ifa = list_init();

    if (getifaddrs(&ifa_list) == -1) {
        perror("getifaddrs");
        return EXIT_FAILURE;
    }

    for(ifa = ifa_list; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL || !(ifa->ifa_flags & IFF_UP) || list_contains(&printed_ifa, ifa->ifa_name)) {
            continue;
        }
        
        printf("%s\n", ifa->ifa_name);
        if (list_insert(&printed_ifa, ifa->ifa_name) == EXIT_FAILURE) {
            list_destroy(&printed_ifa);
            freeifaddrs(ifa_list);
            return EXIT_FAILURE;
        }
    }

    list_destroy(&printed_ifa);
    freeifaddrs(ifa_list);
    return EXIT_SUCCESS;
}
