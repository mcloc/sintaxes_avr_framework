/*
 * MsgpackUINT8.h
 *
 *  Created on: 12 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_MSGPACK_TYPE_VALUES_MSGPACKUINT8_H_
#define LIB_SINTAXES_LIB_MSGPACK_TYPE_VALUES_MSGPACKUINT8_H_

#include <msgpack/MsgpackValue.h>
#include <Arduino.h>

class MsgpackUINT8 : public MsgpackValue {
public:
	MsgpackUINT8(uint8_t _value);
	uint8_t getValue();

private:
	uint8_t value;
};

#endif /* LIB_SINTAXES_LIB_MSGPACK_TYPE_VALUES_MSGPACKUINT8_H_ */
