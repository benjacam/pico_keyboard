#ifndef __KEY_EVENTS_H__
#define __KEY_EVENTS_H__

#include "tusb.h"
#include "key_layers.h"

typedef enum key_events
{
    EVENT_NONE = HID_KEY_NONE,
    EVENT_A = HID_KEY_A,
    EVENT_B = HID_KEY_B,
    EVENT_C = HID_KEY_C,
    EVENT_D = HID_KEY_D,
    EVENT_E = HID_KEY_E,
    EVENT_F = HID_KEY_F,
    EVENT_G = HID_KEY_G,
    EVENT_H = HID_KEY_H,
    EVENT_I = HID_KEY_I,
    EVENT_J = HID_KEY_J,
    EVENT_K = HID_KEY_K,
    EVENT_L = HID_KEY_L,
    EVENT_M = HID_KEY_M,
    EVENT_N = HID_KEY_N,
    EVENT_O = HID_KEY_O,
    EVENT_P = HID_KEY_P,
    EVENT_Q = HID_KEY_Q,
    EVENT_R = HID_KEY_R,
    EVENT_S = HID_KEY_S,
    EVENT_T = HID_KEY_T,
    EVENT_U = HID_KEY_U,
    EVENT_V = HID_KEY_V,
    EVENT_W = HID_KEY_W,
    EVENT_X = HID_KEY_X,
    EVENT_Y = HID_KEY_Y,
    EVENT_Z = HID_KEY_Z,
    EVENT_1 = HID_KEY_1,
    EVENT_2 = HID_KEY_2,
    EVENT_3 = HID_KEY_3,
    EVENT_4 = HID_KEY_4,
    EVENT_5 = HID_KEY_5,
    EVENT_6 = HID_KEY_6,
    EVENT_7 = HID_KEY_7,
    EVENT_8 = HID_KEY_8,
    EVENT_9 = HID_KEY_9,
    EVENT_0 = HID_KEY_0,
    EVENT_ENTER = HID_KEY_ENTER,
    EVENT_ESCAPE = HID_KEY_ESCAPE,
    EVENT_BACKSPACE = HID_KEY_BACKSPACE,
    EVENT_TAB = HID_KEY_TAB,
    EVENT_SPACE = HID_KEY_SPACE,
    EVENT_MINUS = HID_KEY_MINUS,
    EVENT_EQUAL = HID_KEY_EQUAL,
    EVENT_BRACKET_LEFT = HID_KEY_BRACKET_LEFT,
    EVENT_BRACKET_RIGHT = HID_KEY_BRACKET_RIGHT,
    EVENT_BACKSLASH = HID_KEY_BACKSLASH,
    EVENT_EUROPE_1 = HID_KEY_EUROPE_1,
    EVENT_SEMICOLON = HID_KEY_SEMICOLON,
    EVENT_APOSTROPHE = HID_KEY_APOSTROPHE,
    EVENT_GRAVE = HID_KEY_GRAVE,
    EVENT_COMMA = HID_KEY_COMMA,
    EVENT_PERIOD = HID_KEY_PERIOD,
    EVENT_SLASH = HID_KEY_SLASH,
    EVENT_CAPS_LOCK = HID_KEY_CAPS_LOCK,
    EVENT_F1 = HID_KEY_F1,
    EVENT_F2 = HID_KEY_F2,
    EVENT_F3 = HID_KEY_F3,
    EVENT_F4 = HID_KEY_F4,
    EVENT_F5 = HID_KEY_F5,
    EVENT_F6 = HID_KEY_F6,
    EVENT_F7 = HID_KEY_F7,
    EVENT_F8 = HID_KEY_F8,
    EVENT_F9 = HID_KEY_F9,
    EVENT_F10 = HID_KEY_F10,
    EVENT_F11 = HID_KEY_F11,
    EVENT_F12 = HID_KEY_F12,
    EVENT_PRINT_SCREEN = HID_KEY_PRINT_SCREEN,
    EVENT_SCROLL_LOCK = HID_KEY_SCROLL_LOCK,
    EVENT_PAUSE = HID_KEY_PAUSE,
    EVENT_INSERT = HID_KEY_INSERT,
    EVENT_HOME = HID_KEY_HOME,
    EVENT_PAGE_UP = HID_KEY_PAGE_UP,
    EVENT_DELETE = HID_KEY_DELETE,
    EVENT_END = HID_KEY_END,
    EVENT_PAGE_DOWN = HID_KEY_PAGE_DOWN,
    EVENT_ARROW_RIGHT = HID_KEY_ARROW_RIGHT,
    EVENT_ARROW_LEFT = HID_KEY_ARROW_LEFT,
    EVENT_ARROW_DOWN = HID_KEY_ARROW_DOWN,
    EVENT_ARROW_UP = HID_KEY_ARROW_UP,
    EVENT_NUM_LOCK = HID_KEY_NUM_LOCK,
    EVENT_KEYPAD_DIVIDE = HID_KEY_KEYPAD_DIVIDE,
    EVENT_KEYPAD_MULTIPLY = HID_KEY_KEYPAD_MULTIPLY,
    EVENT_KEYPAD_SUBTRACT = HID_KEY_KEYPAD_SUBTRACT,
    EVENT_KEYPAD_ADD = HID_KEY_KEYPAD_ADD,
    EVENT_KEYPAD_ENTER = HID_KEY_KEYPAD_ENTER,
    EVENT_KEYPAD_1 = HID_KEY_KEYPAD_1,
    EVENT_KEYPAD_2 = HID_KEY_KEYPAD_2,
    EVENT_KEYPAD_3 = HID_KEY_KEYPAD_3,
    EVENT_KEYPAD_4 = HID_KEY_KEYPAD_4,
    EVENT_KEYPAD_5 = HID_KEY_KEYPAD_5,
    EVENT_KEYPAD_6 = HID_KEY_KEYPAD_6,
    EVENT_KEYPAD_7 = HID_KEY_KEYPAD_7,
    EVENT_KEYPAD_8 = HID_KEY_KEYPAD_8,
    EVENT_KEYPAD_9 = HID_KEY_KEYPAD_9,
    EVENT_KEYPAD_0 = HID_KEY_KEYPAD_0,
    EVENT_KEYPAD_DECIMAL = HID_KEY_KEYPAD_DECIMAL,
    EVENT_EUROPE_2 = HID_KEY_EUROPE_2,
    EVENT_APPLICATION = HID_KEY_APPLICATION,
    EVENT_POWER = HID_KEY_POWER,
    EVENT_KEYPAD_EQUAL = HID_KEY_KEYPAD_EQUAL,
    EVENT_F13 = HID_KEY_F13,
    EVENT_F14 = HID_KEY_F14,
    EVENT_F15 = HID_KEY_F15,
    EVENT_F16 = HID_KEY_F16,
    EVENT_F17 = HID_KEY_F17,
    EVENT_F18 = HID_KEY_F18,
    EVENT_F19 = HID_KEY_F19,
    EVENT_F20 = HID_KEY_F20,
    EVENT_F21 = HID_KEY_F21,
    EVENT_F22 = HID_KEY_F22,
    EVENT_F23 = HID_KEY_F23,
    EVENT_F24 = HID_KEY_F24,
    EVENT_EXECUTE = HID_KEY_EXECUTE,
    EVENT_HELP = HID_KEY_HELP,
    EVENT_MENU = HID_KEY_MENU,
    EVENT_SELECT = HID_KEY_SELECT,
    EVENT_STOP = HID_KEY_STOP,
    EVENT_AGAIN = HID_KEY_AGAIN,
    EVENT_UNDO = HID_KEY_UNDO,
    EVENT_CUT = HID_KEY_CUT,
    EVENT_COPY = HID_KEY_COPY,
    EVENT_PASTE = HID_KEY_PASTE,
    EVENT_FIND = HID_KEY_FIND,
    EVENT_MUTE = HID_KEY_MUTE,
    EVENT_VOLUME_UP = HID_KEY_VOLUME_UP,
    EVENT_VOLUME_DOWN = HID_KEY_VOLUME_DOWN,
    EVENT_LOCKING_CAPS_LOCK = HID_KEY_LOCKING_CAPS_LOCK,
    EVENT_LOCKING_NUM_LOCK = HID_KEY_LOCKING_NUM_LOCK,
    EVENT_LOCKING_SCROLL_LOCK = HID_KEY_LOCKING_SCROLL_LOCK,
    EVENT_KEYPAD_COMMA = HID_KEY_KEYPAD_COMMA,
    EVENT_KEYPAD_EQUAL_SIGN = HID_KEY_KEYPAD_EQUAL_SIGN,
    EVENT_KANJI1 = HID_KEY_KANJI1,
    EVENT_KANJI2 = HID_KEY_KANJI2,
    EVENT_KANJI3 = HID_KEY_KANJI3,
    EVENT_KANJI4 = HID_KEY_KANJI4,
    EVENT_KANJI5 = HID_KEY_KANJI5,
    EVENT_KANJI6 = HID_KEY_KANJI6,
    EVENT_KANJI7 = HID_KEY_KANJI7,
    EVENT_KANJI8 = HID_KEY_KANJI8,
    EVENT_KANJI9 = HID_KEY_KANJI9,
    EVENT_LANG1 = HID_KEY_LANG1,
    EVENT_LANG2 = HID_KEY_LANG2,
    EVENT_LANG3 = HID_KEY_LANG3,
    EVENT_LANG4 = HID_KEY_LANG4,
    EVENT_LANG5 = HID_KEY_LANG5,
    EVENT_LANG6 = HID_KEY_LANG6,
    EVENT_LANG7 = HID_KEY_LANG7,
    EVENT_LANG8 = HID_KEY_LANG8,
    EVENT_LANG9 = HID_KEY_LANG9,
    EVENT_ALTERNATE_ERASE = HID_KEY_ALTERNATE_ERASE,
    EVENT_SYSREQ_ATTENTION = HID_KEY_SYSREQ_ATTENTION,
    EVENT_CANCEL = HID_KEY_CANCEL,
    EVENT_CLEAR = HID_KEY_CLEAR,
    EVENT_PRIOR = HID_KEY_PRIOR,
    EVENT_RETURN = HID_KEY_RETURN,
    EVENT_SEPARATOR = HID_KEY_SEPARATOR,
    EVENT_OUT = HID_KEY_OUT,
    EVENT_OPER = HID_KEY_OPER,
    EVENT_CLEAR_AGAIN = HID_KEY_CLEAR_AGAIN,
    EVENT_CRSEL_PROPS = HID_KEY_CRSEL_PROPS,
    EVENT_EXSEL = HID_KEY_EXSEL,
    EVENT_CONTROL_LEFT = HID_KEY_CONTROL_LEFT,
    EVENT_SHIFT_LEFT = HID_KEY_SHIFT_LEFT,
    EVENT_ALT_LEFT = HID_KEY_ALT_LEFT,
    EVENT_GUI_LEFT = HID_KEY_GUI_LEFT,
    EVENT_CONTROL_RIGHT = HID_KEY_CONTROL_RIGHT,
    EVENT_SHIFT_RIGHT = HID_KEY_SHIFT_RIGHT,
    EVENT_ALT_RIGHT = HID_KEY_ALT_RIGHT,
    EVENT_GUI_RIGHT = HID_KEY_GUI_RIGHT,
    EVENT_MOUSE_BUTTON_LEFT,
    EVENT_MOUSE_BUTTON_RIGHT,
    EVENT_MOUSE_BUTTON_MIDDLE,
    EVENT_MOUSE_BUTTON_BACKWARD,
    EVENT_MOUSE_BUTTON_FORWARD,
    EVENT_MOUSE_MOVEMENT_UP,
    EVENT_MOUSE_MOVEMENT_DOWN,
    EVENT_MOUSE_MOVEMENT_LEFT,
    EVENT_MOUSE_MOVEMENT_RIGHT,
    EVENT_MOUSE_WHEEL_UP,
    EVENT_MOUSE_WHEEL_DOWN,
    EVENT_MOUSE_WHEEL_LEFT,
    EVENT_MOUSE_WHEEL_RIGHT,
    EVENT_LAYER_BASE,
    EVENT_LAYER_NAV,
    EVENT_LAYER_MOUSE,
    EVENT_LAYER_NUM,
    EVENT_LAYER_FUN,
    EVENT_LAYER_SYM,
    EVENTS_END,

} key_event_t;

