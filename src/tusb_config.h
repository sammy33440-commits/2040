/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

// defined by compiler flags for flexibility
#ifndef CFG_TUSB_MCU
  #error CFG_TUSB_MCU must be defined
#endif

// USB role configuration
#if defined(DISABLE_USB_HOST)
  // Device-only mode (e.g., snes2usb - no USB host needed)
  #define CFG_TUSB_RHPORT0_MODE       OPT_MODE_DEVICE
#elif defined(CONFIG_USB)
  // Dual-role USB configuration (host + device)
  // Device mode on RHPORT0 (native USB), Host mode on RHPORT1 (PIO USB)
  #define CFG_TUSB_RHPORT0_MODE       OPT_MODE_DEVICE
  #define CFG_TUSB_RHPORT1_MODE       OPT_MODE_HOST
  #define CFG_TUH_RPI_PIO_USB         1  // Enable PIO USB host driver

  // --- REDIRECTION POUR WAVESHARE PIZERO (GPIO 6/7) ---
  #ifdef PICO_USB_HOST_DP_PIN
    #undef PICO_USB_HOST_DP_PIN
  #endif
  #ifdef PICO_USB_HOST_DM_PIN
    #undef PICO_USB_HOST_DM_PIN
  #endif
  #define PICO_USB_HOST_DP_PIN        6
  #define PICO_USB_HOST_DM_PIN        7
  // ----------------------------------------------------

#else
  // Host-only mode for existing console implementations
  #if CFG_TUSB_MCU == OPT_MCU_LPC43XX || CFG_TUSB_MCU == OPT_MCU_LPC18XX || CFG_TUSB_MCU == OPT_MCU_MIMXRT10XX
    #define CFG_TUSB_RHPORT0_MODE       (OPT_MODE_HOST | OPT_MODE_HIGH_SPEED)
  #else
    #define CFG_TUSB_RHPORT0_MODE       OPT_MODE_HOST
  #endif
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS                  OPT_OS_NONE
#endif

// CFG_TUSB_DEBUG is defined by compiler in DEBUG build
#undef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG               1  // Enable debug logging

/* USB DMA macros */
#ifndef CFG_TUD_MEM_SECTION
#define CFG_TUD_MEM_SECTION
#endif

#ifndef CFG_TUD_MEM_ALIGN
#define CFG_TUD_MEM_ALIGN           __attribute__ ((aligned(4)))
#endif

//--------------------------------------------------------------------
// CONFIGURATION
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

// Bluetooth dongle support
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

#if defined(CONFIG_USB) || defined(DISABLE_USB_HOST)
  #define CFG_TUD_ENDPOINT0_SIZE    64
  #define CFG_TUD_HID               4   // Up to 4 HID gamepads
  #define CFG_TUD_XID               1   // Enable XID
  #define CFG_TUD_XID_EP_BUFSIZE    32
  #define CFG_TUD_XINPUT            1   // Enable XInput
  #define CFG_TUD_XINPUT_EP_BUFSIZE 32
  #define CFG_TUD_GC_ADAPTER        1   // Enable GC adapter
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
#endif

#ifdef __cplusplus
 }
#endif

#endif /* _TUSB_CONFIG_H_ */
