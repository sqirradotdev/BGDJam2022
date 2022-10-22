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
            player->max_hp = 3;
            break;
    }

    player->hp = player->max_hp - 1;

    player->sprite = sprite_new(player_texture);
    player->sprite->origin.x = 9;
    player->sprite->origin.y = 18;
    player->sprite->src_rect.height = player_texture.height / 22;

    player->state = PLAYER_IDLE;

    player->current_frame = 0;
    player->frames_counter = 0;
    player->frame_speed = 8;

    player->dash_used = false;

    return player;
}

void player_update(Player* player, struct layerInstances* map_col_layer)
{
    Vector2 temp_velocity = player->velocity;
    Vector2 temp_position = player->position;

    float player_input = ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) - (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))) * PLAYER_ACCEL_SPEED;
    if (player_input != 0)
    {
        // accel
        temp_velocity.x += player_input;
        if (fabs(temp_velocity.x) > PLAYER_MAX_SPEED)
            temp_velocity.x = PLAYER_MAX_SPEED * copysignf(1.0, player->velocity.x);

        player->sprite->flip_x = (player_input < 0);
        _change_player_state(player, PLAYER_MOVING);
    }
    else
    {
        // deaccel
        if (fabs(temp_velocity.x) > 0.5)
            temp_velocity.x += PLAYER_DEACCEL_SPEED * -copysignf(1.0, temp_velocity.x);
        else
            temp_velocity.x = 0.0;

        _change_player_state(player, PLAYER_IDLE);
    }

    bool trying_to_jump = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z);

    temp_velocity.y += GRAVITY;
    if (fabs(temp_velocity.y) > PLAYER_TERMINAL_VELOCITY)
        temp_velocity.y = PLAYER_TERMINAL_VELOCITY * copysignf(1.0, temp_velocity.y);

    temp_position.x += temp_velocity.x;
    temp_position.y += temp_velocity.y;

    bool touching_ground = false;
    for (int y = map_col_layer->autoTiles_data_ptr->count; y-- > 0;)
    {
        Rectangle tile_rect = {
            (float)map_col_layer->autoTiles_data_ptr[y].x,
            (float)map_col_layer->autoTiles_data_ptr[y].y,
            TILE_OFFSET,
            TILE_OFFSET
        };

        for (int i = 0; i < 2; i++)
        {
            Rectangle player_rect = (Rectangle) {
                temp_position.x - 6,
                temp_position.y - 14,
                12,
                14
            };

            if (CheckCollisionRecs(player_rect, tile_rect))
            {
                Rectangle col_rec = GetCollisionRec(player_rect, tile_rect);
                //TraceLog(LOG_INFO, "py: %f, x: %f, y: %f, w: %f, h: %f", player_rect.y, col_rec.x, col_rec.y, col_rec.width, col_rec.height);

                // cant be bothered optimizing tbh

                if (i == 0)
                {
                    float p = player_rect.y + player_rect.height;
                    if ((p >= col_rec.y && p <= col_rec.y + col_rec.height) && temp_velocity.y > 0.0)
                    {
                        temp_position.y -= col_rec.height;
                        temp_velocity.y = 0.0;

                        touching_ground = true;
                    }

                    p = player_rect.y;
                    if ((p >= col_rec.y && p <= col_rec.y + col_rec.height) && temp_velocity.y < 0.0)
                    {
                        temp_position.y += col_rec.height;
                        temp_velocity.y = 0.0;
                    }
                }
                else                
                {
                    float p = player_rect.x + player_rect.width;
                    if ((p >= col_rec.x && p <= col_rec.x + col_rec.width) && temp_velocity.x > 0.0)
                    {
                        temp_position.x -= col_rec.width;
                        temp_velocity.x = 0.0;
                    }

                    p = player_rect.x;
                    if ((p >= col_rec.x && p <= col_rec.x + col_rec.width) && temp_velocity.x < 0.0)
                    {
                        temp_position.x += col_rec.width;
                        temp_velocity.x = 0.0;
                    }
                }
            }
        }
    }

    if (touching_ground)
    {
        if (trying_to_jump)
            temp_velocity.y = -3.5;
    }

    player->position = temp_position;
    player->velocity = temp_velocity;

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
