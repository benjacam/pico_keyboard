#pragma once

#include "key_config.h"

#define DEBOUNCE_TIME 30

#define HOLD_TIME 200

typedef enum key_state
{
    KEY_OFF,
    KEY_DEBOUNCE,
    KEY_ON,
    KEY_HELD,
} key_state_t;

typedef struct keyk
{
    key_state_t state;
    key_state_t last_state;
    // +ve for press, -ve for unpressed
    int32_t age;
    // Duration of last press
    uint32_t last_press_duration;
} keyk_t;

/* Set the current press state of the key */
void Key_SetPressed(keyk_t *key, bool pressed);

/*  Peek at the event associated with the key given its config. 
    Peeking means the event is returned without changing the state of the key.
*/
key_event_t Key_PeekEvent(const keyk_t *key, const key_config_t *config);

/*  Get the event associated with the key given its config and clear the
    state associated with the event (e.g. for a tapped key)
*/
key_event_t Key_GetEvent(keyk_t *key, const key_config_t *config);

void Key_SetHeld(keyk_t *key, const key_config_t *config);

/* Basic query if the key is physically pressed */
static inline bool Key_IsPressed(const keyk_t *key)
{
    return key->state == KEY_ON || key->state == KEY_HELD;
}

/* Query if the key is held */
static inline bool Key_IsHeld(const keyk_t *key)
{
    return key->state == KEY_HELD;
}

static inline bool Key_WasJustPressed(const keyk_t *key)
{
    return key->state == KEY_ON && key->last_state == KEY_DEBOUNCE;
}

static inline bool Key_WasJustReleased(const keyk_t *key)
{
    return key->state == KEY_DEBOUNCE && (key->last_state == KEY_ON || key->last_state == KEY_HELD);
}
