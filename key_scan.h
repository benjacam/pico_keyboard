#pragma once

#include "hardware/pio.h"

#include "key_events.h"

/* Initialise and start the key scanning */
void KeyScan_Init(PIO pio);

/* Query the current layer */
layer_t KeyScan_GetCurrentLayer(void);

/* Called-back by KeyScan after each scan, informing the client
   of any set key events */
void KeyScan_SetEventsCb(key_event_t* set_events, uint n_set);
