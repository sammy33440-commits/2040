#include "tusb.h"
#include "usb/usbd/usbd_mode.h"
#include "usb/usbd/usbd.h"
#include <string.h>

void usbh_init(void) {
    tusb_init(1, NULL); // Mode Host sur le port 1
}

void usbh_task(void) {
    tuh_task();
}
