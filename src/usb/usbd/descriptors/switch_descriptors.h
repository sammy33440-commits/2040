// switch_descriptors.h - Nintendo Switch USB HID descriptors
// VERSION FINALE : Mode Pro Controller (DÉBLOQUE ZL/ZR et les STICKS)
// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Robert Dale Smith

#ifndef SWITCH_DESCRIPTORS_H
#define SWITCH_DESCRIPTORS_H

#include <stdint.h>
#include "tusb.h"

// ============================================================================
// SWITCH USB IDENTIFIERS (Passage en mode Officiel Pro Controller)
// ============================================================================

#define SWITCH_VID              0x057E  // Nintendo
#define SWITCH_PID              0x2009  // Pro Controller
#define SWITCH_BCD_DEVICE       0x0210  // Version du firmware émulé

// ============================================================================
// SWITCH BUTTON DEFINITIONS
// ============================================================================

#define SWITCH_MASK_Y       (1U <<  0)
#define SWITCH_MASK_B       (1U <<  1)
#define SWITCH_MASK_A       (1U <<  2)
#define SWITCH_MASK_X       (1U <<  3)
#define SWITCH_MASK_L       (1U <<  4)
#define SWITCH_MASK_R       (1U <<  5)
#define SWITCH_MASK_ZL      (1U <<  6)
#define SWITCH_MASK_ZR      (1U <<  7)
#define SWITCH_MASK_MINUS   (1U <<  8)
#define SWITCH_MASK_PLUS    (1U <<  9)
#define SWITCH_MASK_L3      (1U << 10)
#define SWITCH_MASK_R3      (1U << 11)
#define SWITCH_MASK_HOME    (1U << 12)
#define SWITCH_MASK_CAPTURE (1U << 13)

// D-pad / Hat switch values
#define SWITCH_HAT_UP         0x00
#define SWITCH_HAT_UP_RIGHT   0x01
#define SWITCH_HAT_RIGHT      0x02
#define SWITCH_HAT_DOWN_RIGHT 0x03
#define SWITCH_HAT_DOWN       0x04
#define SWITCH_HAT_DOWN_LEFT  0x05
#define SWITCH_HAT_LEFT       0x06
#define SWITCH_HAT_UP_LEFT    0x07
#define SWITCH_HAT_CENTER     0x08

// Analog stick range
#define SWITCH_JOYSTICK_MIN  0x00
#define SWITCH_JOYSTICK_MID  0x80
#define SWITCH_JOYSTICK_MAX  0xFF

// ============================================================================
// SWITCH REPORT STRUCTURES
// ============================================================================

// Input Report (gamepad state) - 8 bytes
typedef struct __attribute__((packed)) {
    uint16_t buttons;    // 16 button bits (ZL/ZR inclus)
    uint8_t  hat;        // D-pad (hat switch, 0-8)
    uint8_t  lx;         // Left stick X
    uint8_t  ly;         // Left stick Y
    uint8_t  rx;         // Right stick X
    uint8_t  ry;         // Right stick Y
    uint8_t  vendor;     // Byte réservé
} switch_in_report_t;

_Static_assert(sizeof(switch_in_report_t) == 8, "switch_in_report_t must be 8 bytes");

// Output Report (rumble) - 8 bytes
typedef struct __attribute__((packed)) {
    uint8_t  data[8];
} switch_out_report_t;

_Static_assert(sizeof(switch_out_report_t) == 8, "switch_out_report_t must be 8 bytes");

// ============================================================================
// SWITCH USB DESCRIPTORS (PLAN DE LA MANETTE)
// ============================================================================

static const uint8_t switch_report_descriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x05,        // Usage (Game Pad)
    0xA1, 0x01,        // Collection (Application)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x35, 0x00,        //   Physical Minimum (0)
    0x45, 0x01,        //   Physical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x10,        //   Report Count (16) - DÉBLOQUE LES 16 BOUTONS (ZL/ZR)
    0x05, 0x09,        //   Usage Page (Button)
    0x19, 0x01,        //   Usage Minimum (Button 1)
    0x29, 0x10,        //   Usage Maximum (Button 16)
    0x81, 0x02,        //   Input (Data,Var,Abs)
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x39,        //   Usage (Hat switch)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x07,        //   Logical Maximum (7)
    0x35, 0x00,        //   Physical Minimum (0)
    0x45, 0x3B, 0x01,  //   Physical Maximum (315)
    0x75, 0x04,        //   Report Size (4)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x42,        //   Input (Data,Var,Abs,Null)
    0x75, 0x04,        //   Report Size (4)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x01,        //   Input (Const)
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //   Usage (X)
    0x09, 0x31,        //   Usage (Y)
    0x09, 0x32,        //   Usage (Z)
    0x09, 0x35,        //   Usage (Rz)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x04,        //   Report Count (4)
    0x81, 0x02,        //   Input (Data,Var,Abs)
    0xC0               // End Collection
};

// Device descriptor
static const tusb_desc_device_t switch_device_descriptor = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = 64,
    .idVendor           = SWITCH_VID,
    .idProduct          = SWITCH_PID,
    .bcdDevice          = SWITCH_BCD_DEVICE,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01
};

// Configuration descriptor
#define SWITCH_CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_INOUT_DESC_LEN)

static const uint8_t switch_config_descriptor[] = {
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, SWITCH_CONFIG_TOTAL_LEN, 0x80, 250),
    TUD_HID_DESCRIPTOR(0, 0, HID_ITF_PROTOCOL_NONE, sizeof(switch_report_descriptor), 0x81, 64, 1),
    0x07, TUSB_DESC_ENDPOINT, 0x02, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(64), 1,
    0x07, TUSB_DESC_ENDPOINT, 0x81, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(64), 1
};

// String descriptors definitions
#define SWITCH_MANUFACTURER  "Nintendo"
#define SWITCH_PRODUCT       "Pro Controller"

#endif // SWITCH_DESCRIPTORS_H
