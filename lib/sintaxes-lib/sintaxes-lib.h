#ifndef __SINTAXES_H_
#define __SINTAXES_H_

#include <errors_code.h>
#include <commands_map.h>
#include <devices.h>
#include <sintaxes-framework-defines.h>
#include <msgpack_defines.h>


#include <Arduino.h>
typedef const __FlashStringHelper* FSH;

#include <module_string.h>

#include <StandardCplusplus.h>
#include <UIPEthernet.h>
#include <LocalBuffers.h>
#include <Responses.h>
#include <Commands.h>
#include <MsgPackHandler.h>
#include <DHT.h>


void buzz(uint8_t BUZPIN, int freq, int _delay, uint8_t times = 1);

#endif
