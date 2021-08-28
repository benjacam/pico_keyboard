#include "key.h"

void Key_SetPressed(keyk_t *key, bool pressed)
{
    key_state_t first_state = key->state;

    switch (key->state)
    {
        case KEY_OFF:
            if (pressed)
            {
                key->state = KEY_DEBOUNCE;
                key->age = 0;
            }
            else
            {
                key->age -= 1;
            }
        break;

        case KEY_DEBOUNCE:
            if (pressed)
            {
                key->age += 1;
                if (key->age > DEBOUNCE_TIME)
                {
                    key->state = KEY_ON;
                }
            }
            else
            {
                key->age -= 1;
                if (key->age < -DEBOUNCE_TIME)
                {
                    key->state = KEY_OFF;
                }
            }
        break;

        case KEY_ON:
            if (pressed)
            {
                key->age += 1;
            }
            else
            {
                key->state = KEY_DEBOUNCE;
                key->last_press_duration = key->age;
                key->age = 0;
            }
        break;

        default:
        break;
    }
}

key_event_t Key_PeekEvent(const keyk_t *key, const key_config_t *config)
{
    key_event_t event = EVENT_NONE;

    switch (config->mode)
    {
        case KEY_CONFIG_PRESS:
            if (key->state == KEY_ON)
            {
                event = config->on_press;
            }
        break;

        case KEY_CONFIG_TAP_HOLD:
            if (config->th.on_tap && key->last_press_duration && key->last_press_duration < HOLD_TIME)
            {
                event = config->th.on_tap;
            }
            else if (config->th.on_hold && key->state == KEY_ON && key->age >= HOLD_TIME)
            {
                event = config->th.on_hold;
            }
        break;
        default:
        break;
    }

    return event;
}

key_event_t Key_GetEvent(keyk_t *key, const key_config_t *config)
{
    key_event_t event = Key_PeekEvent(key, config);
    key->last_press_duration = 0;
    return event;
}
