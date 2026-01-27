#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

// --- CONFIGURATION PIZERO DUAL USB-C ---
#define CFG_TUSB_MCU                OPT_MCU_RP2040
#define CFG_TUSB_OS                 OPT_OS_NONE

// Port Droit (Natif) = Console/PC
#define CFG_TUSB_RHPORT0_MODE       OPT_MODE_DEVICE
// Port Gauche (PIO) = Manette
#define CFG_TUSB_RHPORT1_MODE       OPT_MODE_HOST
#define CFG_TUH_RPI_PIO_USB         1

// Pins 6 et 7 forcés
#ifndef PICO_USB_HOST_DP_PIN
  #define PICO_USB_HOST_DP_PIN      6
#endif
#ifndef PICO_USB_HOST_DM_PIN
  #define PICO_USB_HOST_DM_PIN      7
#endif

// --- CONFIGURATION HOST (Entrée Manette) ---
#define CFG_TUH_HUB                 1
#define CFG_TUH_HID                 8
#define CFG_TUH_XINPUT              4
#define CFG_TUH_DEVICE_MAX          (4*CFG_TUH_HUB + 1)
#define CFG_TUH_ENUMERATION_BUFSIZE 1280

// --- CONFIGURATION DEVICE (Sortie Console) ---
#define CFG_TUD_HID                 4
#define CFG_TUD_XID                 1
#define CFG_TUD_XINPUT              1
#define CFG_TUD_GC_ADAPTER          1
#define CFG_TUD_CDC                 1
#define CFG_TUD_ENDPOINT0_SIZE      64

#ifdef __cplusplus
 }
#endif

#endif
