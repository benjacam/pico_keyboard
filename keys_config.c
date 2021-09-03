#include "keys_config.h"

#define COL0_L 2
#define COL1_L 9
#define COL2_L 3
#define COL3_L 4
#define COL4_L 0
#define COL0_R 28
#define COL1_R 22
#define COL2_R 26
#define COL3_R 20
#define COL4_R 18

#define ROW0_L 1
#define ROW1_L 5
#define ROW2_L 10
#define ROW3_L 11
#define ROW0_R 17
#define ROW1_R 19
#define ROW2_R 21
#define ROW3_R 27

const uint32_t row_gpios_l[ROWS] = {1<<ROW0_L, 1<<ROW1_L, 1<<ROW2_L, 1<<ROW3_L};
const uint32_t row_gpios_r[ROWS] = {1<<ROW0_R, 1<<ROW1_R, 1<<ROW2_R, 1<<ROW3_R};

const uint32_t col_mask = (1<<COL0_L)|(1<<COL1_L)|(1<<COL2_L)|(1<<COL3_L)|(1<<COL4_L)|
                          (1<<COL0_R)|(1<<COL1_R)|(1<<COL2_R)|(1<<COL3_R)|(1<<COL4_R);
const uint32_t row_mask = (1<<ROW0_L)|(1<<ROW1_L)|(1<<ROW2_L)|(1<<ROW3_L)|
                          (1<<ROW0_R)|(1<<ROW1_R)|(1<<ROW2_R)|(1<<ROW3_R);

const uint32_t col_scan_sequence[COLS/2] =
{
    1<<COL0_L | 1<<COL0_R,
    1<<COL1_L | 1<<COL1_R,
    1<<COL2_L | 1<<COL2_R,
    1<<COL3_L | 1<<COL3_R,
    1<<COL4_L | 1<<COL4_R,
};

