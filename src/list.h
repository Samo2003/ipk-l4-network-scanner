#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

typedef struct list {
    char **elements;
    int elem_count;
} list_t;

list_t list_init(void);
int list_insert(list_t * list, char *name);
bool list_contains(list_t *list, char *name);
void list_destroy(list_t *list);

#endif
