#pragma once

#include "key_events.h"

typedef enum config_mode
{
    KEY_CONFIG_NONE,
    KEY_CONFIG_PRESS,
    KEY_CONFIG_TAP_HOLD
} key_config_mode_t;

/* Keys only supports either on_press, or on_tap/on_hold */
typedef struct key_config
{
    key_config_mode_t mode;
    union
    {
        /* Key to emit on first press */
        key_event_t on_press;
        struct tap_hold
        {
            /* Key to emit on tap (and release) */
            key_event_t on_tap;
            /* Key to emit on hold */
            key_event_t on_hold;
        } th;
    };

} key_config_t;

/* Macro defining a button that generates events for presses only */
#define ON_PRESS(key)          {.mode = KEY_CONFIG_PRESS, .on_press = EVENT_##key}
/* Macro defining a button that generates events for tap and hold */
#define ON_TAP_HOLD(tap, hold) {.mode = KEY_CONFIG_TAP_HOLD, .th = {.on_tap = EVENT_##tap, .on_hold = EVENT_##hold}}
/* Macro defining a button that doesn't generate events */
#define ON_NOWT                {.mode = KEY_CONFIG_NONE}

static inline bool KeyConfig_IsTapHold(const key_config_t *c)
{
    return c->mode == KEY_CONFIG_TAP_HOLD;
}
