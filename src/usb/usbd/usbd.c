// usbd.c - USB device output
// SPDX-License-Identifier: Apache-2.0
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

// Current HID report (for HID mode)
static joypad_hid_report_t hid_report;

#define USB_MAX_PLAYERS 4
static input_event_t pending_events[USB_MAX_PLAYERS];
static bool pending_flags[USB_MAX_PLAYERS] = {false};

#define USB_SERIAL_LEN 12
static char usb_serial_str[USB_SERIAL_LEN + 1];

static usb_output_mode_t output_mode = USB_OUTPUT_MODE_HID;
static flash_t flash_settings;

static const char* mode_names[] = {
    [USB_OUTPUT_MODE_HID] = "DInput",
    [USB_OUTPUT_MODE_XBOX_ORIGINAL] = "Xbox Original (XID)",
    [USB_OUTPUT_MODE_XINPUT] = "XInput",
    [USB_OUTPUT_MODE_PS3] = "PS3",
    [USB_OUTPUT_MODE_PS4] = "PS4",
    [USB_OUTPUT_MODE_SWITCH] = "Switch",
    [USB_OUTPUT_MODE_PSCLASSIC] = "PS Classic",
    [USB_OUTPUT_MODE_XBONE] = "Xbox One",
    [USB_OUTPUT_MODE_XAC] = "XAC Compat",
    [USB_OUTPUT_MODE_KEYBOARD_MOUSE] = "KB/Mouse",
    [USB_OUTPUT_MODE_GC_ADAPTER] = "GC Adapter",
};

const usbd_mode_t* usbd_modes[USB_OUTPUT_MODE_COUNT] = {0};
static const usbd_mode_t* current_mode = NULL;

void usbd_register_modes(void)
{
    usbd_modes[USB_OUTPUT_MODE_HID] = &hid_mode;
#if CFG_TUD_XINPUT
    usbd_modes[USB_OUTPUT_MODE_XINPUT] = &xinput_mode;
#endif
    usbd_modes[USB_OUTPUT_MODE_SWITCH] = &switch_mode;
    usbd_modes[USB_OUTPUT_MODE_PS3] = &ps3_mode;
    usbd_modes[USB_OUTPUT_MODE_PSCLASSIC] = &psclassic_mode;
    usbd_modes[USB_OUTPUT_MODE_PS4] = &ps4_mode;
    usbd_modes[USB_OUTPUT_MODE_XBOX_ORIGINAL] = &xid_mode;
    usbd_modes[USB_OUTPUT_MODE_XBONE] = &xbone_mode;
    usbd_modes[USB_OUTPUT_MODE_XAC] = &xac_mode;
    usbd_modes[USB_OUTPUT_MODE_KEYBOARD_MOUSE] = &kbmouse_mode;
#if CFG_TUD_GC_ADAPTER
    usbd_modes[USB_OUTPUT_MODE_GC_ADAPTER] = &gc_adapter_mode;
#endif
}

const usbd_mode_t* usbd_get_current_mode(void) { return current_mode; }

static uint32_t apply_usbd_profile(const input_event_t* event, profile_output_t* profile_out)
{
    const profile_t* profile = profile_get_active(OUTPUT_TARGET_USB_DEVICE);
    profile_apply(profile, event->buttons, event->analog[ANALOG_LX], event->analog[ANALOG_LY],
                  event->analog[ANALOG_RX], event->analog[ANALOG_RY],
                  event->analog[ANALOG_L2], event->analog[ANALOG_R2], profile_out);
    uint8_t output_axes[6] = { profile_out->left_x, profile_out->left_y, profile_out->right_x, profile_out->right_y, profile_out->l2_analog, profile_out->r2_analog };
    cdc_commands_send_output_event(profile_out->buttons, output_axes);
    return profile_out->buttons;
}

