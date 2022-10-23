#include "player.h"

#include <stdlib.h>
#include <math.h>
#include <raymath.h>

#include "constants.h"
#include "global_resources.h"
#include "state/state_main.h"

static Rectangle cur_player_rect;
static Rectangle cur_player_future_x;
static Rectangle cur_player_future_y;
static bool cur_collided_x = false;
static bool cur_collided_y = false;

static void _begin_update_physics(Vector2* position, Vector2* velocity);
static bool _update_physics(Player* player, Rectangle other_rect, Vector2* position, Vector2* velocity, bool* touching_ground);
static void _change_player_state(Player* player, int state);

Player* player_new(PlayerCharacter player_character)
{
    Player* player = (Player*)malloc(sizeof(Player));

    player->position = (Vector2) { 0.0, 0.0 };
    player->velocity = (Vector2) { 0.0, 0.0 };

    Texture2D player_texture;
    switch (player_character)
    {
        case PLAYERCHAR_0:
            player_texture = LoadTexture("./resources/textures/char0.png");
            player->max_hp = 3;
            break;
        case PLAYERCHAR_1:
            player_texture = LoadTexture("./resources/textures/char1.png");
            player->max_hp = 4;
            break;
    }
    player->character = player_character;

    player->hp = player->max_hp;

    player->sprite = sprite_new(player_texture);
    player->sprite->origin.x = 9;
    player->sprite->origin.y = 18;
    player->sprite->src_rect.height = player_texture.height / 22;

    player->state = PLAYER_IDLE;

    player->level_size = (Vector2) { 100.0, 100.0 };
    player->door_rect = (Rectangle) { 0.0, 0.0, 0.0, 0.0 };

    player->current_frame = 0;
    player->frames_counter = 0;
    player->frame_speed = 8;

    player->hurt_anim = false;
    player->hurt_timer = 0;

    player->dashing = false;
    player->dash_direction = (Vector2) { 0.0, 0.0 };
    player->dash_frames = 0;

    player->touching_ground = false;
    player->jump_used = false;
    player->dash_used = false;

    player->keys_collected = 0;

    return player;
}

