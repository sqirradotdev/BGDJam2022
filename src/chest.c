#include "chest.h"

#include <stdlib.h>

Chest* chest_new()
{
    Chest* chest = (Chest*)malloc(sizeof(Chest));
    chest->position = (Vector2) { 0.0, 0.0 };
    chest->opened = false;

    return chest;
}

void chest_draw(Chest* chest)
{
    Rectangle src_rect = { 128.0, chest->opened ? 32.0 : 0.0, 16.0, 16.0};
    Rectangle dst_rect = { chest->position.x, chest->position.y, 16.0, 16.0};
    DrawTexturePro(chest->texture, src_rect, dst_rect, (Vector2) { 0.0, 0.0 }, 0.0, WHITE);
}

void chest_destroy(Chest* chest)
{
    free(chest);
}
