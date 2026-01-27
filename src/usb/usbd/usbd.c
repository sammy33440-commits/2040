#include "usbd.h"
#include "usbd_mode.h"
#include "usb/descriptors/hid_descriptors.h"
#include "usb/descriptors/xbox_og_descriptors.h"
#include "usb/descriptors/xinput_descriptors.h"
#include "usb/descriptors/switch_descriptors.h"
#include "usb/descriptors/ps3_descriptors.h"
#include "usb/descriptors/psclassic_descriptors.h"
#include "usb/descriptors/ps4_descriptors.h"
#include "usb/descriptors/xbone_descriptors.h"
#include "usb/descriptors/xac_descriptors.h"
#include "usb/descriptors/kbmouse_descriptors.h"
#include "usb/descriptors/gc_adapter_descriptors.h"
#include "kbmouse/kbmouse.h"
#include "drivers/tud_xid.h"
#include "drivers/tud_xinput.h"
#include "drivers/tud_xbone.h"
#include "cdc/cdc.h"
#include "cdc/cdc_commands.h"
#include "core/router/router.h"
#include "core/input_event.h"
#include "core/buttons.h"
#include "core/services/storage/flash.h"
#include "core/services/button/button.h"
#include "core/services/profiles/profile.h"
#ifndef DISABLE_USB_HOST
#include "usb/usbh/hid/devices/vendors/sony/sony_ds4.h"
#endif
#include "tusb.h"
#include "device/usbd_pvt.h"
#include "pico/unique_id.h"
#include "hardware/watchdog.h"
#include "hardware/gpio.h"
#include <string.h>
#include <stdio.h>

static joypad_hid_report_t hid_report;
#define USB_MAX_PLAYERS 4
static input_event_t pending_events[USB_MAX_PLAYERS];
static bool pending_flags[USB_MAX_PLAYERS] = {false};
static char usb_serial_str[13];
static usb_output_mode_t output_mode = USB_OUTPUT_MODE_HID;
static flash_t flash_settings;

const usbd_mode_t* usbd_modes[USB_OUTPUT_MODE_COUNT] = {0};
static const usbd_mode_t* current_mode = NULL;

void usbd_register_modes(void) {
    usbd_modes[USB_OUTPUT_MODE_HID] = &hid_mode;
    usbd_modes[USB_OUTPUT_MODE_SWITCH] = &switch_mode;
    usbd_modes[USB_OUTPUT_MODE_PS3] = &ps3_mode;
    usbd_modes[USB_OUTPUT_MODE_PS4] = &ps4_mode;
    // Ajoutez d'autres modes ici si nécessaire
}

void usbd_init(void) {
    usbd_register_modes();
    flash_init();
    if (flash_load(&flash_settings)) {
        if (flash_settings.usb_output_mode < USB_OUTPUT_MODE_COUNT)
            output_mode = (usb_output_mode_t)flash_settings.usb_output_mode;
    }
    current_mode = usbd_modes[output_mode];
    tusb_init(0, NULL);
}

void usbd_task(void) {
    tud_task();
    if (current_mode && current_mode->is_ready()) {
        // Logique d'envoi de rapport simplifiée
    }
}

bool usbd_send_report(uint8_t player_index) { return true; }
uint8_t const *tud_descriptor_device_cb(void) { return current_mode->get_device_descriptor(); }
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) { return current_mode->get_config_descriptor(); }
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf) { return current_mode->get_report_descriptor(); }
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {}
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) { return 0; }
