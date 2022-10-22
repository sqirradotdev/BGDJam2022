#include "state_main.h"

#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "../constants.h"
#include "../player.h"
#include "../hud.h"
#include "../util/formatter.h"
#include "../global_resources.h"
#include "state.h"
#include "../core/sprite.h"

#include "cLDtk.h"
#include "physac.h"

// Textures
static Texture2D map_texture;

// Map
static struct levels *level;
static struct layerInstances *level_bg;
static struct layerInstances *level_col;
static struct entityInstances *level_players;
static struct entityInstances *level_chests;
static int current_level = 1;

Player* player;

HUD* hud;

Camera2D camera = { 0 };

static Music bgm;

static void _draw_tiles(struct layerInstances *layer, Texture2D texture, Color tint);
static void _update_camera(bool lerp);

void state_main_enter()
{
    map_texture = LoadTexture("./resources/textures/tileset.png");
    
    player = player_new(PLAYERCHAR_0);

    hud = hud_new(player);

    camera = (Camera2D) { 0 };

    loadJSONFile("{\"jsonVersion\":\"\"}", "./resources/maps/map.json");
    importMapData();

    level = getLevel(TextFormat("level%i", current_level));
    level_bg = getLayer("bg", level->uid);
    level_col = getLayer("col", level->uid);
    level_players = getEntity("player", level->uid);
    level_chests = getEntity("chest", level->uid);

    player->position = (Vector2) { level_players[0].x, level_players[0].y };

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

    player_update(player, level_col);
    hud_update(hud);

    _update_camera(true);
}

void state_main_draw()
{
    ClearBackground(BLACK);
    BeginMode2D(camera);
        if (current_level == 0)
        {

        }
        else
            _draw_tiles(level_bg, map_texture, WHITE);

        _draw_tiles(level_col, map_texture, WHITE);
        player_draw(player);
    EndMode2D();
    hud_draw(hud);
    DrawTextPro(gr_small_font, formatter_vector2(player->velocity), (Vector2) {0.0, 0.0}, (Vector2) {0.0, 0.0}, 0, gr_small_font.baseSize, 0, WHITE);
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
