#include "list.h"

#include <raylib.h>

#define ALLOC_STEP 2

static void _list_alloc_more(List* list);

List* list_new()
{
    List* list = (List*)malloc(sizeof(List));
    list->elements = (void**)malloc(ALLOC_STEP * sizeof(void*));
    list->allocated = ALLOC_STEP;
    list->length = 0;

    return list;
}

void list_push(List* list, void* data)
{
    if ((list->length + 1) % ALLOC_STEP == 0)
        _list_alloc_more(list);

    list->elements[list->length] = data;
    list->length++;
}

void* list_pop(List* list)
{
    list->length--;
    void* popped = list->elements[list->length];
    list->elements[list->length] = NULL;

    return popped;
}

void* list_get(List* list, size_t idx)
{
    return list->elements[idx];
}

int list_find(List* list, void* data)
{
    for (int i = 0; i < list->length; i++)
    {
        if (list->elements[i] == data)
            return i;
    }

    return -1;
}

static void _list_alloc_more(List* list)
{
    int alloc_size = list->allocated + ALLOC_STEP;
    list->elements = (void**)realloc(list->elements, alloc_size * sizeof(void*));
    list->allocated = alloc_size;
}
