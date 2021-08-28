#include <stdbool.h>
#include "stdlib.h"

#include "key_scan.h"
#include "oled.h"
#include "hid.h"

void KeyScan_SetEventsCb(key_event_t* set_events, uint n_set)
{
    Hid_HandleKeyEvents(set_events, n_set);
    Oled_Refresh();
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
    Oled_Start();
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    Oled_Stop();
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    Oled_Stop();
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    Oled_Start();
}

int main()
{
    //stdio_init_all();
    tusb_init();
    Oled_Init(pio0);
    KeyScan_Init(pio1);

    // Functionality is now interrupt based so main can sleep
    while (true)
    {
        sleep_ms(1000000);
    }
    return 0;
}
