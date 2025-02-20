#include "network.h"

int network_setup(void) {
    struct addrinfo *addr, *list = get_dst_addresses();
    for(addr = list; addr != NULL; addr = addr->ai_next) {
        network.family = addr->ai_family;
        if (get_interface_ip() != EXIT_SUCCESS) {
            continue;
        }
        memcpy(&network.dst, addr->ai_addr, sizeof(struct sockaddr));
        freeaddrinfo(list);
        return setup_sockets();
    }

    fprintf(stderr, "ERROR: source or destination ip adress not found\n");
    freeaddrinfo(list);
    return EXIT_FAILURE;
}

int setup_sockets(void) {
    network.sockets[TCP] = socket(network.family, SOCK_RAW, IPPROTO_TCP);
    network.sockets[UDP] = socket(network.family, SOCK_RAW, IPPROTO_UDP);
    network.sockets[ICMP] = socket(network.family, SOCK_RAW, network.family == AF_INET ? IPPROTO_ICMP : IPPROTO_ICMPV6);
    if (network.sockets[TCP] == -1 || network.sockets[UDP] == -1 || network.sockets[ICMP] == -1) {
        fprintf(stderr, "ERROR: socket init failed\n");
        close_sockets();
        return EXIT_FAILURE;
    }

    for(int i = 0; i < SOCKET_COUNT; i++) {
        if (setsockopt(network.sockets[i], SOL_SOCKET, SO_BINDTODEVICE, parameters.interface, strlen(parameters.interface)) == -1) {
            perror("setsockopt");
            close_sockets();
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

void network_clean_up(void) {
    close_sockets();
}

void close_sockets(void) {
    for(int i = 0; i < SOCKET_COUNT; i++) {
        close(network.sockets[i]);
    }
}

int get_interface_ip(void) {
    struct ifaddrs *ifa_list, *ifa;

    if (getifaddrs(&ifa_list) == -1) {
        return EXIT_FAILURE;
    }

    for(ifa = ifa_list; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != network.family || strcmp(ifa->ifa_name, parameters.interface) != 0) {
            memcpy(&network.src, ifa->ifa_addr, sizeof(struct sockaddr));
            freeifaddrs(ifa_list);
            return EXIT_SUCCESS;
        }
    }
    freeifaddrs(ifa_list);
    return EXIT_FAILURE;
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
        
        fprintf(stdout, "%s\n", ifa->ifa_name);
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

struct addrinfo *get_dst_addresses(void) {
    struct addrinfo *res, hints = {0};
    if (getaddrinfo(parameters.ip_or_domain, NULL, &hints, &res) != 0) {
        perror("getaddrinfo");
        return NULL;
    }
    return res;
}
