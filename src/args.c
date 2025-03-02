/**
 * @file args.c
 * @brief Implementation of command-line argument processing.
 *
 * This source file contains the implementation of functions for processing and 
 * validating command-line arguments.
 *
 * @author Samuel Stefanik (xstefas00)
 * @date   2025-02-13
 */
#include "args.h"

/**
 * @brief Initializes the parameters structure with default values.
 * 
 */
static void parameters_setup(void) {
    parameters.interface = NULL;
    parameters.ip_or_domain = NULL;
    parameters.tcp_ports_count = 0;
    parameters.udp_ports_count = 0;
    parameters.timeout = 5000;
}

/**
 * @brief Checks if a given port number is within the valid range.
 * 
 * @param port The port number to check.
 * @return true if the port is within the valid range, false otherwise.
 */
static bool port_in_range(int port) {
    return port >= 0 && port < UINT16_MAX;
}

/**
 * @brief Parses the port numbers from the input argument and stores them in the appropriate array.
 * 
 * @param udp A boolean indicating whether to parse UDP ports (true) or TCP ports (false).
 * @return true if all ports were parsed successfully, false if an error occurred.
 */
static bool parse_ports(bool udp) {
    char *extra_ptr, extra;
    int start, end, commas = 0;

    uint16_t *ports = udp ? parameters.udp_ports : parameters.tcp_ports;
    int *port_count = udp ? &parameters.udp_ports_count : &parameters.tcp_ports_count;

    if (strchr(optarg, ',') != NULL) {
        char *str = optarg;
        // Counting number of commas
        while ((str = strchr(str, ',')) != NULL) {
            commas++;
            str++;
        }
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
        
        // More commas than ports
        if (commas != *port_count - 1) {
            fprintf(stderr, "ERROR: invalid port number\n");
            return false;
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

/**
 * @brief Validates and processes command-line arguments after option parsing.
 * 
 * @param argc The argument count.
 * @param argv The argument vector.
 * @return EXIT_SUCCESS if the arguments are valid, otherwise EXIT_FAILURE.
 */
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

/**
 * @brief Parses and processes command-line arguments.
 * 
 * @param argc The argument count.
 * @param argv The argument vector.
 * @return LIST_INTERFACES if no arguments are given, EXIT_SUCCESS on success, or EXIT_FAILURE on failure.
 */
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
                fprintf(stderr, "ERROR: unknown parameter -%c\n", optopt);
                return EXIT_FAILURE;
        }
    }

    return test_args(argc, argv);
}
