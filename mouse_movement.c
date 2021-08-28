#include "mouse_movement.h"
#include "keys_config.h"
#include "key_scan.h"

/* Must be in the same order as the events
    EVENT_MOUSE_MOVEMENT_UP,
    EVENT_MOUSE_MOVEMENT_DOWN,
    EVENT_MOUSE_MOVEMENT_LEFT,
    EVENT_MOUSE_MOVEMENT_RIGHT, and
    EVENT_MOUSE_WHEEL_UP,
    EVENT_MOUSE_WHEEL_DOWN,
    EVENT_MOUSE_WHEEL_LEFT,
    EVENT_MOUSE_WHEEL_RIGHT,
*/
enum directions
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    N_DIRECTIONS,
};

struct mouse_movement_state
{
    uint mouse_ages[N_DIRECTIONS];
    uint wheel_ages[N_DIRECTIONS];
    int mouse_up_down_integral;
    int mouse_left_right_integral;

} state;

void MouseMovement_HandleKeyEvent(key_event_t* set_events, uint n_set)
{
    if (KeyScan_GetCurrentLayer() == LAYER_MOUSE)
    {
        for (uint d = 0; d < N_DIRECTIONS; d++)
        {
            bool mouse_found = false;
            bool wheel_found = false;

            for (uint e = 0; e < n_set; e++)
            {
                key_event_t event = set_events[e];
                if (event == (EVENT_MOUSE_MOVEMENT_UP + d))
                {
                    mouse_found = true;
                }
                else if (event == (EVENT_MOUSE_WHEEL_UP + d))
                {
                    wheel_found = true;
                }
            }

            if (mouse_found)
            {
                switch (d)
                {
                    case UP:    state.mouse_up_down_integral++;    break;
                    case DOWN:  state.mouse_up_down_integral--;    break;
                    case LEFT:  state.mouse_left_right_integral++; break;
                    case RIGHT: state.mouse_left_right_integral--; break;
                    default: break;
                }
                state.mouse_ages[d]++;
            }
            else
            {
                state.mouse_ages[d] = 0;
            }
            if (wheel_found)
            {
                state.wheel_ages[d]++;
            }
            else
            {
                state.wheel_ages[d] = 0;
            }
        }

    }
    else
    {
        memset(&state, 0, sizeof(state));
    }
}

bool MouseMovement_GetMouseDeltas(int8_t *x, int8_t *y)
{
    bool report = false;

    static uint time_since_x_report = 0;
    static uint time_since_y_report = 0;

    switch (mouse_move_config.mode)
    {
        case MOUSE_MOVE_MODE_TILT_TABLE:
            if (state.mouse_left_right_integral)
            {
                if (time_since_x_report++ > mouse_move_config.tilt_table_config.interval)
                {
                    report = true;
                    time_since_x_report = 0;

                    int xres = state.mouse_left_right_integral / mouse_move_config.tilt_table_config.integral_divide;
                    xres = MAX(xres, -127);
                    xres = MIN(xres, 127);
                    *x = xres;
                }
            }
            else
            {
                time_since_x_report = 0;
            }
            if (state.mouse_up_down_integral)
            {
                if (time_since_y_report++ > mouse_move_config.tilt_table_config.interval)
                {
                    report = true;
                    time_since_y_report = 0;

                    int yres = state.mouse_up_down_integral / mouse_move_config.tilt_table_config.integral_divide;
                    yres = MAX(yres, -127);
                    yres = MIN(yres, 127);
                    *y = yres;
                }
            }
            else
            {
                time_since_y_report = 0;
            }
        break;
        default:
        break;
    }

    return report;
}

bool MouseMovement_GetWheelDeltas(int8_t *vertical, int8_t *horizontal)
{
    bool report = false;

    static uint time_since_v_report = 0;
    static uint time_since_h_report = 0;

    switch (mouse_wheel_config.mode)
    {
        case MOUSE_WHEEL_MODE_CONSTANT:
        {
            int8_t step = mouse_wheel_config.constant_config.step;
            uint interval = mouse_wheel_config.constant_config.interval;

            if (state.wheel_ages[UP] || state.wheel_ages[DOWN])
            {
                if (time_since_v_report++ > interval)
                {
                    report = true;
                    time_since_v_report = 0;
                    *vertical = state.wheel_ages[UP] ? step : -step;
                }
            }
            else
            {
                time_since_v_report = 0;
            }
                
            if (state.wheel_ages[LEFT] || state.wheel_ages[RIGHT])
            {
                if (time_since_h_report++ > interval)
                {
                    report = true;
                    time_since_h_report = 0;
                    *horizontal = state.wheel_ages[UP] ? step : -step;
                }
            }
            else
            {
                time_since_h_report = 0;
            }
        }
        break;
        default:
        break;
    }
    return report;
}
