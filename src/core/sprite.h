#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <raylib.h>
#include <stdbool.h>

typedef struct Sprite
{
    Vector2 position;
    Vector2 scale;
    float rotation;
    Vector2 origin;

    bool flip_x;
    bool flip_y;

    Color color;

    Rectangle src_rect;

    Texture2D texture;
    BlendMode blend_mode;
} Sprite;

Sprite* sprite_new(Texture2D texture);
void sprite_draw(Sprite* sprite);
void sprite_destroy(Sprite* sprite);
void sprite_set_origin_normalized(Sprite* sprite, Vector2 origin);
void sprite_center_origin(Sprite* sprite);

#endif
