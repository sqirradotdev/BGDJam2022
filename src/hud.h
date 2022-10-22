#ifndef _HUD_H_
#define _HUD_H_

#include <raylib.h>

#include "player.h"
#include "util/list.h"

typedef struct HUD
{
    Player* player;
    Texture2D hearts_texture;

    int h_blink_frames_counter;
} HUD;

HUD* hud_new(Player* player);
void hud_update(HUD* hud);
void hud_draw(HUD* hud);
void hud_destroy(HUD* hud);

#endif
