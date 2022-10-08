/*
 * DHT21.h
 *
 *  Created on: 14 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_DEVICES_TYPES_DHT21_H_
#define LIB_SINTAXES_LIB_DEVICES_TYPES_DHT21_H_

#include <devices/SintaxesSensors.h>
#include <Arduino.h>

class DHT21 : public SintaxesSensors{
public:
	const __FlashStringHelper * getName();
	const __FlashStringHelper * getModel();

private:
	const char name[35] PROGMEM = "Humidity and Temperature Sensor DHT21";
	const char model[5] PROGMEM = "DHT21";
};

#endif /* LIB_SINTAXES_LIB_DEVICES_TYPES_DHT21_H_ */
