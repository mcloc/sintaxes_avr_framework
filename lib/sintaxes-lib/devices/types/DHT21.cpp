/*
 * DHT21.cpp
 *
 *  Created on: 14 de out de 2019
 *      Author: mcloc
 */

#include <devices/types/DHT21.h>
#include <Arduino.h>
#include <sintaxes-lib.h>

SintaxesSensors::SintaxesSensors(uint32_t _uuid){
	uuid = _uuid;
	active = true;
}

const __FlashStringHelper * DHT21::getName(){
	return FSH(name);
}

const __FlashStringHelper * DHT21::getModel(){
	return FSH(model);
}
