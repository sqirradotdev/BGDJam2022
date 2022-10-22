#ifndef _GAMELOOP_H_
#define _GAMELOOP_H_

#include <stdbool.h>
#include <raylib.h>

bool gameloop_init();
bool gameloop_loop();
void gameloop_clean();

Vector2 gameloop_get_viewport_size();

#endif
