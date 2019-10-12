/*
 * MsgPack4BCPMap.cpp
 *
 *  Created on: 10 de out de 2019
 *      Author: mcloc
 */

#include <4BCProtocol/MsgPack4BCPMap.h>

MsgPack4BCPMap::MsgPack4BCPMap() {
	size = 0;
	position = 0;
}

void MsgPack4BCPMap::setSize(uint8_t _size){
	size = _size;
	position = 0;
}
uint8_t MsgPack4BCPMap::getSize(){
	return size;
}
uint16_t MsgPack4BCPMap::getPosition(){
	return position;
}

bool MsgPack4BCPMap::haveElements(){
	if(! size > 0)
		return false;

	return true;
}
