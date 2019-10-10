/*
 * MsgPack4BCPMapElement.cpp
 *
 *  Created on: 10 de out de 2019
 *      Author: mcloc
 */

#include <MsgPack4BCPMapElement.h>
#include <msgpack_defines.h>
#include <commands_map.h>
#include <devices.h>

MsgPack4BCPMapElement::MsgPack4BCPMapElement() {
	// TODO Auto-generated constructor stub
}

void MsgPack4BCPMapElement::setKey(uint32_t _key){
	key = _key;
}

void MsgPack4BCPMapElement::setType(uint8_t _value_type){
	value_type = _value_type;
}

bool MsgPack4BCPMapElement::setElement(MsgPack4BCPMapElement _element){
	if(total_nested_elements == 0){
			nested_element[total_nested_elements] = &_element;
			incTotalNestedElements();
			return true;
	}

	if(total_nested_elements < MAX_MSGPACK_ARGS){
		nested_element[total_nested_elements] = &_element;
		incTotalNestedElements();
		return true;
	}

	return false;
}

uint8_t MsgPack4BCPMapElement::getTotalNestedElements(){
	return total_nested_elements;
}
bool MsgPack4BCPMapElement::incTotalNestedElements(){
	total_nested_elements++;
	if(total_nested_elements >= MAX_MSGPACK_ARGS)
		return false;

	return true;
}

bool MsgPack4BCPMapElement::setValue() {
	switch(value_type){
		case MSGPACK_NIL: {
			return true;
		}

		case MSGPACK_FALSE: {
			value_bool = false;
			return true;
		}

		case MSGPACK_TRUE: {
			value_bool = true;
			return true;
		}

//		case MSGPACK_BIN8: {
//			return true;
//		}
//
//		case MSGPACK_BIN16: {
//			return true;
//		}
//
//		case MSGPACK_BIN32: {
//			return true;
//		}
//
//		case MSGPACK_EXT8: {
//			return true;
//		}
//
//		case MSGPACK_EXT16: {
//			return true;
//		}
//
//		case MSGPACK_EXT32: {
//			return true;
//		}

		case MSGPACK_FLOAT32: {
//			value_float = _
			return true;
		}

		case MSGPACK_FLOAT64: {
			return true;
		}

		case MSGPACK_UINT8: {
			return true;
		}

		case MSGPACK_UINT16: {
			return true;
		}

		case MSGPACK_UINT32: {
			return true;
		}

		case MSGPACK_UINT64: {
			return true;
		}

		case MSGPACK_INT8: {
			return true;
		}

		case MSGPACK_INT16: {
			return true;
		}

		case MSGPACK_INT32: {
			return true;
		}

		case MSGPACK_INT64: {
			return true;
		}

		case MSGPACK_FIXEXT1: {
			return true;
		}

		case MSGPACK_FIXEXT2: {
			return true;
		}

		case MSGPACK_FIXEXT4: {
			return true;
		}

		case MSGPACK_FIXEXT8: {
			return true;
		}

		case MSGPACK_FIXEXT16: {
			return true;
		}

		case MSGPACK_STR8: {
			return true;
		}

		case MSGPACK_STR16: {
			return true;
		}

		case MSGPACK_STR32: {
			return true;
		}

		case MSGPACK_ARRAY16: {
			return true;
		}

		case MSGPACK_ARRAY32: {
			return true;
		}

		case MSGPACK_MAP16: {
			return true;
		}

		case MSGPACK_MAP32: {
			return true;
		}

		default:{
			return false;
		}
	}

	return false;
}

