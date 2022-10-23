#include "state_main.h"

#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "../constants.h"
#include "../player.h"
#include "../crate.h"
#include "../chest.h"
#include "../hud.h"
#include "../util/list.h"
#include "../util/formatter.h"
#include "../global_resources.h"
#include "state.h"
#include "../core/sprite.h"
#include "../core/tween.h"

#include "cLDtk.h"

int chosen_character;

// Textures
static Texture2D sky_texture;
static Texture2D map_texture;

// Map
static struct levels *level;
static struct layerInstances *level_col;
static struct layerInstances *level_lantern_chains;
static struct layerInstances *level_lanterns;
static struct layerInstances *level_spikes;
static struct entityInstances *level_players;
static struct entityInstances *level_chests;
static struct entityInstances *level_crates_big;
static struct entityInstances *level_doors;
static int current_level = 0;
static bool level_loaded = false;

#define CHEST_INIT_LENGTH 64
static Chest* chest_list[CHEST_INIT_LENGTH];
static int chest_count = 0;

#define CRATE_INIT_LENGTH 64
static Crate* crate_list[CRATE_INIT_LENGTH];
static int crate_count = 0;

static Vector2 mouse_pos;
static Vector2 prev_mouse_pos;
static Vector2 mouse_vel;

static Player* player;
static HUD* hud;
static Camera2D camera = { 0 };

static Music bgm;

static bool debug_overlay = false;

static Tween* fade_tween = NULL;
static float tween_progress = 0;

static void _on_fade_tween_finished();
static void _load_level();
static void _draw_tiles(struct layerInstances *layer, Texture2D texture, Color tint);
static void _update_camera(bool lerp);

void state_main_enter()
{
    current_level = 0;

    sky_texture = LoadTexture("./resources/textures/sky.png");
    map_texture = LoadTexture("./resources/textures/tileset.png");

    loadJSONFile("{\"jsonVersion\":\"\"}", "./resources/maps/map.json");
    importMapData();

    _load_level();

    if (chosen_character == 0)
        player = player_new(PLAYERCHAR_0);
    else
        player = player_new(PLAYERCHAR_1);
    player->level_size = (Vector2) { level->pxWid, level->pxHei };
    player->door_rect = (Rectangle) { level_doors[0].x - 8, level_doors[0].y - 32, 16, 32 };
    player->position = (Vector2) { level_players[0].x, level_players[0].y };
    player->respawn_position = player->position;
    player->keys_target = chest_count;

    hud = hud_new(player);

    camera = (Camera2D) { 0 };
    camera.offset = (Vector2) { -20.0 + INIT_VIEWPORT_WIDTH * 0.5, INIT_VIEWPORT_HEIGHT * 0.5, };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    _update_camera(false);

    bgm = LoadMusicStream("./resources/bgm/where_visions_overlap.xm");
    PlayMusicStream(bgm);
}

void state_main_update()
{
    UpdateMusicStream(bgm);

    if (fade_tween != NULL)
    {
        tween_update(fade_tween);
        return;
    }

    mouse_pos = GetMousePosition();
    mouse_pos.x *= ((float)INIT_VIEWPORT_WIDTH / (float)INIT_WINDOW_WIDTH);
    mouse_pos.x += camera.target.x - camera.offset.x;
    mouse_pos.y *= ((float)INIT_VIEWPORT_HEIGHT / (float)INIT_WINDOW_HEIGHT);
    mouse_pos.y += camera.target.y - camera.offset.y;

    mouse_vel = (Vector2) { mouse_pos.x - prev_mouse_pos.x, mouse_pos.y - prev_mouse_pos.y };

    // if (IsKeyPressed(KEY_ESCAPE))
    //     state_main_game_over();

    if (IsKeyPressed(KEY_R))
        player->position = player->respawn_position;

    if (IsKeyPressed(KEY_F2))
        debug_overlay = !debug_overlay;

    player_update(player, level_col, level_spikes, crate_list, crate_count, chest_list, chest_count);
    for (int i = 0; i < crate_count; i++)
    {
        if (crate_list[i] == NULL)
            continue;
        crate_update(crate_list[i], level_col, crate_list, crate_count, IsMouseButtonDown(0) && player->character == PLAYERCHAR_0, mouse_pos, mouse_vel);
    }
    hud_update(hud);
    _update_camera(true);

    prev_mouse_pos = mouse_pos;

    switch (current_level)
    {
        case 0:
            if (player->position.x > 983.0)
                state_main_next_level();
            break;
        case 1:
            if (player->position.x > 980.0)
                state_main_next_level();
            break;
    }
}

