#pragma once

#include "key_events.h"

/* Handle a mouse movement key press */
void MouseMovement_HandleKeyEvent(key_event_t* set_events, uint n_set);

/* Get the mouse movement deltas to report */
bool MouseMovement_GetMouseDeltas(int8_t *x, int8_t *y);

/* Get the wheel movement deltas to report */
bool MouseMovement_GetWheelDeltas(int8_t *vertical, int8_t *horizontal);
