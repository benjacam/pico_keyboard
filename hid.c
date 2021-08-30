#include <stdio.h>
#include <stdbool.h>
#include "tusb.h"
#include "stdlib.h"
#include "usb_descriptors.h"

#include "key_events.h"
#include "mouse_movement.h"

typedef struct pico_hid_report
{
    report_id_t rid;
    union //anon
    {
        hid_keyboard_report_t kr;
        hid_mouse_report_t mr;
    };
} pico_hid_report_t;

#define QUEUE_SIZE 16

typedef struct hid_queue
{
    uint index;
    uint outdex;
    pico_hid_report_t q[QUEUE_SIZE];
} hid_queue_t;

// A queue of hid reports, full if index-outdex=1, empty if index == outdex
static hid_queue_t queue;

typedef struct hid_state
{
    unsigned caps_lock : 1;
    unsigned scroll_lock : 1;
    unsigned num_lock : 1;
    unsigned shifted : 1;
} hid_state_t;

static hid_state_t hid_state;

static void hid_queue_kick(void);

static inline uint xdex_next(uint xdex)
{
    return (xdex + 1) % QUEUE_SIZE;
}

static inline bool hid_queue_is_empty(void)
{
    return queue.index == queue.outdex;
}

static inline bool hid_queue_is_full(void)
{
    return xdex_next(queue.index) == queue.outdex;
}

static pico_hid_report_t *hid_queue_get(report_id_t id)
{
    pico_hid_report_t *item = NULL;
    if (!hid_queue_is_full())
    {
        item = &queue.q[queue.index];
        memset(item, 0, sizeof(*item));
        item->rid = id;
    }
    return item;
}

/* Queue an item, assumes hid_queue_get already returned a non-NULL item  */
static void hid_queue_enqueue(void)
{
    queue.index = xdex_next(queue.index);
}

static report_id_t hid_queue_head_id(void)
{
    return queue.q[queue.outdex].rid;
}

static pico_hid_report_t *hid_queue_dequeue(void)
{
    pico_hid_report_t *item = NULL;
    if (!hid_queue_is_empty())
    {
        item = &queue.q[queue.outdex];
        queue.outdex = xdex_next(queue.outdex);
    }
    return item;
}

void Hid_HandleKeyEvents(key_event_t* set_events, uint n_set)
{
    pico_hid_report_t *hr;
    static hid_keyboard_report_t kr_last = {0};
    static hid_mouse_report_t mr_last = {0};

    uint key_index = 0;
    uint8_t mouse_buttons = 0;

    hr = hid_queue_get(REPORT_ID_KEYBOARD);
    if (!hr)return; //assert(hr);

    for (uint e = 0; e < n_set; e++)
    {
        key_event_t event = set_events[e];
        if (Event_IsKey(event))
        {
            if (key_index < count_of(hr->kr.keycode))
                hr->kr.keycode[key_index++] = Event_GetHidKey(event);
        }
        else if (Event_IsKeyModifier(event))
        {
            hr->kr.modifier |= Event_GetHidKeyModifier(event);
        }
        else if (Event_IsMouseButton(event))
        {
            mouse_buttons |= Event_GetHidMouseButton(event);
        }
    }

    MouseMovement_HandleKeyEvent(set_events, n_set);

    if (memcmp(hr->kr.keycode, kr_last.keycode, sizeof(hr->kr.keycode)) || hr->kr.modifier != kr_last.modifier)
    {
        hid_queue_enqueue();
        kr_last = hr->kr;
    }

    hr = hid_queue_get(REPORT_ID_MOUSE);
    assert(hr);

    if (mouse_buttons != mr_last.buttons ||
        MouseMovement_GetMouseDeltas(&hr->mr.x, &hr->mr.y) ||
        MouseMovement_GetWheelDeltas(&hr->mr.wheel, &hr->mr.pan))
    {
        hr->mr.buttons = mouse_buttons;
        hid_queue_enqueue();
        mr_last = hr->mr;
    }

    /* This callback is called after each key scan, and the key scan is never
       disabled. This is a suitable moment to run the tud task, before and after we
       attempt to send another report */ 
    tud_task();
    hid_queue_kick();
    tud_task();

    if (kr_last.modifier & (KEYBOARD_MODIFIER_LEFTSHIFT|KEYBOARD_MODIFIER_RIGHTSHIFT))
    {
        hid_state.shifted = true;
    }
    else
    {
        hid_state.shifted = false;
    }
}

static void hid_queue_kick(void)
{
    if (!hid_queue_is_empty())
    {
        if (!tud_connected() || !tud_mounted())
        {
            hid_queue_dequeue();
            return;  
        }

        // Remote wakeup
        if (tud_suspended())
        {
            // Wake up host if we are in suspend mode
            // and REMOTE_WAKEUP feature is enabled by host
            tud_remote_wakeup();
        }

        switch (hid_queue_head_id())
        {
            case REPORT_ID_KEYBOARD:
                if (tud_hid_n_ready(0))
                {
                    pico_hid_report_t *report = hid_queue_dequeue();
                    tud_hid_n_report(0, report->rid, &report->kr, sizeof(report->kr));
                }
            break;

            case REPORT_ID_MOUSE:
                if (tud_hid_n_ready(0))
                {
                    pico_hid_report_t *report = hid_queue_dequeue();
                    tud_hid_n_report(0, report->rid, &report->mr, sizeof(report->mr));
                }
            break;

            default:
                assert(0);
                break;
        } 
    }
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t itf, uint8_t const* report, uint8_t len)
{
    (void) itf;
    (void) len;
    (void) report;

    hid_queue_kick();
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    // TODO set LED based on CAPLOCK, NUMLOCK etc...
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;

    if (itf == 0)
    {
        if (report_id == REPORT_ID_KEYBOARD)
        {
            if (report_type == HID_REPORT_TYPE_OUTPUT)
            {
                hid_state.num_lock =    !!(buffer[1] & 1);
                hid_state.caps_lock =   !!(buffer[1] & 2);
                hid_state.scroll_lock = !!(buffer[1] & 4);
            }
        }
    }
}

bool Hid_IsCapsLocked(void)
{
    return hid_state.caps_lock;
}

bool Hid_IsScrollLocked(void)
{
    return hid_state.scroll_lock;
}

bool Hid_IsNumLocked(void)
{
    return hid_state.num_lock;
}

bool Hid_IsShifted(void)
{
    return hid_state.shifted;
}
