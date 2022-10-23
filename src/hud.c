#include "hud.h"

#include "constants.h"

HUD* hud_new(Player* player)
{
    HUD* hud = (HUD*)malloc(sizeof(HUD));
    hud->player = player;
    hud->hearts_texture = LoadTexture("./resources/textures/hearts.png");
    hud->keys_texture = LoadTexture("./resources/textures/keys.png");

    hud->h_blink_frames_counter = 0;

    return hud;
}

void hud_update(HUD* hud)
{
    if (hud->player->hp != hud->player->max_hp || hud->player->hp != 0)
    {
        int reset_at;
        switch (hud->player->hp)
        {
            case 1:
                reset_at = 30;
                break;
            case 2:
                reset_at = 100;
                break;
            default:
                reset_at = 150;
                break;
        }

        hud->h_blink_frames_counter++;

        if (hud->h_blink_frames_counter > reset_at)
            hud->h_blink_frames_counter = 0;    
    }
}

void hud_draw(HUD* hud)
{
    float heart_height = hud->hearts_texture.height / 3;

    for (int i = 0; i < hud->player->max_hp; i++)
    {
        Rectangle src_rect = { 0.0, 0.0, hud->hearts_texture.width, heart_height };
        if (i >= hud->player->hp)
        {
            src_rect.y = heart_height * 1;
            if (hud->h_blink_frames_counter <= 10)
                src_rect.y = heart_height * 2;
        }
        Rectangle dst_rect = { 7 + i * (hud->hearts_texture.width + 1), INIT_VIEWPORT_HEIGHT - heart_height - 7, hud->hearts_texture.width, heart_height };

        DrawTexturePro(hud->hearts_texture, src_rect, dst_rect, (Vector2) { 0.0, 0.0 }, 0.0, WHITE);
    }

    float key_height = hud->keys_texture.height / 2;

    for (int i = 0; i < hud->player->keys_target; i++)
    {
        Rectangle src_rect = { 0.0, 0.0, hud->keys_texture.width, key_height };
        if (i >= hud->player->keys_collected)
            src_rect.y = key_height * 1;
        Rectangle dst_rect = {
            (INIT_VIEWPORT_WIDTH - ((hud->keys_texture.width - 1) * hud->player->keys_target) - 7) + i * (hud->hearts_texture.width + 1),
            INIT_VIEWPORT_HEIGHT - key_height - 2,
            hud->keys_texture.width,
            key_height
        };

        DrawTexturePro(hud->keys_texture, src_rect, dst_rect, (Vector2) { 0.0, 0.0 }, 0.0, WHITE);
    }
}

void hud_destroy(HUD* hud)
{
    UnloadTexture(hud->hearts_texture);
    free(hud);
}
