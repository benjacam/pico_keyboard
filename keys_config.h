#pragma once

#include "key_config.h"
#include "key_events.h"
#include "key_layers.h"
#include "key.h"

// Number of columns on both hands
#define COLS 10
// Number of rows on each hand
#define ROWS 4

/* Helper macro to interate through columns */
#define FOREACH_COL(c) for (uint (c) = 0; (c) < COLS; (c)++)

/* Helper macro to interate through rows */
#define FOREACH_ROW(r) for (uint (r) = 0; (r) < ROWS; (r)++)

/* The configuration for each key on a layer */
typedef struct layer_config
{
    /* A layer may optionally specifiy any event that will persist
    whilst the layer is active (typically used for shift) */
    key_event_t layer_modifier;
    /* The configuration for each key in the layer */
    const key_config_t kcfg[ROWS][COLS];
} layer_config_t;

/* A list of supported mouse movement modes */
typedef enum mouse_move_modes
{
    /* Very basic simulation of a ball on a tilt table. The mouse move buttons adjust the tilt 
    of the table. The table resets when exiting mouse layer. The tilt is integrated
    every key scan (1ms), so the amount of tilt is proportional to the length of
    the button press. */
    MOUSE_MOVE_MODE_TILT_TABLE,
} mouse_move_modes_t;

// Configuration structure for mouse movement
typedef struct mouse_move_config
{
    /* The mode to which this config applies (i.e. selects the union member) */
    mouse_move_modes_t mode;
    union // anon
    {
        struct tilt_table_config
        {
            // Time between cursor movements
            uint interval;
            // Amount to divide the integrated tilt
            int integral_divide;
        } tilt_table_config;   
    };
} mouse_move_config_t;

/* A list of supported mouse wheel modes */
typedef enum mouse_wheel_modes
{
    /* The wheel move buttons step when tapped and
       scroll at constant rate when held */
    MOUSE_WHEEL_MODE_CONSTANT,
} mouse_wheel_modes_t;

// Configuration structure for mouse wheel
typedef struct mouse_wheel_config
{
    /* The mode to which this config applies (i.e. selects the union member) */
    mouse_wheel_modes_t mode;
    union // anon
    {
        struct constant_config
        {
            // Time between cursor movements
            uint interval;
            // Scroll delta each time the button is pressed, or held
            int8_t step;            
        } constant_config;   
    };
} mouse_wheel_config_t;

/* The keyboard's keys */
extern keyk_t keys[ROWS][COLS];

/* List of single GPIO bitmasks used for the left hand rows */
extern const uint32_t row_gpios_l[ROWS];
/* List of single GPIO bitmasks used for the right hand rows */
extern const uint32_t row_gpios_r[ROWS];

/* Combined bitmask of every column GPIO (left and right) */
extern const uint32_t col_mask;
/* Combined bitmask of every row GPIO (left and right) */
extern const uint32_t row_mask;

/* Array defining the sequence in which to scan columns.
   Because there are indepentent row GPIOs on left and right hands
   we can scan left and right cols at the same time, hence COLS/2. */
extern const uint32_t col_scan_sequence[COLS/2];

/* The key configuration for each layer */
extern const layer_config_t layer_configs[NLAYERS];

/* The mouse movement configuration */
extern const mouse_move_config_t mouse_move_config;
/* The mouse wheel configuration */
extern const mouse_wheel_config_t mouse_wheel_config;

/* Get a key from its row/column */
static inline keyk_t *Keys_GetKey(uint r, uint c)
{
    return &(keys[r][c]);
}

/* Get a key's configuration given the current layer and its row/column */
static inline const key_config_t *Keys_GetKeyConfig(layer_t l, uint r, uint c)
{
    return &(layer_configs[l].kcfg[r][c]);
}

/* Get the layer's modifier event */ 
static inline key_event_t Keys_GetLayerModifier(layer_t l)
{
    return layer_configs[l].layer_modifier;
}
