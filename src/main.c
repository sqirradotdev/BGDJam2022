#include <raylib.h>
#include <stdlib.h>

#include "core/gameloop.h"
#include "util/list.h"

int main(int argc, char **argv)
{
    if (!gameloop_init())
        return EXIT_FAILURE;

    if (!gameloop_loop())
        return EXIT_FAILURE;

    gameloop_clean();

    return EXIT_SUCCESS;
}
