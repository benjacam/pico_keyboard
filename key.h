#pragma once

#include "key_config.h"

#define DEBOUNCE_TIME 10

#define HOLD_TIME 300

typedef enum key_state
{
    KEY_OFF,
    KEY_DEBOUNCE,
    KEY_ON,
} key_state_t;

typedef struct keyk
{
    key_state_t state;
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

/* Basic query if the key is physically pressed */
static inline bool Key_IsPressed(const keyk_t *key)
{
    return key->state == KEY_ON;
}
