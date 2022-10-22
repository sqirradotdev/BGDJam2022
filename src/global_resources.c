#include "global_resources.h"

Font gr_medium_font;
Font gr_small_font;

void gr_load_global_resources()
{
    gr_medium_font = LoadFont("./resources/fonts/m5x7.fnt");
    gr_small_font = LoadFont("./resources/fonts/m3x6.fnt");
}
