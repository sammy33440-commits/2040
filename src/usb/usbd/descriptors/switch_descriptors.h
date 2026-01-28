#ifndef SWITCH_DESCRIPTORS_H
#define SWITCH_DESCRIPTORS_H

#include <stdint.h>
#include "tusb.h"

// VID/PID pour HORIPAD (Reconnu comme Switch Pro filaire avec ZL/ZR)
#define SWITCH_VID              0x0F0D
#define SWITCH_PID              0x00C1
#define SWITCH_BCD_DEVICE       0x0100

// Mapping des boutons (Standard Switch)
#define SWITCH_MASK_Y       (1U <<  0)
#define SWITCH_MASK_B       (1U <<  1)
#define SWITCH_MASK_A       (1U <<  2)
#define SWITCH_MASK_X       (1U <<  3)
#define SWITCH_MASK_L       (1U <<  4)
#define SWITCH_MASK_R       (1U <<  5)
#define SWITCH_MASK_ZL      (1U <<  6) // Gâchette Gauche
#define SWITCH_MASK_ZR      (1U <<  7) // Gâchette Droite
#define SWITCH_MASK_MINUS   (1U <<  8)
#define SWITCH_MASK_PLUS    (1U <<  9)
#define SWITCH_MASK_L3      (1U << 10)
#define SWITCH_MASK_R3      (1U << 11)
#define SWITCH_MASK_HOME    (1U << 12)
#define SWITCH_MASK_CAPTURE (1U << 13)

#define SWITCH_HAT_UP         0x00
#define SWITCH_HAT_UP_RIGHT   0x01
#define SWITCH_HAT_RIGHT      0x02
#define SWITCH_HAT_DOWN_RIGHT 0x03
#define SWITCH_HAT_DOWN       0x04
#define SWITCH_HAT_DOWN_LEFT  0x05
#define SWITCH_HAT_LEFT       0x06
#define SWITCH_HAT_UP_LEFT    0x07
#define SWITCH_HAT_CENTER     0x08

#define SWITCH_JOYSTICK_MIN   0x00
#define SWITCH_JOYSTICK_MID   0x80
#define SWITCH_JOYSTICK_MAX   0xFF

// Structure du rapport (identique au Pokken mais interprété différemment par la console grâce au PID)
typedef struct __attribute__((packed)) {
    uint16_t buttons;
    uint8_t  hat;
    uint8_t  lx;
    uint8_t  ly;
    uint8_t  rx;
    uint8_t  ry;
    uint8_t  vendor;
} switch_in_report_t;

// Descripteur HID Standard Switch
static const uint8_t switch_report_descriptor[] = {
    0x05, 0x01, 0x09, 0x05, 0xa1, 0x01, 0x15, 0x00, 0x25, 0x01, 0x35, 0x00, 0x45, 0x01, 0x75, 0x01, 0x95, 0x0e,
    0x05, 0x09, 0x19, 0x01, 0x29, 0x0e, 0x81, 0x02, 0x95, 0x02, 0x81, 0x01, 0x05, 0x01, 0x25, 0x07, 0x46, 0x3b,
    0x01, 0x75, 0x04, 0x95, 0x01, 0x65, 0x14, 0x09, 0x39, 0x81, 0x42, 0x65, 0x00, 0x95, 0x01, 0x81, 0x01, 0x26,
    0xff, 0x00, 0x46, 0xff, 0x00, 0x09, 0x30, 0x09, 0x31, 0x09, 0x32, 0x09, 0x35, 0x75, 0x08, 0x95, 0x04, 0x81,
    0x02, 0x06, 0x00, 0xff, 0x09, 0x20, 0x95, 0x01, 0x81, 0x02, 0xc0
};

static const tusb_desc_device_t switch_device_descriptor = {
    .bLength = 18, .bDescriptorType = 1, .bcdUSB = 0x0200, .idVendor = SWITCH_VID, .idProduct = SWITCH_PID,
    .bcdDevice = 0x0100, .iManufacturer = 1, .iProduct = 2, .iSerialNumber = 3, .bNumConfigurations = 1
};

#define SWITCH_CONFIG_TOTAL_LEN (9 + 9 + 9 + 7)
static const uint8_t switch_config_descriptor[] = {
    0x09, 0x02, 0x22, 0x00, 0x01, 0x01, 0x00, 0x80, 0xFA,
    0x09, 0x04, 0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00,
    0x09, 0x21, 0x11, 0x01, 0x00, 0x01, 0x22, 0x56, 0x00,
    0x07, 0x05, 0x81, 0x03, 0x40, 0x00, 0x01
};

#define SWITCH_MANUFACTURER "HORI CO.,LTD."
#define SWITCH_PRODUCT "HORIPAD S"

#endif
