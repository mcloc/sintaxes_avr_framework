#!/bin/bash
#avrdude -v -p atmega2560 -carduino -P/dev/ttyUSB0   -b115200 -D -U flash:w:.pio/build/uno/firmware.hex
#avrdude -v -patmega2560 -cwriring C /home/mcloc/.platformio/packages/tool-avrdude/avrdude.conf  -b115200 -D -P"/dev/ttyUSB0" -U flash:w:.pio/build/mega/firmware.hex:i
#avrdude -v -p atmega2560  -c arduino -b 9600 -D -P "/dev/ttyUSB0" -U flash:w:.pio/build/mega/firmware.hex:i

#/usr/local/arduino-1.8.10/hardware/tools/avr/bin/avrdude -C/usr/local/arduino-1.8.10/hardware/tools/avr/etc/avrdude.conf -v -patmega2560 -cwiring -P/dev/ttyUSB0 -b115200 -D -Uflash:w:/tmp/arduino_build_232009/sketch_sep27a.ino.hex:i
/usr/local/arduino-1.8.10/hardware/tools/avr/bin/avrdude -C/usr/local/arduino-1.8.10/hardware/tools/avr/etc/avrdude.conf -v -patmega2560 -cwiring -P/dev/ttyUSB0 -b115200 -D -Uflash:w:.pio/build/mega/firmware.hex:i

