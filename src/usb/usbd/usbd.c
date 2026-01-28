#include "usbd.h"
#include "usbd_mode.h"

// On n'inclut que ce qui existe sur ton Git
#include "usb/descriptors/hid_descriptors.h"
#include "usb/descriptors/switch_descriptors.h"

#include "tusb.h"
#include <string.h>

// Déclaration des modes externes
extern const usbd_mode_t hid_mode;
extern const usbd_mode_t switch_mode;

static usb_output_mode_t output_mode = USB_OUTPUT_MODE_SWITCH; 
const usbd_mode_t* usbd_modes[USB_OUTPUT_MODE_COUNT] = {0};
static const usbd_mode_t* current_mode = NULL;

void usbd_register_modes(void) {
    usbd_modes[USB_OUTPUT_MODE_HID] = &hid_mode;
    usbd_modes[USB_OUTPUT_MODE_SWITCH] = &switch_mode;
}

void usbd_init(void) {
    usbd_register_modes();
    current_mode = usbd_modes[output_mode];
    
    tusb_rhport_init_t dev_init = { .role = TUSB_ROLE_DEVICE, .speed = TUSB_SPEED_AUTO };
    tusb_init(0, &dev_init);
    
    if (current_mode && current_mode->init) current_mode->init();
}

void usbd_task(void) {
    tud_task();
    if (current_mode && current_mode->is_ready && current_mode->is_ready()) {
        usbd_send_report(0);
    }
}

bool usbd_send_report(uint8_t player_index) {
    (void)player_index;
    return true; 
}

// Callbacks TinyUSB obligatoires
uint8_t const *tud_descriptor_device_cb(void) { return current_mode->get_device_descriptor(); }
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) { (void)index; return current_mode->get_config_descriptor(); }
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf) { (void)itf; return current_mode->get_report_descriptor(); }
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {}
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) { return 0; }
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) { return NULL; }
