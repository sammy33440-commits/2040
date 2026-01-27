#include "tusb.h"
#include <string.h>

void usbh_init(void) {
    // Initialise le port HOST (Port 1 sur RP2040)
    tusb_init(1, NULL); 
}

void usbh_task(void) {
    tuh_task(); 
}
