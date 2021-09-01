#include "key_scan.h"
#include "keys_config.h"
#include "key.h"
#include "keys_config.h"
#include "key_scan.pio.h"
#include "mouse_movement.h"

#include "hardware/dma.h"
#include "hardware/irq.h"

static struct key_scan_state
{
    PIO scan_pio;
    layer_t current_layer;
} state;

static void pio_interrupt_handler(void);
static void update_set_events(void);
static void update_scanned_keys(void);
static void update_interrupted_holds(void);
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
    update_interrupted_holds();
    update_set_events();

    pio_interrupt_clear(state.scan_pio, KEY_SCAN_IRQ);
    irq_set_enabled(pio_irq, true);

    scan_start();
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

        FOREACH_ROW(r) // left rows
        {
            bool value = !!(input & row_gpios_l[r]);
            Key_SetPressed(Keys_GetKey(r, c), value);
        }
        FOREACH_ROW(r) // right nows
        {
            bool value = !!(input & row_gpios_r[r]);
            Key_SetPressed(Keys_GetKey(r, (COLS/2) + c), value);
        }
    }
}

static layer_t get_layer_from_keys(void)
{
    layer_t layer = LAYER_BASE;

    FOREACH_ROW(r)
    {
        FOREACH_COL(c)
        {
            layer = Event_GetLayer(Key_PeekEvent(Keys_GetKey(r, c), Keys_GetKeyConfig(layer, r, c)));
            if (layer != LAYER_BASE)
                break;
        }
    }

    return layer;
}

static bool isNonTapHoldKeyPressed(void)
{
    FOREACH_ROW(r)
    {
        FOREACH_COL(c)
        {
            keyk_t *key = Keys_GetKey(r, c);
            const key_config_t *config = Keys_GetKeyConfig(state.current_layer, r, c);

            if (!KeyConfig_IsTapHold(config))
            {
                if (Key_IsPressed(key))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

/* If a non-tap-hold key is pressed at the same time as a tap-hold key, the
   hold timer is interrupted, and no longer behaves as a hold key. Instead
   the tap-hold key will emit its tap event.
*/
static void update_interrupted_holds(void)
{
    if (isNonTapHoldKeyPressed())
    {
        FOREACH_ROW(r)
        {
            FOREACH_COL(c)
            {
                keyk_t *key = Keys_GetKey(r, c);
                const key_config_t *config = Keys_GetKeyConfig(state.current_layer, r, c);
                
                if (KeyConfig_IsTapHold(config))
                {
                    if (Key_IsPressed(key) && !Key_IsHeld(key))
                    {
                        Key_SetInterrupted(key, config);
                    }
                }
            }
        }
    }
}

static void update_set_events(void)
{
    key_event_t events[8];
    uint event_index = 0;
    key_event_t layer_modifier;

    layer_modifier = Keys_GetLayerModifier(state.current_layer);

    if (layer_modifier != EVENT_NONE)
    {
        events[event_index++] = layer_modifier;
    }

    // Since tap hold keys can be interrupted, their events need to be captured first
    FOREACH_COL(c)
    {
        FOREACH_ROW(r)
        {
            const key_config_t *config = Keys_GetKeyConfig(state.current_layer, r, c);
            if (KeyConfig_IsTapHold(config))
            {
                key_event_t event;
                event = Key_GetEvent(Keys_GetKey(r, c), config);
                if (event)
                {
                    if (event_index < count_of(events))
                    {
                        events[event_index++] = event;
                    }
                }
            }
        }
    }

    FOREACH_COL(c)
    {
        FOREACH_ROW(r)
        {
            const key_config_t *config = Keys_GetKeyConfig(state.current_layer, r, c);
            if (!KeyConfig_IsTapHold(config))
            {
                key_event_t event;
                event = Key_GetEvent(Keys_GetKey(r, c), Keys_GetKeyConfig(state.current_layer, r, c));
                if (event)
                {
                    if (event_index < count_of(events))
                    {
                        events[event_index++] = event;
                    }
                }
            }
        }
    }

    KeyScan_SetEventsCb(events, event_index);
}
