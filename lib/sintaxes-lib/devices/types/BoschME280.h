/*
 * DHT21.h
 *
 *  Created on: 14 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_DEVICES_TYPES_BME280_H_
#define LIB_SINTAXES_LIB_DEVICES_TYPES_BME280_H_

#include <devices/SintaxesSensors.h>
#include <Arduino.h>

class BME280 : public SintaxesSensors{
public:
	const __FlashStringHelper * getName();
	const __FlashStringHelper * getModel();

private:
	const char name[47] PROGMEM = "Humidity,Temperature and Pressor Sensor BME280";
	const char model[6] PROGMEM = "BME280";
};


#endif /* LIB_SINTAXES_LIB_DEVICES_TYPES_DHT21_H_ */