const layer_config_t layer_configs[NLAYERS] = 
{
    [LAYER_BASE] =
    {
        .layer_modifier=EVENT_NONE,
        .kcfg = {
            {ON_PRESS(Q),              ON_PRESS(W),              ON_PRESS(E),                  ON_PRESS(R),                   ON_PRESS(T),                      ON_PRESS(Y),                       ON_PRESS(U),                   ON_PRESS(I),                  ON_PRESS(O),               ON_PRESS(P)},
            {ON_PRESS(A),              ON_TAP_HOLD(S, ALT_LEFT), ON_TAP_HOLD(D, CONTROL_LEFT), ON_TAP_HOLD(F, SHIFT_LEFT),    ON_PRESS(G),                      ON_PRESS(H),                       ON_TAP_HOLD(J, SHIFT_LEFT),    ON_TAP_HOLD(K, CONTROL_LEFT), ON_TAP_HOLD(L, ALT_LEFT),  ON_TAP_HOLD(SEMICOLON, GUI_LEFT)},
            {ON_TAP_HOLD(Z, GUI_LEFT), ON_PRESS(X),              ON_PRESS(C),                  ON_PRESS(V),                   ON_PRESS(B),                      ON_PRESS(N),                       ON_PRESS(M),                   ON_PRESS(COMMA),              ON_PRESS(PERIOD),          ON_PRESS(SLASH)},
            {ON_NOWT,                  ON_NOWT,                  ON_PRESS(ESCAPE),             ON_TAP_HOLD(ENTER, LAYER_NAV), ON_TAP_HOLD(DELETE, LAYER_MOUSE), ON_TAP_HOLD(BACKSPACE, LAYER_SYM), ON_TAP_HOLD(SPACE, LAYER_NUM), ON_TAP_HOLD(TAB, LAYER_FUN),  ON_NOWT,                   ON_NOWT},
        }
    },
    [LAYER_NAV] =
    {
        .layer_modifier=EVENT_NONE,
        .kcfg = {
            {ON_NOWT,            ON_NOWT,            ON_NOWT,                ON_NOWT,                       ON_NOWT,          ON_PRESS(AGAIN),     ON_PRESS(PASTE),      ON_PRESS(COPY),       ON_PRESS(CUT),      ON_PRESS(UNDO)}, 
            {ON_NOWT,            ON_PRESS(ALT_LEFT), ON_PRESS(CONTROL_LEFT), ON_PRESS(SHIFT_LEFT),          ON_NOWT,          ON_PRESS(CAPS_LOCK), ON_PRESS(ARROW_LEFT), ON_PRESS(ARROW_DOWN), ON_PRESS(ARROW_UP), ON_PRESS(ARROW_RIGHT)},
            {ON_PRESS(GUI_LEFT), ON_NOWT,            ON_NOWT,                ON_NOWT,                       ON_NOWT,          ON_PRESS(INSERT),    ON_PRESS(HOME),       ON_PRESS(PAGE_DOWN),  ON_PRESS(PAGE_UP),  ON_PRESS(END)},
            {ON_NOWT,            ON_NOWT,            ON_PRESS(ESCAPE),       ON_TAP_HOLD(ENTER, LAYER_NAV), ON_PRESS(DELETE), ON_PRESS(BACKSPACE), ON_PRESS(SPACE),      ON_PRESS(TAB),        ON_NOWT,            ON_NOWT},
        },
    },
    [LAYER_MOUSE] = 
    {
        .layer_modifier=EVENT_NONE,
        .kcfg = {
            {ON_NOWT,            ON_NOWT,            ON_NOWT,                ON_NOWT,              ON_NOWT,                           ON_PRESS(AGAIN),             ON_PRESS(PASTE),               ON_PRESS(COPY),                ON_PRESS(CUT),               ON_PRESS(UNDO)}, 
            {ON_NOWT,            ON_PRESS(ALT_LEFT), ON_PRESS(CONTROL_LEFT), ON_PRESS(SHIFT_LEFT), ON_NOWT,                           ON_NOWT,                     ON_PRESS(MOUSE_MOVEMENT_LEFT), ON_PRESS(MOUSE_MOVEMENT_DOWN), ON_PRESS(MOUSE_MOVEMENT_UP), ON_PRESS(MOUSE_MOVEMENT_RIGHT)},
            {ON_PRESS(GUI_LEFT), ON_NOWT,            ON_NOWT,                ON_NOWT,              ON_NOWT,                           ON_NOWT,                     ON_PRESS(MOUSE_WHEEL_LEFT),    ON_PRESS(MOUSE_WHEEL_DOWN),    ON_PRESS(MOUSE_WHEEL_UP),    ON_PRESS(MOUSE_WHEEL_RIGHT)},
            {ON_NOWT,            ON_NOWT,            ON_PRESS(ESCAPE),       ON_PRESS(ENTER),      ON_TAP_HOLD(DELETE, LAYER_MOUSE),  ON_PRESS(MOUSE_BUTTON_LEFT), ON_PRESS(MOUSE_BUTTON_MIDDLE), ON_PRESS(MOUSE_BUTTON_RIGHT),  ON_NOWT,                     ON_NOWT},
        },
    },
    [LAYER_NUM] =
    {
        .layer_modifier=EVENT_NONE,
        .kcfg = {
            {ON_PRESS(BRACKET_LEFT), ON_PRESS(7), ON_PRESS(8),      ON_PRESS(9), ON_PRESS(BRACKET_RIGHT), ON_NOWT,             ON_NOWT,                           ON_NOWT,                ON_NOWT,            ON_PRESS(APOSTROPHE)},
            {ON_PRESS(APOSTROPHE),   ON_PRESS(4), ON_PRESS(5),      ON_PRESS(6), ON_PRESS(EQUAL),         ON_NOWT,             ON_PRESS(SHIFT_LEFT),              ON_PRESS(CONTROL_LEFT), ON_PRESS(ALT_LEFT), ON_PRESS(GUI_LEFT)},
            {ON_PRESS(GRAVE),        ON_PRESS(1), ON_PRESS(2),      ON_PRESS(3), ON_PRESS(BACKSLASH),     ON_NOWT,             ON_NOWT,                           ON_NOWT,                ON_NOWT,            ON_NOWT},
            {ON_NOWT,                ON_NOWT,     ON_PRESS(PERIOD), ON_PRESS(0), ON_PRESS(MINUS),         ON_PRESS(BACKSPACE), ON_TAP_HOLD(SPACE, LAYER_NUM),     ON_PRESS(TAB),          ON_NOWT,            ON_NOWT},
        },
    },
    [LAYER_SYM] = 
    {
        .layer_modifier=EVENT_SHIFT_LEFT,
        .kcfg = {
            {ON_PRESS(BRACKET_LEFT), ON_PRESS(7), ON_PRESS(8),      ON_PRESS(9), ON_PRESS(BRACKET_RIGHT), ON_NOWT,                           ON_NOWT,               ON_NOWT,                     ON_NOWT,            ON_PRESS(APOSTROPHE)},
            {ON_PRESS(APOSTROPHE),   ON_PRESS(4), ON_PRESS(5),      ON_PRESS(6), ON_PRESS(EQUAL),         ON_NOWT,                           ON_PRESS(SHIFT_LEFT), ON_PRESS(CONTROL_LEFT),      ON_PRESS(ALT_LEFT), ON_PRESS(GUI_LEFT)},
            {ON_PRESS(GRAVE),        ON_PRESS(1), ON_PRESS(2),      ON_PRESS(3), ON_PRESS(BACKSLASH),     ON_NOWT,                           ON_NOWT,              ON_NOWT,                     ON_NOWT,            ON_NOWT},
            {ON_NOWT,                ON_NOWT,     ON_PRESS(PERIOD), ON_PRESS(0), ON_PRESS(MINUS),         ON_TAP_HOLD(BACKSPACE, LAYER_SYM), ON_PRESS(SPACE),      ON_PRESS(TAB), ON_NOWT,            ON_NOWT},
        },
    },
    [LAYER_FUN] =
    {
        .layer_modifier=EVENT_NONE,
        .kcfg = {
            {ON_PRESS(F12), ON_PRESS(F7), ON_PRESS(F8),          ON_PRESS(F9),    ON_PRESS(PRINT_SCREEN), ON_NOWT,             ON_NOWT,              ON_NOWT,                     ON_NOWT,            ON_PRESS(APOSTROPHE)},
            {ON_PRESS(F11), ON_PRESS(F4), ON_PRESS(F5),          ON_PRESS(F6),    ON_PRESS(SCROLL_LOCK),  ON_NOWT,             ON_PRESS(SHIFT_LEFT), ON_PRESS(CONTROL_LEFT),      ON_PRESS(ALT_LEFT), ON_PRESS(GUI_LEFT)},
            {ON_PRESS(F10), ON_PRESS(F1), ON_PRESS(F2),          ON_PRESS(F3),    ON_PRESS(PAUSE),        ON_NOWT,             ON_NOWT,              ON_NOWT,                     ON_NOWT,            ON_NOWT},
            {ON_NOWT,       ON_NOWT,      ON_PRESS(APPLICATION), ON_PRESS(ENTER), ON_PRESS(DELETE),       ON_PRESS(BACKSPACE), ON_PRESS(SPACE),      ON_TAP_HOLD(TAB, LAYER_FUN), ON_NOWT,            ON_NOWT},
        },
    },
};

keyk_t keys[ROWS][COLS];

const mouse_move_config_t mouse_move_config = {
    .mode = MOUSE_MOVE_MODE_TILT_TABLE,
    .tilt_table_config = { .interval = 50, .integral_divide=16},
};
const mouse_wheel_config_t mouse_wheel_config = {
    .mode = MOUSE_WHEEL_MODE_CONSTANT,
    .constant_config = {.interval = 100, .step = 1},
};
