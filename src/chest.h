#ifndef _CHEST_H_
#define _CHEST_H_

#include <raylib.h>
#include <stdbool.h>

typedef struct Chest
{
    Vector2 position;
    bool opened;
    Texture2D texture;
} Chest;

Chest* chest_new();
void chest_draw(Chest* chest);
void chest_destroy(Chest* chest);

#endif
