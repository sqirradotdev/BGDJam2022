#include <raylib.h>
#include <stdlib.h>

#include "core/gameloop.h"
#include "util/list.h"

int main(int argc, char **argv)
{
    /* int t1 = 10;
    int t2 = 20;
    int t3 = 30;

    List* test = list_new();
    list_push(test, &t1);
    list_push(test, &t2);
    list_push(test, &t3);

    list_pop(test);
    list_pop(test);

    for (int i = 0; i < test->length; i++)
    {
        TraceLog(LOG_INFO, "%i", *((int*)list_get(test, i)));
    }

    return 0; */

    if (!gameloop_init())
        return EXIT_FAILURE;

    if (!gameloop_loop())
        return EXIT_FAILURE;

    gameloop_clean();

    return EXIT_SUCCESS;
}
