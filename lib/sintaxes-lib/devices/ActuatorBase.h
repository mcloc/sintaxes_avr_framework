/*
 * ActuatorBase.h
 *
 *  Created on: 15 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_DEVICES_ACTUATORBASE_H_
#define LIB_SINTAXES_LIB_DEVICES_ACTUATORBASE_H_

#include <Arduino.h>


class ActuatorBase {
public:
	ActuatorBase(uint32_t _uuid);
	const __FlashStringHelper * getName();
	const __FlashStringHelper * getModel();
	uint8_t getUUID();
	uint32_t getStatePeriod();
	bool isActive();
protected:
	uint32_t uuid;
	bool active = false;
	uint32_t state_period;
	static const __FlashStringHelper * name;
	static const __FlashStringHelper * model;
};

#endif /* LIB_SINTAXES_LIB_DEVICES_ACTUATORBASE_H_ */
