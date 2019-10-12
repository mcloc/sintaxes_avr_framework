/*
 * MsgPackDataTypes.h
 *
 *  Created on: 9 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_MSGPACK_MSGPACKDATATYPES_H_
#define LIB_SINTAXES_LIB_MSGPACK_MSGPACKDATATYPES_H_

#include <Arduino.h>
#include <defines/msgpack_defines.h>
#include <defines/sintaxes-framework-defines.h>

class MsgPackDataTypes {
public:
	MsgPackDataTypes();
	static bool checkDataType(uint8_t _byte);

//	virtual ~MsgPackDataTypes();

private:
	static const uint8_t msgpack_types[MSGPACK_TOTAL_ELEMENTS];

};

#endif /* LIB_SINTAXES_LIB_MSGPACK_MSGPACKDATATYPES_H_ */
