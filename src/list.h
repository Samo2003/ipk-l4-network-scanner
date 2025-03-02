/**
 * @file list.h
 * @brief Header file for a dynamic list implementation.
 *
 * This header file contains the definition of the `list_t` structure and the 
 * declarations of functions for managing a dynamic list of strings.
 * 
 * @author Samuel Stefanik (xstefas00)
 * @date   2025-02-17
 */
#ifndef LIST_H
#define LIST_H

#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

/**
 * @brief A structure representing a dynamic list of strings.
 */
typedef struct list {
    char **elements; /**< Pointer to an array of strings stored in the list. */
    int elem_count;  /**< The number of elements currently stored in the list. */
} list_t;

/**
 * @brief Initializes an empty list.
 * 
 * This function initializes a list structure with no elements and a count of 0.
 * 
 * @return A new list_t structure.
 */
list_t list_init(void);

/**
 * @brief Inserts an element into the list.
 * 
 * This function reallocates memory for the list's elements and adds a new 
 * string (name) to the list.
 * 
 * @param list Pointer to the list structure.
 * @param name The string to be added to the list.
 * @return EXIT_SUCCESS on success, or EXIT_FAILURE if memory allocation fails.
 */
int list_insert(list_t * list, char *name);

/**
 * @brief Checks if an element exists in the list.
 * 
 * This function iterates through the list and checks if a given string (name) 
 * is present in the list of elements.
 * 
 * @param list Pointer to the list structure.
 * @param name The string to check for in the list.
 * @return true if the name is found in the list, false otherwise.
 */
bool list_contains(list_t *list, char *name);

/**
 * @brief Frees the memory used by the list and its elements.
 * 
 * This function iterates through the list, freeing each string element.
 * 
 * @param list Pointer to the list structure that needs to be destroyed.
 */
void list_destroy(list_t *list);

#endif
