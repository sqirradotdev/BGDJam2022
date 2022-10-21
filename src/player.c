#include "player.h"

#include <stdlib.h>
#include <math.h>

#include "constants.h"

static void _change_player_state(Player* player, int state);

Player* player_new(PlayerCharacter player_character)
{
    Player* player = (Player*)malloc(sizeof(Player));

    player->position = (Vector2) { 0.0, 0.0 };
    player->velocity = (Vector2) { 0.0, 0.0 };

    Texture2D player_texture;
    switch (player_character)
    {
        default:
            player_texture = LoadTexture("./resources/textures/char0.png");
            break;
    }

    player->sprite = sprite_new(player_texture);
    player->sprite->origin.x = 9;
    player->sprite->origin.y = 18;
    player->sprite->src_rect.height = player_texture.height / 22;

    player->state = PLAYER_IDLE;

    player->current_frame = 0;
    player->frames_counter = 0;
    player->frame_speed = 8;

    return player;
}

void player_update(Player* player, struct layerInstances* map_col_layer)
{
    float player_input = (IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT)) * PLAYER_ACCEL_SPEED;
    if (player_input != 0)
    {
        player->velocity.x += player_input;
        if (fabs(player->velocity.x) > PLAYER_MAX_SPEED)
            player->velocity.x = PLAYER_MAX_SPEED * copysignf(1.0, player->velocity.x);

        player->sprite->flip_x = (player_input < 0);

        _change_player_state(player, PLAYER_MOVING);
    }
    else
    {
        if (fabs(player->velocity.x) > 0.5)
            player->velocity.x += PLAYER_DEACCEL_SPEED * -copysignf(1.0, player->velocity.x);
        else
            player->velocity.x = 0.0;

        _change_player_state(player, PLAYER_IDLE);
    }

    bool on_floor = false;
    float y_snap_to;
    for (int y = map_col_layer->autoTiles_data_ptr->count; y-- > 0;)
    {
        Rectangle tile_rect = {
            (float)map_col_layer->autoTiles_data_ptr[y].x,
            (float)map_col_layer->autoTiles_data_ptr[y].y,
            TILE_OFFSET,
            TILE_OFFSET
        };

        Rectangle player_rect = (Rectangle) {
            player->position.x - player->sprite->origin.x,
            player->position.y - player->sprite->origin.y,
            18,
            18
        };

        if (CheckCollisionRecs(player_rect, tile_rect))
        {
            on_floor = true;
            y_snap_to = tile_rect.y;
        }
    }

    if (on_floor)
    {
        player->position.y = y_snap_to;
        player->velocity.y = 0;
    }
    else
    {
        player->velocity.y += GRAVITY;
        if (fabs(player->velocity.y) > PLAYER_TERMINAL_VELOCITY)
            player->velocity.y = PLAYER_TERMINAL_VELOCITY * copysignf(1.0, player->velocity.y);
    }

    player->position.x += player->velocity.x;
    player->position.y += player->velocity.y;

    player->sprite->position = player->position;
}

void player_draw(Player* player)
{
    player->frames_counter++;

    if (player->frames_counter >= (60 / player->frame_speed))
    {
        player->frames_counter = 0;
        player->current_frame++;

        switch (player->state)
        {
            case PLAYER_IDLE:
                if (player->current_frame > 3)
                    player->current_frame = 0;
                break;
            case PLAYER_MOVING:
                if (player->current_frame > 7)
                    player->current_frame = 4;
                break;
            default:
                break;
        }

        player->sprite->src_rect.y = (float)player->current_frame * (float)player->sprite->texture.height / 22;
    }

    sprite_draw(player->sprite);
}

void player_destroy(Player* player)
{
    sprite_destroy(player->sprite);
    free(player);
}

static void _change_player_state(Player* player, int state)
{
    if (player->state == state)
        return;
    
    player->state = state;

    switch (player->state)
    {
        case PLAYER_IDLE:
            player->current_frame = 0;
            player->frame_speed = 7;
            break;
        case PLAYER_MOVING:
            player->current_frame = 4;
            player->frame_speed = 10;
            break;
        default:
            break;
    }
}
