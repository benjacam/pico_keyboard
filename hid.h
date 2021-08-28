#pragma once

#include "key_events.h"

/* Handle key events as a result of a key scan */
void Hid_HandleKeyEvents(key_event_t* set_events, uint n_set);

/* Query for caps lock */
bool Hid_IsCapsLocked(void);

/* Query for scroll lock */
bool Hid_IsScrollLocked(void);

/* Query for num lock */
bool Hid_IsNumLocked(void);

/* Query for shift key (or caps lock) */
bool Hid_IsShifted(void);
