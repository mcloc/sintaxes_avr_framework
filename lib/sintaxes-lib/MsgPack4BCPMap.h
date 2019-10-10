/*
 * MsgPack4BCPMap.h
 *
 *  Created on: 10 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_MSGPACK4BCPMAP_H_
#define LIB_SINTAXES_LIB_MSGPACK4BCPMAP_H_
#include <Arduino.h>
#include <MsgPack4BCPMapElement.h>


class MsgPack4BCPMap {
public:
	MsgPack4BCPMap();
	void setSize(uint8_t size);
	uint8_t getSize();
	uint16_t getPosition();
	MsgPack4BCPMapElement elements[];

private:
	uint8_t size;
	uint16_t position;

};

#endif /* LIB_SINTAXES_LIB_MSGPACK4BCPMAP_H_ */
