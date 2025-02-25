#include "scan.h"

static int scan(int port, bool tcp) {
    if ((tcp ? send_tcp_msg(port) : send_udp_msg(port)) < 0) {
        perror("sendto");
        return EXIT_FAILURE;
    }

    set_time();
    char rcv_buffer[BUFFER_SIZE];
    struct sockaddr_storage place_holder = {0};
    socklen_t place_holder_length = sizeof(struct sockaddr_storage);
    sockets_t socket = tcp ? TCP : ICMP;

    bool tcp_retry = tcp;
    while(true) {
        if (out_of_time()) {
            if (tcp_retry) {
                tcp_retry = false;
                set_time();
                continue;
            }
            break;
        }
        if (wait_response(socket) > 0) {
            size_t b_rcv = recvfrom(network.sockets[socket], rcv_buffer, BUFFER_SIZE, 0, (struct sockaddr *)&place_holder, &place_holder_length);
            if (b_rcv > 0) {
                if ((tcp ? handle_tcp_msg(rcv_buffer, b_rcv, port) : handle_udp_msg(rcv_buffer, b_rcv, port)) == EXIT_SUCCESS) {
                    return EXIT_SUCCESS;
                }
            }
        }
    }

    tcp ? fprintf(stdout, "%d/tcp\tfiltered\n", port) : fprintf(stdout, "%d/udp\topen\n", port);
    return EXIT_SUCCESS;
}

int scan_ports(void) {
    do {
        fprintf(stdout, "Scanning ports for: %s\nPORT\tSTATE\n", network.scanned_ip_adresses.elements[network.scanned_ip_adresses.elem_count-1]);

        for(int i = 0; i < parameters.tcp_ports_count; i++) {
            if (scan(parameters.tcp_ports[i], true) != EXIT_SUCCESS) {
                return EXIT_FAILURE;
            }
        }

        for(int i = 0; i < parameters.udp_ports_count; i++) {
            if (scan(parameters.udp_ports[i], false) != EXIT_SUCCESS) {
                return EXIT_FAILURE;
            }
        }
    } while(next_ip() != EXIT_FAILURE);
    return EXIT_SUCCESS;
}
