#include "tusb.h"
#include "usb/descriptors/switch_descriptors.h"
#include "usb/usbd/usbd.h"
#include <string.h>

static switch_in_report_t switch_report;

static void switch_mode_init(void) {
    memset(&switch_report, 0, sizeof(switch_in_report_t));
    switch_report.lx = 128;
    switch_report.ly = 128;
    switch_report.rx = 128;
    switch_report.ry = 128;
    switch_report.hat = 8;
}

static bool switch_mode_is_ready(void) {
    return tud_hid_ready();
}

static bool switch_mode_send_report(uint8_t player_index, const void* event, const void* profile_out, uint32_t buttons) {
    (void)player_index; (void)event; (void)profile_out; (void)buttons;
    return tud_hid_report(0, &switch_report, sizeof(switch_in_report_t));
}

static const uint8_t* switch_mode_get_device_descriptor(void) { return (const uint8_t*)&switch_device_descriptor; }
static const uint8_t* switch_mode_get_config_descriptor(void) { return switch_config_descriptor; }
static const uint8_t* switch_mode_get_report_descriptor(void) { return switch_report_descriptor; }

const usbd_mode_t switch_mode = {
    .init = switch_mode_init,
    .is_ready = switch_mode_is_ready,
    .send_report = switch_mode_send_report,
    .get_device_descriptor = switch_mode_get_device_descriptor,
    .get_config_descriptor = switch_mode_get_config_descriptor,
    .get_report_descriptor = switch_mode_get_report_descriptor,
};
