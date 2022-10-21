#ifndef _LIST_H_
#define _LIST_H

#include <stdlib.h>

typedef struct List
{
    void** elements;
    size_t allocated;
    size_t length;
} List;

List* list_new();
void list_push(List* list, void* data);
void* list_pop(List* list);
void* list_get(List* list, size_t idx);

#endif
