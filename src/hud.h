#ifndef _HUD_H_
#define _HUD_H_

#include <raylib.h>

#include "player.h"
#include "util/list.h"

typedef struct HUD
{
    Player* player;
    Texture2D hearts_texture;

    List* sprite_list;
} HUD;

#endif