void player_update(Player* player,
    struct layerInstances* map_col_layer, struct layerInstances* map_spikes,
    Crate** crates_ptr, int crate_size, Chest** chest_ptr, int chest_size)
{
    if (player->hurt_anim)
    {
        player->hurt_timer++;
        if (player->hurt_timer > 60)
        {
            if (player->hp == 0)
                state_main_game_over();
            else
            {
                player->hurt_timer = 0;
                player->hurt_anim = false;
                player->position = player->respawn_position;
            }
        }

        return;
    }
    
    Vector2 temp_velocity = player->velocity;
    Vector2 temp_position = player->position;

    Vector2 player_input = {
        (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) - (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)),
        (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) - (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    };

    if (player->dash_frames > 6)
    {
        player->dash_frames = 0;
        player->dashing = false;
    }

    if (player->character == PLAYERCHAR_1 && !player->dash_used && IsKeyPressed(KEY_X) && Vector2Length(player_input) > 0.0)
    {
        player->dashing = true;
        player->dash_used = true;

        PlaySound(gr_dash_sfx);
    }

    if (!player->dashing)
    {
        if (player_input.x != 0)
        {
            // accel
            temp_velocity.x += player_input.x * PLAYER_ACCEL_SPEED;
            if (fabs(temp_velocity.x) > PLAYER_MAX_SPEED)
                temp_velocity.x = PLAYER_MAX_SPEED * copysignf(1.0, player->velocity.x);

            player->sprite->flip_x = (player_input.x < 0);
            _change_player_state(player, PLAYER_MOVING);
        }
        else
        {
            // deaccel
            if (fabs(temp_velocity.x) > 0.5f)
                temp_velocity.x += PLAYER_DEACCEL_SPEED * -copysignf(1.0, temp_velocity.x);
            else
                temp_velocity.x = 0.0f;

            _change_player_state(player, PLAYER_IDLE);
        }

        temp_velocity.y += GRAVITY;
        if (fabs(temp_velocity.y) > PLAYER_TERMINAL_VELOCITY)
            temp_velocity.y = PLAYER_TERMINAL_VELOCITY * copysignf(1.0f, temp_velocity.y);
    }
    else
    {
        temp_velocity.x = player_input.x * 3;
        temp_velocity.y = player_input.y * 3;
        player->dash_frames++;
    }

    // temp_position.x += temp_velocity.x;
    // temp_position.y += temp_velocity.y;

    player->touching_ground = false;
    _begin_update_physics(&temp_position, &temp_velocity);
    for (int y = map_col_layer->autoTiles_data_ptr->count; y-- > 0;)
    {
        Rectangle tile_rect = {
            (float)map_col_layer->autoTiles_data_ptr[y].x,
            (float)map_col_layer->autoTiles_data_ptr[y].y,
            TILE_OFFSET,
            TILE_OFFSET
        };

        if (_update_physics(player, tile_rect, &temp_position, &temp_velocity, &player->touching_ground))
        {
            player->dash_frames = 0;
            player->dashing = false;
            break;
        }
    }
    for (int i = 0; i < crate_size; i++)
    {
        if (crates_ptr[i] == NULL)
            continue;

        if (_update_physics(player, crates_ptr[i]->current_rect, &temp_position, &temp_velocity, &player->touching_ground))
        {
            player->dash_frames = 0;
            player->dashing = false;
            break;
        }
    }

    if (player->keys_collected < player->keys_target)
        _update_physics(player, player->door_rect, &temp_position, &temp_velocity, &player->touching_ground);

    if (temp_position.x < 0.0)
    {
        temp_position.x = 0.0;
        temp_velocity.x = 0.0;
    }
    if (temp_position.x > player->level_size.x)
    {
        temp_position.x = player->level_size.x;
        temp_velocity.x = 0.0;
    }
    if (temp_position.y < 0.0)
    {
        temp_position.y = 0.0;
        temp_velocity.y = 0.0;
    }
    if (temp_position.y > player->level_size.y)
    {
        temp_position.y = player->level_size.y;
        temp_velocity.y = 0.0;
    }

    temp_position.x += temp_velocity.x;
    temp_position.y += temp_velocity.y;

    bool jump_pressed = IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_Z);
    bool jump_released = IsKeyReleased(KEY_SPACE) || IsKeyReleased(KEY_Z);
    if (player->jump_used && jump_released)
        player->jump_used = false;

    if (player->touching_ground)
    {
        if (player->dash_used)
            player->dash_used = false;

        if (jump_pressed && !player->jump_used)
        {
            temp_velocity.y = -4.25f;
            player->jump_used = true;

            PlaySound(gr_jump_sfx);
        }
    }

    player->position = temp_position;
    player->velocity = temp_velocity;

    player->sprite->position = player->position;

    Rectangle player_rect = { player->position.x - 6, player->position.y - 14, 12, 14 };

    for (int y = map_spikes->autoTiles_data_ptr->count; y-- > 0;)
    {
        Rectangle tile_rect = {
            (float)map_spikes->autoTiles_data_ptr[y].x,
            (float)map_spikes->autoTiles_data_ptr[y].y,
            TILE_OFFSET,
            TILE_OFFSET
        };

        if (CheckCollisionRecs(player_rect, tile_rect))
        {
            player->hp--;
            player->hurt_anim = true;
            PlaySound(gr_hurt_sfx);

            break;
        }
    }

    for (int i = 0; i < chest_size; i++)
    {
        if (chest_ptr[i] == NULL)
            continue;

        Rectangle chest_rect = { chest_ptr[i]->position.x, chest_ptr[i]->position.y, 16.0, 16.0};
        if (!chest_ptr[i]->opened && CheckCollisionRecs(player_rect, chest_rect))
        {
            chest_ptr[i]->opened = true;
            player->keys_collected++;

            PlaySound(gr_key_sfx);
        }
    }
}

void player_draw(Player* player)
{
    if (player->hurt_anim)
        return;

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

    player->sprite->color = player->dashing ? GREEN : WHITE;

    sprite_draw(player->sprite);
}

void player_destroy(Player* player)
{
    sprite_destroy(player->sprite);
    free(player);
}

static void _begin_update_physics(Vector2* position, Vector2* velocity)
{
    cur_collided_x = false;
    cur_collided_y = false;

    cur_player_rect = (Rectangle) { position->x - 6, position->y - 14, 12, 14 };

    cur_player_future_x = (Rectangle) { cur_player_rect.x + velocity->x, cur_player_rect.y, cur_player_rect.width, cur_player_rect.height };
    cur_player_future_y = (Rectangle) { cur_player_rect.x, cur_player_rect.y + velocity->y, cur_player_rect.width, cur_player_rect.height };
}

static bool _update_physics(Player* player, Rectangle other_rect, Vector2* position, Vector2* velocity, bool* touching_ground)
{
    if (CheckCollisionRecs(cur_player_future_x, other_rect))
    {
        Rectangle col_rec = GetCollisionRec(cur_player_future_x, other_rect);

        //temp_position.x += temp_velocity.x;

        if (velocity->x > 0.0f)
            position->x -= col_rec.width;
        else
            position->x += col_rec.width;

        velocity->x = 0.0f;

        cur_collided_x = true;
    }

    if (CheckCollisionRecs(cur_player_future_y, other_rect))
    {
        Rectangle col_rec = GetCollisionRec(cur_player_future_y, other_rect);

        //temp_position.y += temp_velocity.y;

        if (velocity->y > 0.0f)
        {
            position->y -= col_rec.height;
            *touching_ground = true;
        }
        else
            position->y += col_rec.height;

        velocity->y = 0.0f;

        cur_collided_y = true;
    }

    return cur_collided_x && cur_collided_y;
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
