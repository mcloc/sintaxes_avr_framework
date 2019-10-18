/*
 * 4BCPElement.h
 *
 *  Created on: 18 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_4BCPROTOCOL_4BCPELEMENT_H_
#define LIB_SINTAXES_LIB_4BCPROTOCOL_4BCPELEMENT_H_
#include <Arduino.h>
#include <defines/sintaxes-framework-defines.h>


//	//4Bytes Command Protocol map
	//old_c_structs
typedef struct _4BCPElementValue {
	uint8_t uint8_value;
	uint16_t uint16_value;
	uint32_t uint32_value;
	int8_t int8_value;
	int16_t int16_value;
	int32_t int32_value;
	float float_value;
	bool bool_value;
	char char_value;
} _4BCPElementValue;

typedef struct _4BCPMapElement {
	uint32_t key;
	uint8_t value_type;
	uint8_t total_nested_elements = 0;
	_4BCPElementValue *value;
	_4BCPMapElement *nested_elements[MAX_MSGPACK_NESTED_ELEMENTS];
} _4BCPMapElement;

typedef struct _4BCPMap {
	uint8_t size = 0;
	_4BCPMapElement *elements[MAX_MSGPACK_COMMAND_LOOP];
} _4BCPMap;



class _4BCPElement {
public:
	_4BCPElement();
	static _4BCPMap map4BCP;
	static _4BCPMapElement *element4BCP_1;
	static _4BCPMapElement *element4BCP_2;
	static _4BCPMapElement *element4BCP_3;
	static _4BCPMapElement *element4BCP_4;
	static _4BCPMapElement *element4BCP_5;
	static _4BCPMapElement *element4BCP_6;
	static _4BCPMapElement *element4BCP_7;
	static _4BCPMapElement *element4BCP_8;
	static _4BCPMapElement *element4BCP_9;

	static _4BCPElementValue *value_1;
	static _4BCPElementValue *value_2;
	static _4BCPElementValue *value_3;
	static _4BCPElementValue *value_4;
	static _4BCPElementValue *value_5;
	static _4BCPElementValue *value_6;
};

#endif /* LIB_SINTAXES_LIB_4BCPROTOCOL_4BCPELEMENT_H_ */
