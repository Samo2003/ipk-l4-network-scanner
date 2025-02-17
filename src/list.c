#include "list.h"

list_t list_init(void) {
    list_t list = {NULL, 0};
    return list;
}

int list_insert(list_t *list, char *name) {
    list->elements = realloc(list->elements, (list->elem_count + 1) * sizeof(char *));
    if (list->elements == NULL) {
        perror("realloc list");
        return EXIT_FAILURE;
    }
    list->elements[list->elem_count++] = strdup(name);
    
    return EXIT_SUCCESS;
}

bool list_contains(list_t *list, char *name) {
    for(int i = 0; i < list->elem_count; i++) {
        if (strcmp(list->elements[i], name) == 0) {
            return true;
        }
    }
    return false;
}

void list_destroy(list_t *list) {
    for(int i = 0; i < list->elem_count; i++) {
        free(list->elements[i]);
    }
    free(list->elements);
}
