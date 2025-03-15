/**
 * @file args.h
 * @brief Header file for command-line argument processing.
 *
 * This header file contains the definition of the parameters_t structure and the 
 * declarations of functions for processing and validating command-line arguments
 * for the network scanner project.
 *
 * @author Samuel Stefanik (xstefas00)
 * @date   2025-02-13
 */
#ifndef ARGS_H
#define ARGS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_PORTS UINT16_MAX
#define LIST_INTERFACES 2

/**
 * @brief Stores parsed command-line parameters.
 */
typedef struct parameters {
    char *interface;                 /**< Network interface to use */
    uint16_t tcp_ports[MAX_PORTS];   /**< List of TCP ports to scan */
    int tcp_ports_count;             /**< Number of TCP ports specified */
    uint16_t udp_ports[MAX_PORTS];   /**< List of UDP ports to scan */
    int udp_ports_count;             /**< Number of UDP ports specified */
    int timeout;                     /**< Timeout value for scanning */
    char *ip_or_domain;              /**< Target IP address or domain name */
} parameters_t;

extern parameters_t parameters;

/**
 * @brief Processes command-line arguments and initializes parameters.
 * 
 * This function parses the command-line arguments using getopt_long, ensuring 
 * that valid options are provided and that necessary parameters (interface, ports, timeout) 
 * are correctly set.
 * 
 * @param argc The argument count.
 * @param argv The argument vector.
 * @return LIST_INTERFACES if no parameters are provided, EXIT_SUCCESS on success, or EXIT_FAILURE on error.
 */
int process_args(int argc, char **argv);

#endif
