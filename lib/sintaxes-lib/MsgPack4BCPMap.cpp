/*
 * MsgPack4BCPMap.cpp
 *
 *  Created on: 10 de out de 2019
 *      Author: mcloc
 */

#include <MsgPack4BCPMap.h>

MsgPack4BCPMap::MsgPack4BCPMap() {
	// TODO Auto-generated constructor stub

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
