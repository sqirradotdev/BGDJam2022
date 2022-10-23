#include "global_resources.h"

Font gr_medium_font;
Font gr_small_font;

Sound gr_hurt_sfx;
Sound gr_jump_sfx;
Sound gr_lose_sfx;
Sound gr_dash_sfx;
Sound gr_key_sfx;

void gr_load_global_resources()
{
    gr_medium_font = LoadFont("./resources/fonts/m5x7.fnt");
    gr_small_font = LoadFont("./resources/fonts/m3x6.fnt");

    gr_hurt_sfx = LoadSound("./resources/sfx/hurt.wav");
    gr_jump_sfx = LoadSound("./resources/sfx/jump.wav");
    gr_lose_sfx = LoadSound("./resources/sfx/lose.wav");
    gr_dash_sfx = LoadSound("./resources/sfx/dash.wav");
    gr_key_sfx = LoadSound("./resources/sfx/key.wav");
}
