// usbh.c - USB Host implementation
// SPDX-License-Identifier: Apache-2.0

#include "tusb.h"
#include "usb/usbd/usbd_mode.h"
#include "usb/usbd/usbd.h"
#include <string.h>

// Initialisation du port USB Host (celui où tu branches ta manette)
void usbh_init(void) {
    tusb_init(1, NULL); 
}

// Tâche de fond pour gérer les périphériques branchés
void usbh_task(void) {
    tuh_task(); 
}
