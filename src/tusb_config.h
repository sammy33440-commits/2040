/*
 * The MIT License (MIT)
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 */

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

#ifndef CFG_TUSB_MCU
  #error CFG_TUSB_MCU must be defined
#endif

// USB role configuration
#if defined(DISABLE_USB_HOST)
  #define CFG_TUSB_RHPORT0_MODE       OPT_MODE_DEVICE
#elif defined(CONFIG_USB)
  #define CFG_TUSB_RHPORT0_MODE       OPT_MODE_DEVICE
  #define CFG_TUSB_RHPORT1_MODE       OPT_MODE_HOST
  #define CFG_TUH_RPI_PIO_USB         1  

  // --- MODIFICATION POUR TA CARTE PIZERO ---
  #undef PICO_USB_HOST_DP_PIN
  #undef PICO_USB_HOST_DM_PIN
  #define PICO_USB_HOST_DP_PIN        6
  #define PICO_USB_HOST_DM_PIN        7
  // ------------------------------------------
#else
  #define CFG_TUSB_RHPORT0_MODE       OPT_MODE_HOST
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS                  OPT_OS_NONE
#endif

#define CFG_TUSB_DEBUG               1  

#ifndef CFG_TUD_MEM_SECTION
#define CFG_TUD_MEM_SECTION
#endif

#ifndef CFG_TUD_MEM_ALIGN
#define CFG_TUD_MEM_ALIGN           __attribute__ ((aligned(4)))
#endif

#define CFG_TUH_ENUMERATION_BUFSIZE 1280

#ifndef CFG_TUH_MEM_SECTION
#define CFG_TUH_MEM_SECTION
#endif

#ifndef CFG_TUH_MEM_ALIGN
#define CFG_TUH_MEM_ALIGN        __attribute__ ((aligned(4)))
#endif

#define CFG_TUH_HUB                 1
#define CFG_TUH_HID                 8 
#define CFG_TUH_XINPUT              4 
#define CFG_TUH_DEVICE_MAX          (4*CFG_TUH_HUB + 1)
#define CFG_TUH_API_EDPT_XFER       1
#define CFG_TUH_HID_EPIN_BUFSIZE    64
#define CFG_TUH_HID_EPOUT_BUFSIZE   64

#if defined(CONFIG_USB) || defined(DISABLE_USB_HOST)
  #define CFG_TUD_ENDPOINT0_SIZE    64
  #define CFG_TUD_HID               4 
  #define CFG_TUD_XID               1 
  #define CFG_TUD_XINPUT            1 
  #define CFG_TUD_GC_ADAPTER        1 
  #define CFG_TUD_CDC               2
  #define CFG_TUD_HID_EP_BUFSIZE    64
#endif

#ifdef __cplusplus
 }
#endif

#endif
