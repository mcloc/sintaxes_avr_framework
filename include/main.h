#ifndef __MAIN_MODULE_H_
#define __MAIN_MODULE_H_
#include "Arduino.h"
#include "avr/pgmspace.h"
#include "commands_map.h"
#include "devices.h"
#include <sintaxes-lib.h>

//void buzz(uint8_t BUZZPIN, int freq, int _delay, uint8_t times = 1);

#define STX 0x02
#define ETX 0x03
#define MAX_SIZE_ALLOWED_REQUEST 180 // 180 bytes allowed in post get incomming requests
#define GREEN_LED 6
#define DHT1PIN 7
#define DHT2PIN 8
#define DHTTYPE DHT21   // DHT 11



#endif

