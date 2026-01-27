#include "tusb.h"
#include "usb/usbd/usbd_mode.h"
#include "usb/usbd/usbd.h"
#include <string.h>

void usbh_init(void) {
    tusb_init(1, NULL); // Initialise le port HOST (Port 1 sur RP2040)
}

void usbh_task(void) {
    tuh_task();
}
