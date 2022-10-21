#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <raylib.h>

#include "core/sprite.h"
#include "cLDtk.h"

#define PLAYER_ACCEL_SPEED 0.08
#define PLAYER_DEACCEL_SPEED 0.1
#define PLAYER_MAX_SPEED 1.35
#define PLAYER_TERMINAL_VELOCITY 5

typedef enum
{
    PLAYERCHAR_0,
    PLAYERCHAR_1
} PlayerCharacter;

typedef struct Player
{
    Vector2 position;
    Vector2 velocity;

    Sprite* sprite;

    enum { PLAYER_IDLE, PLAYER_MOVING } state;

    int current_frame;
    int frames_counter;
    int frame_speed;
} Player;

Player* player_new(PlayerCharacter player_character);
void player_update(Player* player, struct layerInstances* map_col_layer);
void player_draw(Player* player);
void player_destroy(Player* player);

#endif
