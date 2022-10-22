#include "tween.h"

#include <raylib.h>
#include <math.h>

static float _ease_out_bounce(float t);

Tween* tween_new(float* value_to_tween, float from, float to, float duration, EaseType ease)
{
    Tween* tween = (Tween*)malloc(sizeof(Tween));
    tween->value_to_tween = value_to_tween;
    tween->from = from;
    tween->to = to;
    tween->duration = duration;
    tween->ease_type = ease;

    tween->on_tween_finished_ptr = NULL;

    tween->current_time = 0.0;
    tween->is_playing = false;

    return tween;
}

void tween_start(Tween* tween)
{
    tween->is_playing = true;
    tween->current_time = 0.0;
}

void tween_pause(Tween* tween)
{
    tween->is_playing = false;
}

void tween_resume(Tween* tween)
{
    tween->is_playing = true;
}

void tween_stop(Tween* tween)
{
    tween->is_playing = false;
    tween->current_time = 0.0;
}

void tween_update(Tween* tween)
{
    if (!tween->is_playing)
        return;

    tween->current_time += GetFrameTime();

    if (tween->current_time >= tween->duration)
    {
        *tween->value_to_tween = tween->to;
        tween->is_playing = false;
        tween->current_time = tween->duration;

        if (*tween->on_tween_finished_ptr != NULL)
            (*tween->on_tween_finished_ptr)();

        return;
    }

    float t = tween->current_time / tween->duration;
    switch (tween->ease_type)
    {
        case EASE_LINEAR:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * t;
            break;
        case EASE_IN_QUAD:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * t * t;
            break;
        case EASE_OUT_QUAD:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * t * (2 - t);
            break;
        case EASE_IN_OUT_QUAD:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * t * t * (3 - 2 * t);
            break;
        case EASE_IN_CUBIC:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * t * t * t;
            break;
        case EASE_OUT_CUBIC:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * ((t - 1) * (t - 1) * (t - 1) + 1);
            break;
        case EASE_IN_OUT_CUBIC:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (t < 0.5 ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1);
            break;
        case EASE_IN_QUART:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * t * t * t * t;
            break;
        case EASE_OUT_QUART:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (1 - (t - 1) * (t - 1) * (t - 1) * (t - 1));
            break;
        case EASE_IN_OUT_QUART:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (t < 0.5 ? 8 * t * t * t * t : 1 - 8 * (t - 1) * (t - 1) * (t - 1) * (t - 1));
            break;
        case EASE_IN_QUINT:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * t * t * t * t * t;
            break;
        case EASE_OUT_QUINT:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (1 + (t - 1) * (t - 1) * (t - 1) * (t - 1) * (t - 1));
            break;
        case EASE_IN_OUT_QUINT:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (t < 0.5 ? 16 * t * t * t * t * t : 1 + 16 * (t - 1) * (t - 1) * (t - 1) * (t - 1) * (t - 1));
            break;
        case EASE_IN_SINE:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (1 - cos(t * PI / 2));
            break;
        case EASE_OUT_SINE:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * sin(t * PI / 2);
            break;
        case EASE_IN_OUT_SINE:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (1 - cos(t * PI)) / 2;
            break;
        case EASE_IN_EXPO:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * pow(2, 10 * (t - 1));
            break;
        case EASE_OUT_EXPO:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (-pow(2, -10 * t) + 1);
            break;
        case EASE_IN_OUT_EXPO:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (t < 0.5 ? pow(2, 20 * t - 10) / 2 : (-pow(2, -20 * t + 10) + 2) / 2);
            break;
        case EASE_IN_CIRC:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (1 - sqrt(1 - t * t));
            break;
        case EASE_OUT_CIRC:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * sqrt(1 - (t - 1) * (t - 1));
            break;
        case EASE_IN_OUT_CIRC:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (t < 0.5 ? (1 - sqrt(1 - 4 * t * t)) / 2 : (sqrt(1 - (-2 * t + 2) * (-2 * t + 2)) + 1) / 2);
            break;
        case EASE_IN_ELASTIC:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (t == 0 ? 0 : t == 1 ? 1 : -pow(2, 10 * t - 10) * sin((t * 10 - 10.75) * (2 * PI) / 3));
            break;
        case EASE_OUT_ELASTIC:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (t == 0 ? 0 : t == 1 ? 1 : pow(2, -10 * t) * sin((t * 10 - 0.75) * (2 * PI) / 3) + 1);
            break;
        case EASE_IN_OUT_ELASTIC:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (t == 0 ? 0 : t == 1 ? 1 : t < 0.5 ? -(pow(2, 20 * t - 10) * sin((20 * t - 11.125) * (2 * PI) / 4.5)) / 2 : (pow(2, -20 * t + 10) * sin((20 * t - 11.125) * (2 * PI) / 4.5)) / 2 + 1);
            break;
        case EASE_IN_BACK:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (t * t * t - t * sin(t * PI));
            break;
        case EASE_OUT_BACK:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (1 + (t - 1) * (t - 1) * (t - 1) + (t - 1) * sin((t - 1) * PI));
            break;
        case EASE_IN_OUT_BACK:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (t < 0.5 ? (2 * t * t * t - 2 * t * sin(2 * t * PI)) / 2 : (2 * t * t * t + 2 * t * sin(2 * t * PI)) / 2 + 1);
            break;
        case EASE_IN_BOUNCE:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (1 - _ease_out_bounce(1 - t));
            break;
        case EASE_OUT_BOUNCE:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * _ease_out_bounce(t);
            break;
        case EASE_IN_OUT_BOUNCE:
            *tween->value_to_tween = tween->from + (tween->to - tween->from) * (t < 0.5 ? (1 - _ease_out_bounce(1 - 2 * t)) / 2 : (1 + _ease_out_bounce(2 * t - 1)) / 2);
            break;
        default:
            break;
    }
}

void tween_destroy(Tween* tween)
{
    free(tween);
}

static float _ease_out_bounce(float t)
{
    float n1 = 7.5625;
    float d1 = 2.75;

    if (t < 1 / d1)
    {
        return n1 * t * t;
    }
    else if (t < 2 / d1)
    {
        t -= 1.5 / d1;
        return n1 * t * t + 0.75;
    }
    else if (t < 2.5 / d1)
    {
        t -= 2.25 / d1;
        return n1 * t * t + 0.9375;
    }
    else
    {
        t -= 2.625 / d1;
        return n1 * t * t + 0.984375;
    }
}
