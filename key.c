#include "key.h"

static void setState(keyk_t *key, key_state_t new_state)
{
    key->last_state = key->state;
    key->state = new_state;
}

static void handleReleased(keyk_t *key, uint32_t age)
{
    setState(key, KEY_DEBOUNCE);
    key->last_press_duration = age;
    key->age = 0;
}

void Key_SetPressed(keyk_t *key, bool pressed)
{
    setState(key, key->state);

    switch (key->state)
    {
        case KEY_OFF:
            if (pressed)
            {
                setState(key, KEY_DEBOUNCE);
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
                    setState(key, KEY_ON);
                }
            }
            else
            {
                key->age -= 1;
                if (key->age < -DEBOUNCE_TIME)
                {
                    setState(key, KEY_OFF);
                }
            }
        break;

        case KEY_ON:
            if (pressed)
            {
                key->age += 1;
                if (key->age > HOLD_TIME)
                {
                    setState(key, KEY_HELD);
                }
            }
            else
            {
                handleReleased(key, key->age);
            }
        break;

        case KEY_HELD:
            if (pressed)
            {
                key->age += 1;
            }
            else
            {
                handleReleased(key, key->age);
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
            if (Key_IsPressed(key))
            {
                event = config->on_press;
            }
        break;

        case KEY_CONFIG_RELEASE:
            if (key->last_press_duration)
            {
                event = config->on_release;
            }
        break;

        case KEY_CONFIG_TAP_HOLD:
            if (config->th.on_tap && key->last_press_duration && key->last_press_duration < HOLD_TIME)
            {
                event = config->th.on_tap;
            }
            else if (config->th.on_hold && Key_IsHeld(key))
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

void Key_SetHeld(keyk_t *key, const key_config_t *config)
{
    switch (config->mode)
    {
        case KEY_CONFIG_TAP_HOLD:
            switch (key->state)
            {
                case KEY_ON:
                    setState(key, KEY_HELD);
                    key->age = HOLD_TIME;
                break;

                default:
                break;
            }
        break;

        default:
        break;
    }
}
