/*
 * Joypad - TinyUSB Configuration
 * CONFIGURATION POUR WAVESHARE RP2040-PIZERO
 * Force l'activation USB Host sur GPIO 6 (D+) et GPIO 7 (D-)
 */

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

//====================================================================
// 🔥 ACTIVATION FORCÉE USB HOST - GPIO 6 & 7 🔥
//==================12:40 27/01/2026==================================================

// Active le driver PIO-USB (OBLIGATOIRE pour USB Host)
#define CFG_TUH_RPI_PIO_USB         1

// Définit les pins physiques du port USB secondaire
#undef PICO_USB_HOST_DP_PIN
#undef PICO_USB_HOST_DM_PIN
#define PICO_USB_HOST_DP_PIN        6  // D+ (Data Plus)
#define PICO_USB_HOST_DM_PIN        7  // D- (Data Minus)

// Force le mode Dual-Role :
// - RHPORT0 (port droit) = Device mode (vers PC/Console)
// - RHPORT1 (port gauche) = Host mode (lit la manette)
#define CFG_TUSB_RHPORT0_MODE       OPT_MODE_DEVICE
#define CFG_TUSB_RHPORT1_MODE       OPT_MODE_HOST

//====================================================================

#ifndef CFG_TUSB_MCU
  #error CFG_TUSB_MCU must be defined
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS                  OPT_OS_NONE
#endif

// Debug activé pour diagnostic
#undef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG           1

/* USB DMA memory configuration */
#ifndef CFG_TUD_MEM_SECTION
#define CFG_TUD_MEM_SECTION
#endif

#ifndef CFG_TUD_MEM_ALIGN
#define CFG_TUD_MEM_ALIGN          __attribute__ ((aligned(4)))
#endif

//--------------------------------------------------------------------
// USB HOST CONFIGURATION
//--------------------------------------------------------------------

#define CFG_TUH_ENUMERATION_BUFSIZE 1280

#ifndef CFG_TUH_MEM_SECTION
#define CFG_TUH_MEM_SECTION
#endif

#ifndef CFG_TUH_MEM_ALIGN
#define CFG_TUH_MEM_ALIGN        __attribute__ ((aligned(4)))
#endif

#define CFG_TUH_HUB                 1
#define CFG_TUH_CDC                 0
#define CFG_TUH_HID                 8   // Max 8 HID interfaces
#define CFG_TUH_MSC                 0
#define CFG_TUH_VENDOR              0
#define CFG_TUH_XINPUT              4   // Max 4 XInput interfaces

// Bluetooth support
#ifdef ENABLE_BTSTACK
#define CFG_TUH_BTD                 1
#else
#define CFG_TUH_BTD                 0
#endif

#define CFG_TUH_DEVICE_MAX          (4*CFG_TUH_HUB + 1)
#define CFG_TUH_API_EDPT_XFER       1

#define CFG_TUH_HID_EPIN_BUFSIZE    64
#define CFG_TUH_HID_EPOUT_BUFSIZE   64

//--------------------------------------------------------------------
// USB DEVICE CONFIGURATION
//--------------------------------------------------------------------

#define CFG_TUD_ENDPOINT0_SIZE    64
#define CFG_TUD_HID               4   // Up to 4 HID gamepads
#define CFG_TUD_XID               1   // Xbox Original
#define CFG_TUD_XID_EP_BUFSIZE    32
#define CFG_TUD_XINPUT            1   // Xbox 360
#define CFG_TUD_XINPUT_EP_BUFSIZE 32
#define CFG_TUD_GC_ADAPTER        1   // GameCube Adapter
#define CFG_TUD_GC_ADAPTER_EP_BUFSIZE 37

#ifndef USBR_CDC_DEBUG
#define USBR_CDC_DEBUG            1
#endif
#define CFG_TUD_CDC               (1 + USBR_CDC_DEBUG)

#define CFG_TUD_MSC               0
#define CFG_TUD_MIDI              0
#define CFG_TUD_VENDOR            0

#define CFG_TUD_HID_EP_BUFSIZE    64
#define CFG_TUD_CDC_RX_BUFSIZE    256
#define CFG_TUD_CDC_TX_BUFSIZE    1024
#define CFG_TUD_CDC_EP_BUFSIZE    64

#ifdef __cplusplus
 }
#endif

#endif /* _TUSB_CONFIG_H_ */
