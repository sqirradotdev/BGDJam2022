#include "crate.h"

#include <stdlib.h>

#include "constants.h"

Crate* crate_new(CrateType type)
{
    Crate* crate = (Crate*)malloc(sizeof(Crate));

    crate->position = (Vector2) { 0.0, 0.0 };
    crate->velocity = (Vector2) { 0.0, 0.0 };

    crate->type = type;

    if (type == CRATE_SMALL)
        crate->current_rect = (Rectangle) { 0.0, 0.0, 16.0, 16.0 };
    else if (type == CRATE_BIG)
        crate->current_rect = (Rectangle) { 0.0, 0.0, 32.0, 32.0 };
}

void crate_update(Crate* crate, struct layerInstances* map_col_layer)
{
    crate->current_rect.x = crate->position.x;
    crate->current_rect.y = crate->position.y;
}

void crate_draw(Crate* crate)
{
    Rectangle src_rect;
    if (crate->type == CRATE_SMALL)
        src_rect = (Rectangle) { 128.0, 48.0, 16.0, 16.0};
    else if (crate->type == CRATE_BIG)
        src_rect = (Rectangle) { 112.0, 64.0, 32.0, 32.0};

    DrawTexturePro(crate->texture, src_rect, crate->current_rect, (Vector2) { 0.0, 0.0 }, 0.0, WHITE);
}

void crate_destroy(Crate* crate)
{
    
}

static void _check_collision()
{

}
