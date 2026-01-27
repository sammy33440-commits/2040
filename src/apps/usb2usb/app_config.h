#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

// Force l'utilisation des deux interfaces USB
#define ENABLE_USB_HOST
#define ENABLE_USB_DEVICE

// Définit les interfaces d'entrée et de sortie
#define APP_INPUT_INTERFACES  &input_usb
#define APP_OUTPUT_INTERFACES &output_usb

// Configuration du polling (vitesse de lecture)
#define APP_POLL_INTERVAL_MS 1

#endif
