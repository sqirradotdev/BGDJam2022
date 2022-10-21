#ifndef _STATE_H_
#define _STATE_H_

typedef enum
{
    STATE_NONE,
    STATE_MENU,
    STATE_MAIN
} State;

void state_switchto(State state);
void state_restart();

void state_init();
void state_update();
void state_draw();
void state_clean();

#endif
