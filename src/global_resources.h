#ifndef _GLOBAL_RESOURCES_H_
#define _GLOBAL_RESOURCES_H_

#include <raylib.h>

extern Font gr_medium_font;
extern Font gr_small_font;

extern Sound gr_hurt_sfx;
extern Sound gr_jump_sfx;
extern Sound gr_lose_sfx;
extern Sound gr_dash_sfx;
extern Sound gr_key_sfx;

void gr_load_global_resources();

#endif
