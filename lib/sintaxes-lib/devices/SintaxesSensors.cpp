/*
 * SintaxesSensors.cpp
 *
 *  Created on: 14 de out de 2019
 *      Author: mcloc
 */

#include <devices/SintaxesSensors.h>
#include <Arduino.h>


uint8_t SintaxesSensors::getUUID(){
	return uuid;
}

bool SintaxesSensors::isActive(){
	return active;
}
