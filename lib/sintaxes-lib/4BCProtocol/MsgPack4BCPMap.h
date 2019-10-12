/*
 * MsgPack4BCPMap.h
 *
 *  Created on: 10 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_4BCPROTOCOL_MSGPACK4BCPMAP_H_
#define LIB_SINTAXES_LIB_4BCPROTOCOL_MSGPACK4BCPMAP_H_
#include <4BCProtocol/MsgPack4BCPMapElement.h>
#include <Arduino.h>


class MsgPack4BCPMap {
public:
	MsgPack4BCPMap();
	void setSize(uint8_t size);
	uint8_t getSize();
	uint16_t getPosition();
	MsgPack4BCPMapElement elements[MAX_MSGPACK_COMMANDS];
	bool haveElements();

private:
	uint8_t size;
	uint16_t position;

};

#endif /* LIB_SINTAXES_LIB_4BCPROTOCOL_MSGPACK4BCPMAP_H_ */