void state_main_draw()
{
    ClearBackground(BLACK);

    if (current_level == 0)
        DrawTexturePro(sky_texture,
            (Rectangle) { 0.0, 0.0, sky_texture.width, sky_texture.height },
            (Rectangle) { 0.0, 0.0, INIT_VIEWPORT_WIDTH, INIT_VIEWPORT_HEIGHT },
            (Vector2) { 0.0, 0.0 }, 0.0, WHITE
        );
    else
        DrawRectangle(0, 0, INIT_VIEWPORT_WIDTH, INIT_VIEWPORT_HEIGHT, (Color) { 35, 30, 46, 255 });

    BeginMode2D(camera);
        _draw_tiles(level_col, map_texture, WHITE);
        _draw_tiles(level_lantern_chains, map_texture, WHITE);
        _draw_tiles(level_lanterns, map_texture, WHITE);
        _draw_tiles(level_spikes, map_texture, WHITE);
        if (player->keys_collected < player->keys_target)
            DrawRectangle(level_doors[0].x - 8, level_doors[0].y - 32, 16, 32, (Color) { 183, 168, 122, 255 } );
        for (int i = 0; i < chest_count; i++)
        {
            if (chest_list[i] == NULL)
                continue;
            chest_draw(chest_list[i]);
        }
        for (int i = 0; i < crate_count; i++)
        {
            if (crate_list[i] == NULL)
                continue;
            crate_draw(crate_list[i]);
        }
        player_draw(player);
    EndMode2D();

    hud_draw(hud);

    if (fade_tween != NULL)
        DrawRectangle(0, 0, INIT_VIEWPORT_WIDTH, INIT_VIEWPORT_HEIGHT, (Color) { 0, 0, 0, (tween_progress * 255.0) });

    if (debug_overlay)
    {
        Color color = LIME;
        int fps = GetFPS();

        if ((fps < 30) && (fps >= 15))
            color = ORANGE;
        else if (fps < 15)
            color = RED;

        DrawTextPro(gr_small_font,
            TextFormat("%2i FPS", fps), (Vector2) {0.0, 0.0},
            (Vector2) {0.0, 0.0}, 0, gr_small_font.baseSize, 0, color
        );

        DrawTextPro(gr_small_font,
            TextFormat("pos %s", formatter_vector2(player->position)), (Vector2) {0.0, 10.0},
            (Vector2) {0.0, 0.0}, 0, gr_small_font.baseSize, 0, WHITE
        );

        DrawTextPro(gr_small_font,
            TextFormat("vel %s", formatter_vector2(player->velocity)), (Vector2) {0.0, 20.0},
            (Vector2) {0.0, 0.0}, 0, gr_small_font.baseSize, 0, WHITE
        );

        DrawTextPro(gr_small_font,
            TextFormat("world mouse %s", formatter_vector2(mouse_pos)), (Vector2) {0.0, 30.0},
            (Vector2) {0.0, 0.0}, 0, gr_small_font.baseSize, 0, WHITE
        );
    }
}

void state_main_exit()
{
    for (int i = 0; i < chest_count; i++)
    {
        if (chest_list[i] != NULL)
        {
            chest_destroy(chest_list[i]);
            chest_list[i] = NULL;
        }
    }

    for (int i = 0; i < crate_count; i++)
    {
        if (crate_list[i] != NULL)
        {
            crate_destroy(crate_list[i]);
            crate_list[i] = NULL;
        }
    }

    tween_destroy(fade_tween);
    fade_tween = NULL;
    tween_progress = 0.0;
    UnloadMusicStream(bgm);
    player_destroy(player);
    hud_destroy(hud);
    freeMapData();
    UnloadTexture(map_texture);
    json_value_free(schema);
    json_value_free(user_data);
}

void state_main_next_level()
{
    current_level++;
    if (current_level > 1)
        state_main_game_over();
    else
    {
        _load_level();
        player->level_size = (Vector2) { level->pxWid, level->pxHei };
        player->door_rect = (Rectangle) { level_doors[0].x - 8, level_doors[0].y - 32, 16, 32 };
        player->position = (Vector2) { level_players[0].x, level_players[0].y };
        player->respawn_position = player->position;
        player->keys_target = chest_count;
        player->keys_collected = 0;

        _update_camera(false);
    }
}

