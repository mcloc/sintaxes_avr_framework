/*
 * ActuatorBase.h
 *
 *  Created on: 15 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_DEVICES_ACTUATORBASE_H_
#define LIB_SINTAXES_LIB_DEVICES_ACTUATORBASE_H_

#include <Arduino.h>


//typedef struct ActuatorLog {
//	bool  prev_state;
//	bool  state;
//	uint32_t last_state_duration;
//	uint32_t start_state_millis;
//} ActuatorLog; // save in the SDCard

class ActuatorBase {
public:
	ActuatorBase(uint32_t _uuid, uint8_t PIN_NUMBER);
	const __FlashStringHelper * getName();
	const __FlashStringHelper * getModel();
	uint32_t getUUID();
	uint32_t getStateDuration();
	bool isActive();
	bool setNewState(bool state, uint32_t state_duration);
	uint32_t uuid;
protected:

	uint8_t PIN_NUMBER;
	bool active = false;
	uint32_t state_duration; // seconds ~ 255 days to overflow
	uint32_t last_state_changed;
	uint32_t total_changes = 0;
	static const __FlashStringHelper * name;
	static const __FlashStringHelper * model;
};

#endif /* LIB_SINTAXES_LIB_DEVICES_ACTUATORBASE_H_ */
