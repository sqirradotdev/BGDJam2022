#include "state_menu.h"

#include <raylib.h>
#include <math.h>

#include "../constants.h"
#include "../global_resources.h"
#include "state.h"
#include "../core/sprite.h"

static const char* message = "Press SPACE to switch to state_main";
static Vector2 textPos;

static Music bgm;

void state_menu_enter()
{
    Vector2 measure = MeasureTextEx(gr_small_font, message, gr_medium_font.baseSize, 0);
    textPos = (Vector2) { floorf((INIT_VIEWPORT_WIDTH - measure.x) * 0.5), floorf((INIT_VIEWPORT_HEIGHT - measure.y) * 0.5) };

    bgm = LoadMusicStream("./resources/bgm/i_have_a_vision.xm");
    PlayMusicStream(bgm);
}

void state_menu_update()
{
    UpdateMusicStream(bgm);

    if (IsKeyPressed(KEY_SPACE))
        state_switchto(STATE_MAIN);
}

void state_menu_draw()
{
    ClearBackground(DARKGRAY);
    DrawTextEx(gr_small_font, message, textPos, gr_medium_font.baseSize, 0, WHITE);
}

void state_menu_exit()
{
    UnloadMusicStream(bgm);
}
