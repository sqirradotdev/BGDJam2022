#include "state.h"

#include <raylib.h>

#include "state_menu.h"
#include "state_main.h"

State current_state = STATE_NONE;
State target_state = STATE_NONE;

void _enter_state(State state);
void _exit_state(State state);
void _check_state_switch();

void state_switchto(State state)
{
    target_state = state;
}

void state_restart()
{
    state_switchto(current_state);
}

void state_init()
{
    state_switchto(STATE_MENU);
}

void state_update()
{
    _check_state_switch();

    switch (current_state)
    {
        case STATE_MENU:    state_menu_update();    break;
        case STATE_MAIN:    state_main_update();    break;
        default: break;
    }
}

void state_draw()
{
    switch (current_state)
    {
        case STATE_MENU:    state_menu_draw();      break;
        case STATE_MAIN:    state_main_draw();      break;
        default: break;
    }
}

void state_clean()
{
    _exit_state(current_state);    
}

void _enter_state(State state)
{
    switch (state)
    {
        case STATE_MENU:    state_menu_enter();     break;
        case STATE_MAIN:    state_main_enter();     break;
        default: break;
    }
}

void _exit_state(State state)
{
    switch (state)
    {
        case STATE_MENU:    state_menu_exit();      break;
        case STATE_MAIN:    state_main_exit();      break;
        default: break;
    }
}

void _check_state_switch()
{
    if (target_state != STATE_NONE)
    {
        // Exit current state first
        _exit_state(current_state);
        _enter_state(target_state);

        current_state = target_state;
        target_state = STATE_NONE;
    }
}
