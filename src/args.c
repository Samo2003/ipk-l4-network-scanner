#include "args.h"

static void parameters_setup(void) {
    parameters.interface = NULL;
    parameters.ip_or_domain = NULL;
    parameters.tcp_ports_count = 0;
    parameters.udp_ports_count = 0;
    parameters.timeout = 5000;
}

static bool port_in_range(int port) {
    return port >= 0 && port < UINT16_MAX;
}

static bool parse_ports(bool udp) {
    char *extra_ptr, extra;
    int start, end;

    uint16_t *ports = udp ? parameters.udp_ports : parameters.tcp_ports;
    int *port_count = udp ? &parameters.udp_ports_count : &parameters.tcp_ports_count;

    if (strchr(optarg, ',') != NULL) {
        char *token = strtok(optarg, ",");
        while (token != NULL) {
            int port = strtol(token, &extra_ptr, 10);
            if (extra_ptr[0] != '\0' || !port_in_range(port)) {
                fprintf(stderr, "ERROR: invalid port number\n");
                return false;
            }
            ports[(*port_count)++] = port;
            token = strtok(NULL, ",");
        }
    } else if (strchr(optarg, '-') != NULL) {
        if (sscanf(optarg, "%d-%d%c", &start, &end, &extra) != 2 || start > end || start < 0 || end > UINT16_MAX) {
            fprintf(stderr, "ERROR: invalid port range\n");
            return false;
        }
        for (int i = start; i <= end; i++) {
            ports[(*port_count)++] = i;
        }
    } else {
        int port = strtol(optarg, &extra_ptr, 10);
        if (extra_ptr[0] != '\0' || !port_in_range(port)) {
            fprintf(stderr, "ERROR: invalid port number\n");
            return false;
        }
        ports[(*port_count)++] = port;
    }

    return true;
}

static int test_args(int argc, char **argv) {
    for (int i = optind; i < argc; i++) {
        if (parameters.ip_or_domain == NULL) {
            parameters.ip_or_domain = argv[i];
        } else {
            fprintf(stderr, "ERROR: multiple domain names or IP addresses provided\n");
            return EXIT_FAILURE;
        }
    }

    if (parameters.ip_or_domain == NULL) {
        fprintf(stderr, "ERROR: missing domain name or IP address\n");
        return EXIT_FAILURE;
    }

    if (parameters.tcp_ports_count == 0 && parameters.udp_ports_count == 0) {
        fprintf(stderr, "ERROR: no ports provided\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int process_args(int argc, char **argv) {
    if (argc == 1) {
        return LIST_INTERFACES;
    }

    parameters_setup();

    static struct option long_opt[] = {
        {"interface", required_argument, 0, 'i'},
        {"pt", required_argument, 0, 't'},
        {"pu", required_argument, 0, 'u'},
        {"wait", required_argument, 0, 'w'},
        {0, 0, 0, 0}
    };

    int opt;
    bool only_opt = true, timeout_set = false;

    while ((opt = getopt_long(argc, argv, ":i:t:u:w:", long_opt, NULL)) != -1) {
        switch (opt) {
            case 'i':
                if (parameters.interface != NULL) {
                    fprintf(stderr, "ERROR: redefinition of interface\n");
                    return EXIT_FAILURE;
                } else if (optarg == NULL || optarg[0] == '-') {
                    fprintf(stderr, "ERROR: missing interface parameter\n");
                    return EXIT_FAILURE;
                } else {
                    parameters.interface = optarg;
                }
                break;
            case 't':
                only_opt = false;
                if (parameters.tcp_ports_count != 0) {
                    fprintf(stderr, "ERROR: redefinition of tcp ports\n");
                    return EXIT_FAILURE;
                } else if (!parse_ports(false)) {
                    return EXIT_FAILURE;
                }
                break;
            case 'u':
                only_opt = false;
                if (parameters.udp_ports_count != 0) {
                    fprintf(stderr, "ERROR: redefinition of udp ports\n");
                    return EXIT_FAILURE;
                } else if (!parse_ports(true)) {
                    return EXIT_FAILURE;
                }
                break;
            case 'w':
                only_opt = false;
                if (timeout_set) {
                    fprintf(stderr, "ERROR: redefinition of timeout\n");
                    return EXIT_FAILURE;
                }
                if (optarg == NULL) {
                    fprintf(stderr, "ERROR: mising timeout value\n");
                    return EXIT_FAILURE;
                }
                timeout_set = true;
                parameters.timeout = atoi(optarg);
                break;
            case ':':
                if (optopt == 'i') {
                    if (only_opt) {
                        return LIST_INTERFACES;
                    }
                    else {
                        fprintf(stderr, "ERROR: -i requires argument when other options are specified\n");
                        return EXIT_FAILURE;
                    }
                } else {
                    fprintf(stderr, "ERROR: -%c requires argument\n", optopt);
                    return EXIT_FAILURE;
                }
            default:
                break;
        }
    }

    return test_args(argc, argv);
}
