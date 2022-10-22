#include "state_menu.h"

#include <raylib.h>
#include <math.h>

#include "../constants.h"
#include "../global_resources.h"
#include "state.h"
#include "../core/sprite.h"
#include "../core/tween.h"

static const char* message = "Press SPACE to switch to state_main";
static Vector2 textPos;

static Tween* fade_tween;
static float fade_alpha_progress = 1;

static Tween* bruhtween;

static Music bgm;

static bool selected = false;

static void _on_fade_tween_finished(void);

void state_menu_enter()
{
    Vector2 measure = MeasureTextEx(gr_small_font, message, gr_medium_font.baseSize, 0);
    textPos = (Vector2) { floorf((INIT_VIEWPORT_WIDTH - measure.x) * 0.5), floorf((INIT_VIEWPORT_HEIGHT - measure.y) * 0.5) };

    fade_tween = tween_new(&fade_alpha_progress, 1, 0, 2, EASE_LINEAR);
    fade_tween->on_tween_finished_ptr = &_on_fade_tween_finished;
    tween_start(fade_tween);

    bruhtween = tween_new(&textPos.y, 0, textPos.y, 2, EASE_OUT_BOUNCE);
    tween_start(bruhtween);

    bgm = LoadMusicStream("./resources/bgm/i_have_a_vision.xm");
    PlayMusicStream(bgm);
    SetMusicVolume(bgm, 0.0);
}

void state_menu_update()
{
    UpdateMusicStream(bgm);
    SetMusicVolume(bgm, 1.0 - fade_alpha_progress);

    if (!selected)
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            //state_switchto(STATE_MAIN);
            selected = true;

            fade_tween->from = 0;
            fade_tween->to = 1;
            tween_start(fade_tween);
        }
    }

    tween_update(fade_tween);
    tween_update(bruhtween);
}

void state_menu_draw()
{
    ClearBackground(BLACK);
    DrawTextEx(gr_small_font, message, textPos, gr_medium_font.baseSize, 0, WHITE);
    if (fade_alpha_progress != 0)
        DrawRectangle(0, 0, INIT_VIEWPORT_WIDTH, INIT_VIEWPORT_HEIGHT, (Color) { 0, 0, 0, (unsigned char)(fade_alpha_progress * 255) });
}

void state_menu_exit()
{
    UnloadMusicStream(bgm);
    tween_destroy(fade_tween);
}

static void _on_fade_tween_finished(void)
{
    if (selected)
        state_switchto(STATE_MAIN);
}
