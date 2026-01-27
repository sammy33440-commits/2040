#!/bin/sh
cmake -G "Unix Makefiles" \
  -DFAMILY=rp2040 \
  -DPICO_BOARD=waveshare_rp2040_zero \
  -DPICO_USB_HOST_DP_PIN=7 \
  -DPICO_USB_HOST_DM_PIN=6 \
  -DCONFIG_USB=1 \
  -B build
