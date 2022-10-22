#include "hud.h"

#include "constants.h"

HUD* hud_new(Player* player)
{
    HUD* hud = (HUD*)malloc(sizeof(HUD));
    hud->player = player;
    hud->hearts_texture = LoadTexture("./resources/textures/hearts.png");

    return hud;
}

void hud_update(HUD* hud)
{
    if (hud->player->hp == hud->player->max_hp || hud->player->hp == 0)
        return;

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

void hud_draw(HUD* hud)
{
    float height = hud->hearts_texture.height / 3;

    for (int i = 0; i < hud->player->max_hp; i++)
    {
        Rectangle src_rect = { 0.0, 0.0, hud->hearts_texture.width, height };
        if (i >= hud->player->hp)
        {
            src_rect.y = height * 1;
            if (hud->h_blink_frames_counter <= 10)
                src_rect.y = height * 2;
        }
        Rectangle dst_rect = { 7 + i * (hud->hearts_texture.width + 1), INIT_VIEWPORT_HEIGHT - height - 7, hud->hearts_texture.width, height };

        DrawTexturePro(hud->hearts_texture, src_rect, dst_rect, (Vector2) { 0.0, 0.0 }, 0.0, WHITE);
    }
}

void hud_destroy(HUD* hud)
{
    UnloadTexture(hud->hearts_texture);
    free(hud);
}
