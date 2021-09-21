#include "key_scan.h"
#include "keys_config.h"
#include "key.h"
#include "keys_config.h"
#include "key_scan.pio.h"
#include "mouse_movement.h"

#include "hardware/dma.h"
#include "hardware/irq.h"

#include "stdlib.h"

// Linked list node
typedef struct _keyconfiglist
{
    // Store keys by row/col so key instance and config can be retrieved
    uint8_t row, col;
    struct _keyconfiglist *next;
} key_list_t;

static struct key_scan_state
{
    PIO scan_pio;
    layer_t current_layer;
    /* List of keys, maintained in the order in which they were pressed */
    key_list_t *head;
} state;

static void pio_interrupt_handler(void);
static void update_set_events(void);
static void update_scanned_keys(void);
static layer_t get_layer_from_keys(void);
static void scan_init(void);
static void scan_start(void);

void KeyScan_Init(PIO pio)
{
    state.scan_pio = pio;
    scan_init();
    scan_start();
}

layer_t KeyScan_GetCurrentLayer(void)
{
    return state.current_layer;
}

static void scan_init(void)
{
    uint pio_irq = pio_get_index(state.scan_pio) ? PIO1_IRQ_0 : PIO0_IRQ_0;

    pio_key_scan_init(state.scan_pio, col_mask, row_mask);

    // Configure the processor to run pio_interrupt_handler() when IRQ 0 is asserted
    irq_set_exclusive_handler(pio_irq, pio_interrupt_handler);
    pio_interrupt_clear(state.scan_pio, KEY_SCAN_IRQ);
    pio_set_irq0_source_enabled(state.scan_pio, pis_interrupt0, true);
    irq_set_enabled(pio_irq, true);
}

static void scan_start(void)
{
    for (uint b = 0; b < count_of(col_scan_sequence); b++)
    {
        pio_sm_put(state.scan_pio, SM_WRITE, col_scan_sequence[b]);
    }
}

static void pio_interrupt_handler(void)
{
    uint pio_irq = pio_get_index(state.scan_pio) ? PIO1_IRQ_0 : PIO0_IRQ_0;

    update_scanned_keys();
    state.current_layer = get_layer_from_keys();
    update_set_events();

    pio_interrupt_clear(state.scan_pio, KEY_SCAN_IRQ);
    irq_set_enabled(pio_irq, true);

    scan_start();
}

static void key_list_add(uint8_t row, uint8_t col)
{
    key_list_t *kl_new = NULL;

    kl_new = malloc(sizeof(*kl_new));
    assert(kl_new);
    kl_new->col = col;
    kl_new->row = row;
    kl_new->next = NULL;

    if (!state.head)
    {
        state.head = kl_new;
    }
    else
    {
        for (key_list_t *kl = state.head; kl; kl = kl->next)
        {
            if (kl->row == row && kl->col == col)
            {
                panic("Adding existing key");
            }
            if (!kl->next)
            {
                kl->next = kl_new;
                return;
            }
        }
    }
}

static void key_list_remove(uint8_t row, uint8_t col)
{
    key_list_t *kl;
    key_list_t *kl_prev = NULL;
    for (kl = state.head; kl; kl = kl->next)
    {
        if (kl->col == col && kl->row == row)
        {
            if (kl_prev)
            {
                kl_prev->next = kl->next;
            }
            else
            {
                state.head = kl->next;
            }
            kl->next = NULL;
            free(kl);
            return;
        }
        kl_prev = kl;
    }
}

static bool key_list_was_press_key_just_released(key_list_t *head)
{
    for (key_list_t *kl = head; kl; kl = kl->next)
    {
        keyk_t *k = Keys_GetKey(kl->row, kl->col);
        const key_config_t *c = Keys_GetKeyConfig(state.current_layer, kl->row, kl->col);
        if (KeyConfig_IsPress(c) && Key_WasJustReleased(k))
        {
            return true;
        }
    }
    return false;
}

