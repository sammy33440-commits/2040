#include "tusb.h"
#include "usb/usbd/usbd_mode.h"
#include "usb/usbd/usbd.h"
#include "usb/descriptors/switch_descriptors.h"
#include "core/buttons.h"
#include <string.h>

static switch_in_report_t switch_report;

static void switch_mode_init(void) {
    memset(&switch_report, 0, sizeof(switch_in_report_t));
    switch_report.lx = switch_report.ly = 128;
    switch_report.rx = switch_report.ry = 128;
    switch_report.hat = 8;
}

static bool switch_mode_send_report(uint8_t player_index, const input_event_t* event, const profile_output_t* profile_out, uint32_t buttons) {
    switch_report.buttons = 0;
    // Mapping Pro Controller
    if (buttons & JP_BUTTON_B1) switch_report.buttons |= SWITCH_MASK_B;
    if (buttons & JP_BUTTON_B2) switch_report.buttons |= SWITCH_MASK_A;
    if (buttons & JP_BUTTON_B3) switch_report.buttons |= SWITCH_MASK_Y;
    if (buttons & JP_BUTTON_B4) switch_report.buttons |= SWITCH_MASK_X;
    if (buttons & JP_BUTTON_L2) switch_report.buttons |= SWITCH_MASK_ZL;
    if (buttons & JP_BUTTON_R2) switch_report.buttons |= SWITCH_MASK_ZR;
    
    switch_report.lx = profile_out->left_x;
    switch_report.ly = profile_out->left_y;
    
    return tud_hid_report(0, &switch_report, sizeof(switch_in_report_t));
}

static const uint8_t* switch_mode_get_device_descriptor(void) { return (const uint8_t*)&switch_device_descriptor; }
static const uint8_t* switch_mode_get_config_descriptor(void) { return switch_config_descriptor; }
static const uint8_t* switch_mode_get_report_descriptor(void) { return switch_report_descriptor; }

const usbd_mode_t switch_mode = {
    .init = switch_mode_init,
    .is_ready = tud_hid_ready,
    .send_report = switch_mode_send_report,
    .get_device_descriptor = switch_mode_get_device_descriptor,
    .get_config_descriptor = switch_mode_get_config_descriptor,
    .get_report_descriptor = switch_mode_get_report_descriptor,
};
