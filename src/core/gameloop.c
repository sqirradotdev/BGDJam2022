#include "gameloop.h"

#include "../global_resources.h"
#include "../constants.h"
#include "../state/state.h"

extern bool gameloop_pixel_mode = true;

static Rectangle viewport_src_rect;
static Rectangle viewport_dst_rect;
static RenderTexture2D viewport;

bool gameloop_init()
{
    InitWindow(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, INIT_WINDOW_TITLE);
    InitAudioDevice();
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    gr_load_global_resources();

    viewport_src_rect = (Rectangle) { 0.0, 0.0, INIT_VIEWPORT_WIDTH, -INIT_VIEWPORT_HEIGHT };
    viewport_dst_rect = (Rectangle) { 0.0, 0.0, INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT };
    viewport = LoadRenderTexture(INIT_VIEWPORT_WIDTH, INIT_VIEWPORT_HEIGHT);

    TraceLog(LOG_INFO, "Game initialized. Hello BGDJam2022!");

    state_init();

    return true;
}

bool gameloop_loop()
{
    while (!WindowShouldClose())
    {
        // Update
        state_update();

        if (gameloop_pixel_mode)
        {
            // Render
            BeginTextureMode(viewport);
                state_draw();
            EndTextureMode();

            BeginDrawing();
                DrawTexturePro(viewport.texture, viewport_src_rect, viewport_dst_rect, (Vector2) { 0.0, 0.0 }, 0.0, WHITE);
            EndDrawing();
        }
        else
        {
            BeginDrawing();
                state_draw();
            EndDrawing();
        }
    }

    return true;
}

void gameloop_clean()
{
    state_clean();
    CloseWindow();

    TraceLog(LOG_INFO, "Goodbye!");
}

Vector2 gameloop_get_viewport_size()
{
    if (gameloop_pixel_mode)
        return (Vector2) { viewport.texture.width, viewport.texture.height };

    return (Vector2) { INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT };
}
