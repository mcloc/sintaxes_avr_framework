#!/bin/bash
avrdude -v -patmega328p -carduino -P/dev/ttyUSB0   -b115200 -D -U flash:w:build/.default/etherCard.hex
