#ifndef __SINTAXES_H_
#define __SINTAXES_H_

#include <Arduino.h>

#ifndef FSH
typedef const __FlashStringHelper* FSH;
#endif

#include <defines/commands_map.h>
#include <defines/devices.h>
#include <defines/errors_code.h>
#include <defines/sintaxes-framework-defines.h>
//#include "msgpack/MsgPackHandler.h"

//#define foreach(list, index) for(index = 0; index < list.size(); index++)
//foreach(cookies, i)
//    printf("Cookie: %s", cookies[i]);
//Since C++11, this is superseded by the range-based for loop.

class SintaxesLib;

//TODO: create namespace
class SintaxesLib {
public:
	SintaxesLib();
	void buzz(int freq, int _delay, uint8_t times = 1);
	void setLed(uint8_t pin_led, uint8_t level);
	void blink(uint8_t pin_led, uint8_t _delay, uint8_t times = 1);
	uint8_t _BUZZPIN;

};

#endif