void state_main_game_over()
{
    fade_tween = tween_new(&tween_progress, 0.0f, 1.0f, 1.0f, EASE_LINEAR);
    fade_tween->on_tween_finished_ptr = _on_fade_tween_finished;
    tween_start(fade_tween);
}

static void _on_fade_tween_finished()
{
    state_switchto(STATE_SPLASH);
}

static void _load_level()
{
    level = getLevel(TextFormat("level%i", current_level));
    level_col = getLayer("col", level->uid);
    level_lantern_chains = getLayer("lantern_data", level->uid);
    level_lanterns = getLayer("lantern", level->uid);
    level_spikes = getLayer("spike", level->uid);
    level_players = getEntity("player", level->uid);
    level_chests = getEntity("chest", level->uid);
    level_crates_big = getEntity("crate_big", level->uid);
    level_doors = getEntity("door", level->uid);

    chest_count = 0;
    for (int i = 0; i < CHEST_INIT_LENGTH; i++)
    {
        if (level_loaded && chest_list[i] != NULL)
            chest_destroy(chest_list[i]);

        if (i < level_chests->size)
        {
            Chest* chest = chest_new();
            chest->texture = map_texture;
            chest->position = (Vector2) { level_chests[i].x, level_chests[i].y };
            chest_list[i] = chest;

            chest_count++;
        }
        else
            chest_list[i] = NULL;
    }

    crate_count = 0;
    for (int i = 0; i < CRATE_INIT_LENGTH; i++)
    {
        if (level_loaded && crate_list[i] != NULL)
            crate_destroy(crate_list[i]);

        if (i < level_crates_big->size)
        {
            Crate* crate = crate_new(CRATE_BIG);
            crate->texture = map_texture;
            crate->position = (Vector2) { level_crates_big[i].x, level_crates_big[i].y };
            crate_list[i] = crate;

            crate_count++;
        }
        else
            crate_list[i] = NULL;
    }

    level_loaded = true;
}

static void _draw_tiles(struct layerInstances *layer, Texture2D texture, Color tint)
{
    for (int y = layer->autoTiles_data_ptr->count; y-- > 0;)
    {
        Rectangle src_rect = { (float)layer->autoTiles_data_ptr[y].SRCx, (float)layer->autoTiles_data_ptr[y].SRCy, TILE_OFFSET, TILE_OFFSET };
        Rectangle dst_rect = {
            (float)layer->autoTiles_data_ptr[y].x,
            (float)layer->autoTiles_data_ptr[y].y,
            fabsf(src_rect.width),
            fabsf(src_rect.height)
        };
        Vector2 origin = { 0.0f, 0.0f };

        int flip = layer->autoTiles_data_ptr[y].f;

        switch (flip)
        {
            case 0:
                break;
            case 1:
                src_rect.width *= -1;
                break;
            case 2:
                src_rect.height *= -1;
                break;
            case 3:
                src_rect.width *= -1;
                src_rect.height *= -1;
                break;
        }

        DrawTexturePro(texture, src_rect, dst_rect, origin, 0.0f, tint);
    }
}

static void _update_camera(bool lerp)
{
    Vector2 target = {
        player->position.x - player->sprite->origin.x,
        player->position.y - player->sprite->origin.y
    };

    if (target.x - camera.offset.x < 0.0)
        target.x = camera.offset.x;
    if (target.x + camera.offset.x >= level->pxWid - (TILE_OFFSET * 2))
        target.x = level->pxWid - (TILE_OFFSET * 2) - camera.offset.x;

    if (target.y - camera.offset.y < 0.0)
        target.y = camera.offset.y;
    if (target.y + camera.offset.y >= level->pxHei - (TILE_OFFSET * 2))
        target.y = level->pxHei - (TILE_OFFSET * 2) - camera.offset.y;

    if (lerp)
    {
        camera.target = (Vector2) {
            floorf(Lerp(camera.target.x, target.x, 0.1)),
            floorf(Lerp(camera.target.y, target.y, 0.1))
        };
    }
    else
    {
        camera.target = (Vector2) {
            floorf(target.x),
            floorf(target.y)
        };
    }
}
