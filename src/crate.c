#include "crate.h"

#include <stdlib.h>
#include <math.h>

#include "constants.h"

static Rectangle cur_crate_rect;
static Rectangle cur_crate_future_x;
static Rectangle cur_crate_future_y;
static bool cur_collided_x = false;
static bool cur_collided_y = false;

static void _begin_update_physics(Rectangle crate_rect, Vector2* position, Vector2* velocity);
static bool _update_physics(Crate* player, Rectangle other_rect, Vector2* position, Vector2* velocity);

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

void crate_update(Crate* crate, struct layerInstances* map_col_layer, Crate** crates_ptr, int crate_size, bool mouse_down, Vector2 mouse_vel)
{
    crate->current_rect.x = crate->position.x;
    crate->current_rect.y = crate->position.y;

    Vector2 temp_velocity = crate->velocity;
    Vector2 temp_position = crate->position;

    if (mouse_down)
    {
        temp_velocity = mouse_vel;
    }
    else
    {
        temp_velocity.y += GRAVITY;
        if (fabs(temp_velocity.y) > CRATE_TERMINAL_VELOCITY)
            temp_velocity.y = CRATE_TERMINAL_VELOCITY * copysignf(1.0, temp_velocity.y);
    }

    _begin_update_physics(crate->current_rect, &temp_position, &temp_velocity);
    for (int y = map_col_layer->autoTiles_data_ptr->count; y-- > 0;)
    {
        Rectangle tile_rect = {
            (float)map_col_layer->autoTiles_data_ptr[y].x,
            (float)map_col_layer->autoTiles_data_ptr[y].y,
            TILE_OFFSET,
            TILE_OFFSET
        };

        if (_update_physics(crate, tile_rect, &temp_position, &temp_velocity))
            break;
    }
    for (int i = 0; i < crate_size; i++)
    {
        if (crates_ptr[i] == NULL)
            continue;

        if (crates_ptr[i] == crate)
            continue;

        if (_update_physics(crate, crates_ptr[i]->current_rect, &temp_position, &temp_velocity))
            break;
    }

    // if (temp_position.x < 0.0)
    // {
    //     temp_position.x = 0.0;
    //     temp_velocity.x = 0.0;
    // }
    // if (temp_position.x > player->level_size.x)
    // {
    //     temp_position.x = player->level_size.x;
    //     temp_velocity.x = 0.0;
    // }
    // if (temp_position.y < 0.0)
    // {
    //     temp_position.y = 0.0;
    //     temp_velocity.y = 0.0;
    // }
    // if (temp_position.y > player->level_size.y)
    // {
    //     temp_position.y = player->level_size.y;
    //     temp_velocity.y = 0.0;
    // }

    temp_position.x += temp_velocity.x;
    temp_position.y += temp_velocity.y;

    crate->position = temp_position;
    crate->velocity = temp_velocity;

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

static void _begin_update_physics(Rectangle crate_rect, Vector2* position, Vector2* velocity)
{
    cur_collided_x = false;
    cur_collided_y = false;

    cur_crate_rect = crate_rect;

    cur_crate_future_x = (Rectangle) { cur_crate_rect.x + velocity->x, cur_crate_rect.y, cur_crate_rect.width, cur_crate_rect.height };
    cur_crate_future_y = (Rectangle) { cur_crate_rect.x, cur_crate_rect.y + velocity->y, cur_crate_rect.width, cur_crate_rect.height };
}

static bool _update_physics(Crate* player, Rectangle other_rect, Vector2* position, Vector2* velocity)
{
    if (CheckCollisionRecs(cur_crate_future_x, other_rect))
    {
        Rectangle col_rec = GetCollisionRec(cur_crate_future_x, other_rect);

        //temp_position.x += temp_velocity.x;

        if (velocity->x > 0.0)
            position->x -= col_rec.width;
        else
            position->x += col_rec.width;

        velocity->x = 0.0;

        cur_collided_x = true;
    }

    if (CheckCollisionRecs(cur_crate_future_y, other_rect))
    {
        Rectangle col_rec = GetCollisionRec(cur_crate_future_y, other_rect);

        //temp_position.y += temp_velocity.y;

        if (velocity->y > 0.0)
            position->y -= col_rec.height;
        else
            position->y += col_rec.height;

        velocity->y = 0.0;

        cur_collided_y = true;
    }

    return cur_collided_x && cur_collided_y;
}
