#ifndef _CRATE_H_
#define _CRATE_H_

#include <raylib.h>

#include "cLDtk.h"

#define CRATE_TERMINAL_VELOCITY 4

typedef enum
{
    CRATE_SMALL,
    CRATE_BIG
} CrateType;

typedef struct Crate
{
    Vector2 position;
    Vector2 velocity;

    CrateType type;

    Texture2D texture;
    Rectangle current_rect;
} Crate;

Crate* crate_new(CrateType type);
void crate_update(Crate* crate, struct layerInstances* map_col_layer, Crate** crates_ptr, int crate_size, bool mouse_down, Vector2 mouse_pos, Vector2 mouse_vel);
void crate_draw(Crate* crate);
void crate_destroy(Crate* crate);

#endif
