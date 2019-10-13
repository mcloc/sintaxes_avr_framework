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
	MsgpackValue();
	virtual ~MsgpackValue();

	//FIXME: no good, would like a single getValue() with each child class with it's own data type
	// check polimorphism @ runtime: checked, all fucking bull sheet, branched to get reed of Class DataType, and forget switch Case and
	// migrated to plain old void *...

	//Old stuff
//	virtual uint8_t getValueUINT8() = 0;
//	virtual uint16_t getValueUINT16() = 0;
//	virtual uint32_t getValueUINT32() = 0;

//	virtual void *getValue() = 0;
	uint8_t getType();
private:
	void *value;
	uint8_t value_type;
};

#endif /* LIB_SINTAXES_LIB_MSGPACK_MSGPACKVALUE_H_ */
