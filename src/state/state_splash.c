#include "state_splash.h"

#include <raylib.h>
#include <math.h>

#include "state.h"
#include "../core/gameloop.h"

static Texture2D logo_bgdj;
static Texture2D logo_bd;
static Texture2D logo_rl;

static int current_logo = 0;
static Vector2 current_position;
static Color current_tint;

static Vector2 viewport_size;

static int frames_timer = 0;

static bool faster = false;

static void _set_logo();

void state_splash_enter()
{
    current_logo = 0;

    gameloop_pixel_mode = false;

    viewport_size = gameloop_get_viewport_size();

    logo_bgdj = LoadTexture("./resources/textures/logo_bgdj.png");
    logo_bd = LoadTexture("./resources/textures/logo_bd.png");
    logo_rl = LoadTexture("./resources/textures/logo_rl.png");

    _set_logo();
}

void state_splash_update()
{
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        faster = true;

    if (frames_timer > 120)
    {
        frames_timer = 0;

        current_logo++;
        if (current_logo > 2)
            state_switchto(STATE_MENU);
        else
            _set_logo();
    }

    current_tint = (Color) { 255, 255, 255, (fabs(sin((frames_timer / 60.0) * PI * 0.5)) * 255) };
    frames_timer += faster ? 2 : 1;
}

void state_splash_draw()
{
    ClearBackground(BLACK);
    switch (current_logo)
    {
        case 0:
            DrawTextureEx(logo_bgdj, current_position, 0.0, 1.0, current_tint);
            break;
        case 1:
            DrawTextureEx(logo_bd, current_position, 0.0, 1.0, current_tint);
            break;
        case 2:
            DrawTextureEx(logo_rl, current_position, 0.0, 1.0, current_tint);
            break;
    }
}

void state_splash_exit()
{
    UnloadTexture(logo_rl);
    UnloadTexture(logo_bd);
    UnloadTexture(logo_bgdj);

    gameloop_pixel_mode = true;
}

static void _set_logo()
{
    Vector2 current_size;
    switch (current_logo)
    {
        case 0:
            current_size = (Vector2) { logo_bgdj.width, logo_bgdj.height };
            break;
        case 1:
            current_size = (Vector2) { logo_bd.width, logo_bd.height };
            break;
        case 2:
            current_size = (Vector2) { logo_rl.width, logo_rl.height };
            break;
    }

    current_position = (Vector2) {
        (viewport_size.x - current_size.x) * 0.5,
        (viewport_size.y - current_size.y) * 0.5,
    };
}