static uint8_t convert_dpad_to_hat(uint32_t buttons)
{
    uint8_t up = (buttons & JP_BUTTON_DU) ? 1 : 0;
    uint8_t down = (buttons & JP_BUTTON_DD) ? 1 : 0;
    uint8_t left = (buttons & JP_BUTTON_DL) ? 1 : 0;
    uint8_t right = (buttons & JP_BUTTON_DR) ? 1 : 0;
    if (up && right) return HID_HAT_UP_RIGHT;
    if (up && left) return HID_HAT_UP_LEFT;
    if (down && right) return HID_HAT_DOWN_RIGHT;
    if (down && left) return HID_HAT_DOWN_LEFT;
    if (up) return HID_HAT_UP;
    if (down) return HID_HAT_DOWN;
    if (left) return HID_HAT_LEFT;
    if (right) return HID_HAT_RIGHT;
    return HID_HAT_CENTER;
}

usb_output_mode_t usbd_get_mode(void) { return output_mode; }

bool usbd_set_mode(usb_output_mode_t mode)
{
    if (mode >= USB_OUTPUT_MODE_COUNT) return false;
    flash_settings.usb_output_mode = (uint8_t)mode;
    flash_save_now(&flash_settings);
    watchdog_enable(100, false);
    while(1);
    return true;
}

static void usbd_on_input(output_target_t output, uint8_t player_index, const input_event_t* event)
{
    if (player_index >= USB_MAX_PLAYERS || !event) return;
    pending_events[player_index] = *event;
    pending_flags[player_index] = true;
}

void usbd_init(void)
{
    usbd_register_modes();
    flash_init();
    if (flash_load(&flash_settings)) {
        if (flash_settings.usb_output_mode < USB_OUTPUT_MODE_COUNT) output_mode = (usb_output_mode_t)flash_settings.usb_output_mode;
    }
    char full_id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
    pico_get_unique_board_id_string(full_id, sizeof(full_id));
    memcpy(usb_serial_str, full_id, USB_SERIAL_LEN);
    usb_serial_str[USB_SERIAL_LEN] = '\0';
    tusb_rhport_init_t dev_init = { .role = TUSB_ROLE_DEVICE, .speed = TUSB_SPEED_AUTO };
    tusb_init(0, &dev_init);
    if (usbd_modes[output_mode] && usbd_modes[output_mode]->init) usbd_modes[output_mode]->init();
    current_mode = usbd_modes[output_mode];
    router_set_tap(OUTPUT_TARGET_USB_DEVICE, usbd_on_input);
}

void usbd_task(void)
{
    tud_task();
    if (current_mode && current_mode->is_ready && current_mode->is_ready()) usbd_send_report(0);
}

bool usbd_send_report(uint8_t player_index)
{
    if (!current_mode || !current_mode->send_report || !pending_flags[player_index]) return false;
    const input_event_t* event = &pending_events[player_index];
    pending_flags[player_index] = false;
    profile_output_t profile_out;
    uint32_t processed_buttons = apply_usbd_profile(event, &profile_out);
    return current_mode->send_report(player_index, event, &profile_out, processed_buttons);
}

// TINYUSB CALLBACKS
uint8_t const *tud_descriptor_device_cb(void) { return current_mode->get_device_descriptor(); }
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) { (void)index; return current_mode->get_config_descriptor(); }
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf) { (void)itf; return current_mode->get_report_descriptor(); }

uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) { return 0; }
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {}

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    static uint16_t _desc_str[32];
    const char *str = NULL;
    if (index == 0) { _desc_str[1] = 0x0409; _desc_str[0] = (TUSB_DESC_STRING << 8) | 4; return _desc_str; }
    if (index == 1) str = SWITCH_MANUFACTURER;
    if (index == 2) str = SWITCH_PRODUCT;
    if (index == 3) str = usb_serial_str;
    if (!str) return NULL;
    uint8_t len = strlen(str);
    for (uint8_t i = 0; i < len; i++) _desc_str[1 + i] = str[i];
    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * len + 2);
    return _desc_str;
}
