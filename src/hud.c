#include "hud.h"

HUD* hud_new(Player* player)
{
    HUD* hud = (HUD*)malloc(sizeof(HUD));
    hud->player = player;
    hud->hearts_texture = LoadTexture("./resources/textures/hearts.png");

    return hud;
}


