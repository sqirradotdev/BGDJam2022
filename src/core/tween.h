#ifndef _TWEEN_H_
#define _TWEEN_H_

#include <stdbool.h>

#include "../util/list.h"

typedef void (*OnTweenFinishedPtr)(void);

typedef enum
{
    EASE_LINEAR,
    EASE_IN_QUAD,
    EASE_OUT_QUAD,
    EASE_IN_OUT_QUAD,
    EASE_IN_CUBIC,
    EASE_OUT_CUBIC,
    EASE_IN_OUT_CUBIC,
    EASE_IN_QUART,
    EASE_OUT_QUART,
    EASE_IN_OUT_QUART,
    EASE_IN_QUINT,
    EASE_OUT_QUINT,
    EASE_IN_OUT_QUINT,
    EASE_IN_SINE,
    EASE_OUT_SINE,
    EASE_IN_OUT_SINE,
    EASE_IN_EXPO,
    EASE_OUT_EXPO,
    EASE_IN_OUT_EXPO,
    EASE_IN_CIRC,
    EASE_OUT_CIRC,
    EASE_IN_OUT_CIRC,
    EASE_IN_ELASTIC,
    EASE_OUT_ELASTIC,
    EASE_IN_OUT_ELASTIC,
    EASE_IN_BACK,
    EASE_OUT_BACK,
    EASE_IN_OUT_BACK,
    EASE_IN_BOUNCE,
    EASE_OUT_BOUNCE,
    EASE_IN_OUT_BOUNCE
} EaseType;

typedef struct Tween
{
    float* value_to_tween;
    float from;
    float to;
    float duration;
    EaseType ease_type;

    OnTweenFinishedPtr on_tween_finished_ptr;

    float current_time;
    bool is_playing;
} Tween;

Tween* tween_new(float* value_to_tween, float from, float to, float duration, EaseType ease);
void tween_start(Tween* tween);
void tween_pause(Tween* tween);
void tween_resume(Tween* tween);
void tween_stop(Tween* tween);
void tween_update(Tween* tween);
void tween_destroy(Tween* tween);

#endif
