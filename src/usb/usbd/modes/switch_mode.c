// switch_mode.c - Nintendo Switch USB device mode (PRO CONTROLLER FIX)
// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Robert Dale Smith

#include "tusb.h"
#include "switch_mode.h"
#include "usb/usbd/usbd.h"
#include "usb/descriptors/switch_descriptors.h"
#include "core/buttons.h"
#include <string.h>

#ifdef HID_KEY_SPACE
#undef HID_KEY_SPACE
#endif

// ============================================================================
// STATE
// ============================================================================

static switch_in_report_t switch_report;

// ============================================================================
// CONVERSION HELPERS
// ============================================================================

static uint8_t convert_dpad_to_hat(uint32_t buttons)
{
    uint8_t up = (buttons & JP_BUTTON_DU) ? 1 : 0;
    uint8_t down = (buttons & JP_BUTTON_DD) ? 1 : 0;
    uint8_t left = (buttons & JP_BUTTON_DL) ? 1 : 0;
    uint8_t right = (buttons & JP_BUTTON_DR) ? 1 : 0;

    if (up && right) return SWITCH_HAT_UP_RIGHT;
    if (up && left) return SWITCH_HAT_UP_LEFT;
    if (down && right) return SWITCH_HAT_DOWN_RIGHT;
    if (down && left) return SWITCH_HAT_DOWN_LEFT;
    if (up) return SWITCH_HAT_UP;
    if (down) return SWITCH_HAT_DOWN;
    if (left) return SWITCH_HAT_LEFT;
    if (right) return SWITCH_HAT_RIGHT;

    return SWITCH_HAT_CENTER;
}

// ============================================================================
// MODE INTERFACE IMPLEMENTATION
// ============================================================================

static void switch_mode_init(void)
{
    memset(&switch_report, 0, sizeof(switch_in_report_t));
    switch_report.hat = SWITCH_HAT_CENTER;
    switch_report.lx = SWITCH_JOYSTICK_MID;
    switch_report.ly = SWITCH_JOYSTICK_MID;
    switch_report.rx = SWITCH_JOYSTICK_MID;
    switch_report.ry = SWITCH_JOYSTICK_MID;
    switch_report.vendor = 0;
}

static bool switch_mode_is_ready(void)
{
    return tud_hid_ready();
}

static bool switch_mode_send_report(uint8_t player_index,
                                     const input_event_t* event,
                                     const profile_output_t* profile_out,
                                     uint32_t buttons)
{
    (void)player_index;
    (void)event;

    // Mapping des 16 bits de boutons
    switch_report.buttons = 0;
    if (buttons & JP_BUTTON_B1) switch_report.buttons |= SWITCH_MASK_B;
    if (buttons & JP_BUTTON_B2) switch_report.buttons |= SWITCH_MASK_A;
    if (buttons & JP_BUTTON_B3) switch_report.buttons |= SWITCH_MASK_Y;
    if (buttons & JP_BUTTON_B4) switch_report.buttons |= SWITCH_MASK_X;
    
    if (buttons & JP_BUTTON_L1) switch_report.buttons |= SWITCH_MASK_L;
    if (buttons & JP_BUTTON_R1) switch_report.buttons |= SWITCH_MASK_R;
    if (buttons & JP_BUTTON_L2) switch_report.buttons |= SWITCH_MASK_ZL;
    if (buttons & JP_BUTTON_R2) switch_report.buttons |= SWITCH_MASK_ZR;
    
    if (buttons & JP_BUTTON_S1) switch_report.buttons |= SWITCH_MASK_MINUS;
    if (buttons & JP_BUTTON_S2) switch_report.buttons |= SWITCH_MASK_PLUS;
    if (buttons & JP_BUTTON_L3) switch_report.buttons |= SWITCH_MASK_L3;
    if (buttons & JP_BUTTON_R3) switch_report.buttons |= SWITCH_MASK_R3;
    
    if (buttons & JP_BUTTON_A1) switch_report.buttons |= SWITCH_MASK_HOME;
    if (buttons & JP_BUTTON_A2) switch_report.buttons |= SWITCH_MASK_CAPTURE;

    switch_report.hat = convert_dpad_to_hat(buttons);

    switch_report.lx = profile_out->left_x;
    switch_report.ly = profile_out->left_y;
    switch_report.rx = profile_out->right_x;
    switch_report.ry = profile_out->right_y;
    
    switch_report.vendor = 0;

    // ON ENVOIE EXACTEMENT LA TAILLE DU DESCRIPTEUR
    return tud_hid_report(0, &switch_report, sizeof(switch_in_report_t));
}

static const uint8_t* switch_mode_get_device_descriptor(void)
{
    return (const uint8_t*)&switch_device_descriptor;
}

static const uint8_t* switch_mode_get_config_descriptor(void)
{
    return switch_config_descriptor;
}

static const uint8_t* switch_mode_get_report_descriptor(void)
{
    return switch_report_descriptor;
}

// ============================================================================
// MODE EXPORT
// ============================================================================

const usbd_mode_t switch_mode = {
    .name = "Switch",
    .mode = USB_OUTPUT_MODE_SWITCH,

    .get_device_descriptor = switch_mode_get_device_descriptor,
    .get_config_descriptor = switch_mode_get_config_descriptor,
    .get_report_descriptor = switch_mode_get_report_descriptor,

    .init = switch_mode_init,
    .send_report = switch_mode_send_report,
    .is_ready = switch_mode_is_ready,

    .handle_output = NULL,
    .get_rumble = NULL,
    .get_feedback = NULL,
    .get_report = NULL,
    .get_class_driver = NULL,
    .task = NULL,
};
