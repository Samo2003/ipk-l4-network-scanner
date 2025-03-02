/**
 * @file main.c
 * @brief Main entry point for the network scanner application.
 *
 * This file contains the main function that initializes the network setup, 
 * processes command-line arguments, handles signals like keyboard interrupts, 
 * and orchestrates the scanning of network ports. It includes setup functions 
 * for signal handling, command-line argument parsing, network initialization, 
 * and execution of the port scanning process.
 *
 * @author Samuel Stefanik (xstefas00)
 * @date   2025-02-13
 */
#include "scan.h"
#include <signal.h>

parameters_t parameters = {0};  /**< Global structure for command-line parameters. */
network_t network = {0};        /**< Global structure for network setup and management. */

/**
 * @brief Signal handler for interrupt signals.
 *
 * This function is called when the process receives a SIGINT signal. It cleans 
 * up network resources and terminates the program.
 *
 * @param num The signal number.
 */
void interupt(int num) {
    fprintf(stdout, "Keyboard interupt: %d\n", num);
    network_clean_up();
    exit(EXIT_FAILURE);
}

/**
 * @brief Main function for network scanning.
 *
 * This is the entry point of the program. It processes the command-line arguments, 
 * handles the network setup, and initiates the port scanning process.
 *
 * @param argc The argument count.
 * @param argv The argument vector.
 * @return EXIT_SUCCESS on successful execution, EXIT_FAILURE on error.
 */
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
