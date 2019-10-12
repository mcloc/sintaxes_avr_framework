/*
 * MsgPackValue.h
 *
 *  Created on: 12 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_MSGPACK_MSGPACKVALUE_H_
#define LIB_SINTAXES_LIB_MSGPACK_MSGPACKVALUE_H_

#include <Arduino.h>

//abstract class for all msgpack data type classes
class MsgpackValue {
public:
	virtual MsgpackValue() = 0;
	virtual ~MsgpackValue() = 0;
	virtual auto getValue() = 0;
	uint8_t getType();
private:
	uint8_t value_type;
};

#endif /* LIB_SINTAXES_LIB_MSGPACK_MSGPACKVALUE_H_ */
