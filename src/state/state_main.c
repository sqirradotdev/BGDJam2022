#include "state_main.h"

#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "../constants.h"
#include "../player.h"
#include "state.h"
#include "../core/sprite.h"

#include "cLDtk.h"
#include "physac.h"

// Textures
static Texture2D map_texture;

// Map
static struct levels *lvl_one;
static struct layerInstances *lvl_one_background;
static struct layerInstances *lvl_one_collisions;

Player* player;

Camera2D camera = { 0 };

static Music bgm;

static void _draw_tiles(struct layerInstances *layer, Texture2D texture, Color tint);

void state_main_enter()
{
    player = player_new(PLAYERCHAR_0);

    camera = (Camera2D) { 0 };

    map_texture = LoadTexture("./resources/textures/tileset.png");

    loadJSONFile("{\"jsonVersion\":\"\"}", "./resources/maps/map.json");
    importMapData();

    lvl_one = getLevel("level0");
    lvl_one_background = getLayer("bg", lvl_one->uid);
    lvl_one_collisions = getLayer("col", lvl_one->uid);

    camera.offset = (Vector2) { -20.0 + INIT_VIEWPORT_WIDTH * 0.5, INIT_VIEWPORT_HEIGHT * 0.5, };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    bgm = LoadMusicStream("./resources/bgm/where_visions_overlap.xm");
    PlayMusicStream(bgm);
}

void state_main_update()
{
    UpdateMusicStream(bgm);

    if (IsKeyPressed(KEY_R))
        state_restart();

    player_update(player, lvl_one_collisions);

    Vector2 target = {
        player->position.x - player->sprite->origin.x,
        player->position.y - player->sprite->origin.y
    };

    camera.target = (Vector2) {
        floorf(Lerp(camera.target.x, target.x, 0.1)),
        floorf(Lerp(camera.target.y, target.y, 0.1))
    };
}

void state_main_draw()
{
    ClearBackground(BLACK);
    BeginMode2D(camera);
        _draw_tiles(lvl_one_background, map_texture, WHITE);
        _draw_tiles(lvl_one_collisions, map_texture, WHITE);

        player_draw(player);
    EndMode2D();
}

void state_main_exit()
{
    UnloadMusicStream(bgm);
    player_destroy(player);
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

static void _draw_hud()
{

}
