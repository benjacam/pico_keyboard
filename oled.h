#pragma once

#include "hardware/pio.h"

#include "stdbool.h"
#include "key_layers.h"
#include "key_events.h"

/* Initialise the oled */
void Oled_Init(PIO pio);

/* Start updating the oled */
void Oled_Start(void);

/* Stop updating the oled */
void Oled_Stop(void);

/* Must be called periodically */
bool Oled_Refresh(void);
