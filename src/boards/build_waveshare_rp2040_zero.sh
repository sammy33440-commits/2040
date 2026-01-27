#!/bin/sh
# CONFIGURATION ULTIME WAVESHARE PIZERO USB-C
# Basé sur GPIO 6 (D+) et GPIO 7 (D-)
# Horloge forcée à 120MHz pour la stabilité PIO-USB

cmake -G "Unix Makefiles" \
  -DFAMILY=rp2040 \
  -DPICO_BOARD=waveshare_rp2040_zero \
  -DPICO_USB_HOST_DP_PIN=6 \
  -DPICO_USB_HOST_DM_PIN=7 \
  -DCONFIG_USB=1 \
  -DPICO_PIO_USB_CLOCK_KHZ=120000 \
  -DPIO_USB_VBUS_PIN=-1 \
  -B build
