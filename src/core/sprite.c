#include "sprite.h"

#include <stdlib.h>
#include <math.h>

Sprite* sprite_new(Texture2D texture)
{
    Sprite* sprite = (Sprite*)malloc(sizeof(Sprite));
    sprite->position = (Vector2) { 0.0, 0.0 };
    sprite->scale = (Vector2) { 1.0, 1.0 };
    sprite->rotation = 0.0;
    sprite->origin = (Vector2) { 0.0, 0.0 };
    sprite->flip_x = false;
    sprite->flip_y = false;
    sprite->color = WHITE;
    sprite->src_rect = (Rectangle) { 0.0, 0.0, texture.width, texture.height };
    sprite->texture = texture;
    sprite->blend_mode = BLEND_ALPHA;

    return sprite;
}

void sprite_draw(Sprite* sprite)
{
    BeginBlendMode(sprite->blend_mode);

    Rectangle src_rect = {
        sprite->src_rect.x,
        sprite->src_rect.y,
        (sprite->flip_x ? -1.0 : 1.0) * sprite->src_rect.width,
        (sprite->flip_y ? -1.0 : 1.0) * sprite->src_rect.height
    };
    Rectangle dst_rect = {
        floorf(sprite->position.x - (sprite->origin.x * (sprite->scale.x - 1.0))),
        floorf(sprite->position.y - (sprite->origin.y * (sprite->scale.y - 1.0))),
        (float)sprite->src_rect.width * sprite->scale.x,
        (float)sprite->src_rect.height * sprite->scale.y
    };

    DrawTexturePro(sprite->texture, src_rect, dst_rect, sprite->origin, sprite->rotation, sprite->color);
}

void sprite_set_origin_normalized(Sprite* sprite, Vector2 origin)
{
    sprite->origin = (Vector2) { origin.x * sprite->texture.width, origin.y * sprite->texture.height };
}

void sprite_center_origin(Sprite* sprite)
{
    sprite_set_origin_normalized(sprite, (Vector2) { 0.5, 0.5 });
}

void sprite_destroy(Sprite* sprite)
{
    free(sprite);
}