static void update_scanned_keys(void)
{
    uint rx_level = pio_sm_get_rx_fifo_level(state.scan_pio, SM_READ);
    assert(rx_level == count_of(col_scan_sequence));

    // Read the values for each column scan
    // Each column scan can scan two columns, because the left and right hands
    // have independent PIOs for rows.
    for (uint c = 0; c < count_of(col_scan_sequence); c++)
    {
        uint32_t col_mask = col_scan_sequence[c];
        uint32_t input = pio_sm_get(state.scan_pio, SM_READ);

        assert(input & col_mask);

        // left rows
        FOREACH_ROW(r)
        {
            bool value = !!(input & row_gpios_l[r]);
            keyk_t *key = Keys_GetKey(r, c);
            Key_SetPressed(key, value);
        }
        // right rows
        FOREACH_ROW(r)
        {
            bool value = !!(input & row_gpios_r[r]);
            keyk_t *key = Keys_GetKey(r, (COLS/2) + c);
            Key_SetPressed(key, value);
        }
    }
}

static layer_t get_layer_from_keys(void)
{
    FOREACH_ROW(r)
    {
        FOREACH_COL(c)
        {
            layer_t key_layer = Event_GetLayer(Key_PeekEvent(Keys_GetKey(r, c), Keys_GetKeyConfig(LAYER_BASE, r, c)));
            if (state.current_layer == LAYER_BASE)
            {
                if (key_layer != LAYER_BASE)
                {
                    // The first non base layer is selected
                    return key_layer;
                }
            }
            else
            {
                if (key_layer == state.current_layer)
                {
                    return key_layer;
                }
            }
        }
    }

    // This avoids non-base layer to non-base layer transitions
    return LAYER_BASE;
}

static void update_set_events(void)
{
    key_event_t events[8];
    uint event_index = 0;
    key_event_t layer_modifier;
    bool tap_hold_pressed = false;

    layer_modifier = Keys_GetLayerModifier(state.current_layer);

    if (layer_modifier != EVENT_NONE)
    {
        events[event_index++] = layer_modifier;
    }

    FOREACH_COL(c)
    {
        FOREACH_ROW(r)
        {
            keyk_t *k = Keys_GetKey(r, c);
            if (Key_WasJustPressed(k))
            {
                key_list_add(r, c);
            }
        }
    }

    for (key_list_t *kl = state.head; kl; kl = kl->next)
    {
        keyk_t *key = Keys_GetKey(kl->row, kl->col);
        key_config_t config = *Keys_GetKeyConfig(state.current_layer, kl->row, kl->col);
        key_event_t event;

        if (KeyConfig_IsTapHold(&config) && !Key_IsHeld(key) && Key_IsPressed(key))
        {
            tap_hold_pressed = true;

            // If a key is pressed and released after this tap/hold key was pressed, it skips the tap/hold
            // key into the held state.
            if (KeyConfig_CanSkipToHold(&config) && key_list_was_press_key_just_released(kl->next))
            {
                Key_SetHeld(key, &config);
                tap_hold_pressed = false;
            }
        }

        // If a tap-hold key is pressed, PRESS keys are switched to emit
        // their event on release instead instead of press. This avoids ordering problems
        // for rolling between tap-hold and press keys: if the tap-hold
        // is released first, it will generate its tap event first, the
        // press key will return to standard press behavior and then it
        // will emit its event. If the press key is released first, that
        // will make the tap-hold key skip to its held state (see loop
        // above) so the held event and the press event are emitted at the
        // same time.
        if (tap_hold_pressed)
        {
            if (KeyConfig_IsPress(&config) && Key_IsPressed(key))
            {
                config.mode = KEY_CONFIG_RELEASE;  
            }
        }

        event = Key_GetEvent(key, &config);
        if (event)
        {
            if (event_index < count_of(events))
            {
                events[event_index++] = event;
            }
        }
    }

    FOREACH_COL(c)
    {
        FOREACH_ROW(r)
        {
            keyk_t *k = Keys_GetKey(r, c);
            if (Key_WasJustReleased(k))
            {
                key_list_remove(r, c);
            }
        }
    }

    KeyScan_SetEventsCb(events, event_index);
}
