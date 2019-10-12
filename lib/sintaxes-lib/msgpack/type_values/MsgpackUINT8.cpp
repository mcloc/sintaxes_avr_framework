/*
 * MsgpackUINT8.cpp
 *
 *  Created on: 12 de out de 2019
 *      Author: mcloc
 */

#include <msgpack/type_values/MsgpackUINT8.h>
#include <Arduino.h>

MsgpackUINT8::MsgpackUINT8(uint8_t _value) {
	value = _value;
}

uint8_t MsgpackUINT8::getValue(){
	return value;
}
