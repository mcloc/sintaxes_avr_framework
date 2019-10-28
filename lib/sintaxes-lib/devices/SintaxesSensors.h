/*
 * SintaxesSensors.h
 *
 *  Created on: 14 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_DEVICES_SINTAXESSENSORS_H_
#define LIB_SINTAXES_LIB_DEVICES_SINTAXESSENSORS_H_

#include <Arduino.h>

class SintaxesSensors {
public:
	SintaxesSensors(uint32_t uuid);
	virtual const __FlashStringHelper * getName() = 0;
	virtual const __FlashStringHelper * getModel() = 0;
	uint8_t getUUID();
	bool isActive();
protected:
	uint32_t uuid;
	bool active = false;
};

#endif /* LIB_SINTAXES_LIB_DEVICES_SINTAXESSENSORS_H_ */
