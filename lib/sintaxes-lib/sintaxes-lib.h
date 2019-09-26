#ifndef __SINTAXES_H_
#define __SINTAXES_H_

#include <errors_code.h>
#include <commands_map.h>
#include <devices.h>
#include <sintaxes-framework-defines.h>

#include <Arduino.h>

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
	uint8_t BUZZPIN;

};




#endif
