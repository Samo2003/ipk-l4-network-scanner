/**
 * @file list.c
 * @brief Implementation of functions for managing a dynamic list of strings.
 *
 * This file implements the functions for manipulating a dynamic list of strings 
 * using the `list_t` structure. 
 *
 * @author Samuel Stefanik (xstefas00)
 * @date   2025-02-17
 */
#include "list.h"

/**
 * @brief Initializes an empty list.
 * 
 * @return A new list_t structure.
 */
list_t list_init(void) {
    list_t list = {NULL, 0};
    return list;
}

/**
 * @brief Inserts an element into the list.
 * 
 * @param list Pointer to the list structure.
 * @param name The string to be added to the list.
 * @return EXIT_SUCCESS on success, or EXIT_FAILURE if memory allocation fails.
 */
int list_insert(list_t *list, char *name) {
    list->elements = realloc(list->elements, (list->elem_count + 1) * sizeof(char *));
    if (list->elements == NULL) {
        perror("realloc list");
        return EXIT_FAILURE;
    }
    list->elements[list->elem_count++] = strdup(name);
    
    return EXIT_SUCCESS;
}

/**
 * @brief Checks if an element exists in the list.
 * 
 * @param list Pointer to the list structure.
 * @param name The string to check for in the list.
 * @return true if the name is found in the list, false otherwise.
 */
bool list_contains(list_t *list, char *name) {
    for(int i = 0; i < list->elem_count; i++) {
        if (strcmp(list->elements[i], name) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Frees the memory used by the list and its elements.
 * 
 * @param list Pointer to the list structure that needs to be destroyed.
 */
void list_destroy(list_t *list) {
    for(int i = 0; i < list->elem_count; i++) {
        free(list->elements[i]);
    }
    if (list->elements != NULL) {
        free(list->elements);
    }
}
