/*
 * MsgPackDataTypes.cpp
 *
 *  Created on: 9 de out de 2019
 *      Author: mcloc
 */

#include <defines/msgpack_defines.h>
#include <msgpack/MsgPackDataTypes.h>


// A T T E N T I O N, this is a copy of #defined macros on msgpack_defines.h which includes MSGPACK_TOTAL_ELEMENTS
// any change here must mirror on msgpack_defines.h including the MSGPACK_TOTAL_ELEMENTS definition
static const uint8_t MsgPackDataTypes::msgpack_types[MSGPACK_TOTAL_ELEMENTS] PROGMEM = {
	0xc0,0xc2,0xc3,0xc4,0xc5,0xc6,
	0xc7,0xc8,0xc9,0xca,0xcb,0xcc,
	0xcd,0xce,0xcf,0xd0,0xd1,0xd2,
	0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,
	0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf
};


MsgPackDataTypes::MsgPackDataTypes() {


}

static bool MsgPackDataTypes::checkDataType(uint8_t _byte){
	for(uint8_t i = 0; i < MSGPACK_TOTAL_ELEMENTS;i++){
		if(_byte == msgpack_types[i])
			return true;
	}

	return false;
}

//MsgPackDataTypes::~MsgPackDataTypes() {
//	// TODO Auto-generated destructor stub
//}

