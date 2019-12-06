/*
 * BME280.cpp
 *
 *  Created on: 14 de out de 2019
 *      Author: mcloc
 */

#include <devices/types/BoschME280.h>
#include <Arduino.h>
#include <sintaxes-lib.h>

SintaxesSensors::SintaxesSensors(uint32_t _uuid){
	uuid = _uuid;
	active = true;
}

const __FlashStringHelper * BME280::getName(){
	return FSH(name);
}

const __FlashStringHelper * BME280::getModel(){
	return FSH(model);
}
