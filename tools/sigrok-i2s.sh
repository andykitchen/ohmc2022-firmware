#!/bin/sh

# use sigrok-cli to dump

sigrok-cli -P i2s:sck=I2S.pads_sclk:ws=I2S.pads_lrclk:sd=I2S.pads_dout -i "$@"
