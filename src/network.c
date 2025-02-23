#include "network.h"

static void close_sockets(void) {
    for(int i = 0; i < SOCKET_COUNT; i++) {
        close(network.sockets[i]);
    }
}

static int setup_sockets(void) {
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

static int get_interface_ip(void) {
    struct ifaddrs *ifa_list, *ifa;

    if (getifaddrs(&ifa_list) == -1) {
        return EXIT_FAILURE;
    }

    for(ifa = ifa_list; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != network.family || strcmp(ifa->ifa_name, parameters.interface) != 0) {
            continue;
        }
        memcpy(&network.src, ifa->ifa_addr, ifa->ifa_addr->sa_family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6));
        freeifaddrs(ifa_list);
        return EXIT_SUCCESS;
    }
    freeifaddrs(ifa_list);
    return EXIT_FAILURE;
}

static struct addrinfo *get_dst_addresses(void) {
    struct addrinfo *res, hints = {0};
    if (getaddrinfo(parameters.ip_or_domain, NULL, &hints, &res) != 0) {
        perror("getaddrinfo");
        return NULL;
    }
    return res;
}

static uint16_t checksum(uint16_t *buffer, int buffer_len) {
    uint32_t sum = 0;

    while(buffer_len > 1) {
        sum += *buffer++;
        buffer_len -= 2;
    }

    if (buffer_len == 1) {
        sum += *(uint8_t *)buffer;
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return (uint16_t)(~sum);
}

static uint16_t ipv4_checksum(uint16_t *header, int header_size, uint8_t protocol) {
    ipv4_psh_t psh = {0};
    psh.src = ((struct sockaddr_in *)&network.src)->sin_addr.s_addr;
    psh.dst = ((struct sockaddr_in *)&network.dst)->sin_addr.s_addr;
    psh.protocol = protocol;
    psh.header_len = header_size;

    char buffer[sizeof(ipv4_psh_t) + header_size];
    memcpy(buffer, &psh, sizeof(ipv4_psh_t));
    memcpy(buffer + sizeof(ipv4_psh_t), header, header_size);

    return checksum((uint16_t *)buffer, sizeof(buffer));
}

static uint16_t ipv6_checksum(uint16_t *header, int header_size, uint8_t protocol) {
    ipv6_psh_t psh = {0};
    memcpy(&psh.src, &((struct sockaddr_in6 *)&network.src)->sin6_addr, sizeof(struct in6_addr));
    memcpy(&psh.dst, &((struct sockaddr_in6 *)&network.dst)->sin6_addr, sizeof(struct in6_addr));
    psh.header_len = header_size;
    psh.next_header = protocol;

    char buffer[sizeof(ipv6_psh_t) + header_size];
    memcpy(buffer, &psh, sizeof(ipv6_psh_t));
    memcpy(buffer + sizeof(ipv6_psh_t), header, header_size);

    return checksum((uint16_t *)buffer, sizeof(buffer));
}

int list_active_interfaces(void) {
    struct ifaddrs *ifa_list, *ifa;
    list_t printed_ifa = list_init();

    if (getifaddrs(&ifa_list) == -1) {
        perror("getifaddrs");
        return EXIT_FAILURE;
    }

    fprintf(stdout, "List of active interfaces:\n");
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

int network_setup(bool first_setup) {
    struct addrinfo *addr, *tmp;
    if (first_setup) {
        network.scanned_ip_adresses = list_init();
        network.list = get_dst_addresses();
        tmp = network.list;
    } else {
        tmp = network.active;
    }
    for(addr = tmp; addr != NULL; addr = addr->ai_next) {
        network.family = addr->ai_family;
        memcpy(&network.dst, addr->ai_addr, addr->ai_addrlen);
        char ip_str[INET6_ADDRSTRLEN] = {0};
        inet_ntop(network.family, network.family == AF_INET ? (void *)&((struct sockaddr_in *)&network.dst)->sin_addr : (void *)&((struct sockaddr_in6 *)&network.dst)->sin6_addr, ip_str, INET6_ADDRSTRLEN);
        
        if (list_contains(&network.scanned_ip_adresses, ip_str) || get_interface_ip() != EXIT_SUCCESS) {
            continue;
        }
        if (list_insert(&network.scanned_ip_adresses, ip_str) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
        }

        network.active = addr->ai_next;
        return setup_sockets();
    }

    return EXIT_FAILURE;
}

void network_clean_up(void) {
    list_destroy(&network.scanned_ip_adresses);
    freeaddrinfo(network.list);
    close_sockets();
}

int next_ip(void) {
    close_sockets();
    return network_setup(false);
}

uint16_t calculate_checksum(uint16_t *header, int header_size, uint8_t protocol) {
    return (network.family == AF_INET ? ipv4_checksum(header, header_size, protocol) : ipv6_checksum(header, header_size, protocol));
}