static inline bool Event_IsKey(key_event_t event)
{
    return event > EVENT_NONE && event <= EVENT_EXSEL;
}

static inline bool Event_IsKeyModifier(key_event_t event)
{
    return event >= EVENT_CONTROL_LEFT && event <= EVENT_GUI_RIGHT;
}

static inline bool Event_IsMouseButton(key_event_t event)
{
    return event >= EVENT_MOUSE_BUTTON_LEFT && event <= EVENT_MOUSE_BUTTON_FORWARD;
}

static inline bool Event_IsMouseMove(key_event_t event)
{
    return event >= EVENT_MOUSE_MOVEMENT_UP && event <= EVENT_MOUSE_WHEEL_RIGHT;
}

static inline uint8_t Event_GetHidKey(key_event_t event)
{
    return (uint8_t)(event & 0xff);
}
static inline hid_keyboard_modifier_bm_t Event_GetHidKeyModifier(key_event_t event)
{
    switch (event)
    {
        case EVENT_CONTROL_LEFT:
            return KEYBOARD_MODIFIER_LEFTCTRL;
        case EVENT_SHIFT_LEFT:
            return KEYBOARD_MODIFIER_LEFTSHIFT;
        case EVENT_ALT_LEFT:
            return KEYBOARD_MODIFIER_LEFTALT;
        case EVENT_GUI_LEFT:
            return KEYBOARD_MODIFIER_LEFTGUI;
        case EVENT_CONTROL_RIGHT:
            return KEYBOARD_MODIFIER_RIGHTCTRL;
        case EVENT_SHIFT_RIGHT:
            return KEYBOARD_MODIFIER_RIGHTSHIFT;
        case EVENT_ALT_RIGHT:
            return KEYBOARD_MODIFIER_RIGHTALT;
        case EVENT_GUI_RIGHT:
            return KEYBOARD_MODIFIER_RIGHTGUI;
        default:
            return 0;
    }
}

static inline hid_mouse_button_bm_t Event_GetHidMouseButton(key_event_t event)
{
    switch (event)
    {
        case EVENT_MOUSE_BUTTON_LEFT:
            return MOUSE_BUTTON_LEFT;
        case EVENT_MOUSE_BUTTON_RIGHT:
            return MOUSE_BUTTON_RIGHT;
        case EVENT_MOUSE_BUTTON_MIDDLE:
            return MOUSE_BUTTON_MIDDLE;
        case EVENT_MOUSE_BUTTON_BACKWARD:
            return MOUSE_BUTTON_BACKWARD;
        case EVENT_MOUSE_BUTTON_FORWARD :
            return MOUSE_BUTTON_FORWARD;
        default:
            return 0;
    }
}

static inline layer_t Event_GetLayer(key_event_t event)
{
    layer_t l = LAYER_BASE;
    if (event >= EVENT_LAYER_BASE && event <= EVENT_LAYER_SYM)
    {
        l = event - EVENT_LAYER_BASE;
    }
    return l;
}

#endif