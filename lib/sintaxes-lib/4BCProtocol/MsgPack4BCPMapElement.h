/*
 * MsgPack4BCPMapElement.h
 *
 *  Created on: 10 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_4BCPROTOCOL_MSGPACK4BCPMAPELEMENT_H_
#define LIB_SINTAXES_LIB_4BCPROTOCOL_MSGPACK4BCPMAPELEMENT_H_

#include <Arduino.h>
#include <defines/msgpack_defines.h>
#include <defines/sintaxes-framework-defines.h>



class MsgPack4BCPMapElement {
public:
	MsgPack4BCPMapElement();
	void setKey(uint32_t key);
	void setType(uint8_t value_type);
	bool setElement(MsgPack4BCPMapElement _element);
	bool setValue();
	uint8_t getTotalNestedElements();
	bool incTotalNestedElements();



	unsigned long key;
	uint8_t value_type;

	bool value_bool;
	char value_fixstring[MSGPACK_FIXSTR_LEGNT];
	float value_float;
	uint8_t value_uint8;
	uint16_t value_uint16;
	uint32_t value_uint32;
	int8_t value_int8;
	int16_t value_int16;
	int32_t value_int32;
	uint8_t value_fixmap;
	uint8_t value_fixarray;

private:
	MsgPack4BCPMapElement *nested_element[MAX_MSGPACK_COMMANDS];
	uint8_t total_nested_elements = 0;
};

#endif /* LIB_SINTAXES_LIB_4BCPROTOCOL_MSGPACK4BCPMAPELEMENT_H_ */
