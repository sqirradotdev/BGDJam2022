#include "state_main.h"

#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "../constants.h"
#include "../player.h"
#include "../crate.h"
#include "../hud.h"
#include "../util/list.h"
#include "../util/formatter.h"
#include "../global_resources.h"
#include "state.h"
#include "../core/sprite.h"

#include "cLDtk.h"
#include "physac.h"

// Textures
static Texture2D sky_texture;
static Texture2D map_texture;

// Map
static struct levels *level;
static struct layerInstances *level_bg;
static struct layerInstances *level_col;
static struct layerInstances *level_lantern_chains;
static struct layerInstances *level_lanterns;
static struct layerInstances *level_spikes;
static struct entityInstances *level_players;
static struct entityInstances *level_chests;
static struct entityInstances *level_crates_big;
static int current_level = 1;

static List* crate_list;

Player* player;
HUD* hud;
Camera2D camera = { 0 };

static Music bgm;

static bool debug_overlay = true;

static void _draw_tiles(struct layerInstances *layer, Texture2D texture, Color tint);
static void _update_camera(bool lerp);

void state_main_enter()
{
    sky_texture = LoadTexture("./resources/textures/sky.png");
    map_texture = LoadTexture("./resources/textures/tileset.png");

    loadJSONFile("{\"jsonVersion\":\"\"}", "./resources/maps/map.json");
    importMapData();

    level = getLevel(TextFormat("level%i", current_level));
    level_bg = getLayer("bg", level->uid);
    level_col = getLayer("col", level->uid);
    level_lantern_chains = getLayer("lantern_data", level->uid);
    level_lanterns = getLayer("lantern", level->uid);
    level_spikes = getLayer("spike", level->uid);
    level_players = getEntity("player", level->uid);
    level_chests = getEntity("chest", level->uid);
    level_crates_big = getEntity("crate_big", level->uid);

    crate_list = list_new();
    for (int i = 0; i < level_crates_big->size; i++)
    {
        Crate* crate = crate_new(CRATE_BIG);
        crate->texture = map_texture;
        crate->position = (Vector2) { level_crates_big[i].x, level_crates_big[i].y };
        list_push(crate_list, crate);
    }

    player = player_new(PLAYERCHAR_0);
    player->level_size = (Vector2) { level->pxWid, level->pxHei };
    player->position = (Vector2) { level_players[0].x, level_players[0].y };

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

    if (IsKeyPressed(KEY_R))
        state_restart();

    if (IsKeyPressed(KEY_F2))
        debug_overlay = !debug_overlay;

    player_update(player, level_col);
    for (int i = 0; i < crate_list->length; i++)
    {
        crate_update((Crate*)crate_list->elements[i], level_col);
    }
    hud_update(hud);

    _update_camera(true);
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
        if (current_level != 0)
            _draw_tiles(level_bg, map_texture, WHITE);
        _draw_tiles(level_col, map_texture, WHITE);
        _draw_tiles(level_lantern_chains, map_texture, WHITE);
        _draw_tiles(level_lanterns, map_texture, WHITE);
        _draw_tiles(level_spikes, map_texture, WHITE);
        for (int i = 0; i < crate_list->length; i++)
        {
            crate_draw((Crate*)crate_list->elements[i]);
        }
        player_draw(player);
    EndMode2D();
    // if (current_level != 0)
    // {
    //     DrawRectangle(0, 0, INIT_VIEWPORT_WIDTH, INIT_VIEWPORT_HEIGHT, (Color) { 0, 0, 0, 180 });
    // }

    hud_draw(hud);

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
    }
}

void state_main_exit()
{
    UnloadMusicStream(bgm);
    player_destroy(player);
    hud_destroy(hud);
    freeMapData();
    UnloadTexture(map_texture);
    json_value_free(schema);
    json_value_free(user_data);
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
