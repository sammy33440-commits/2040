#!/bin/sh
# Build script pour Waveshare RP2040-PiZero
# Force GPIO 6/7 et active CONFIG_USB

cmake -G "Unix Makefiles" \
  -DFAMILY=rp2040 \
  -DPICO_BOARD=waveshare_rp2040_zero \
  -DPICO_USB_HOST_DP_PIN=6 \
  -DPICO_USB_HOST_DM_PIN=7 \
  -DCONFIG_USB=1 \
  -B build
