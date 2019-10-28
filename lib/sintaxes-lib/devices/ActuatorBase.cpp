/*
 * ActuatorBase.cpp
 *
 *  Created on: 15 de out de 2019
 *      Author: mcloc
 */

#include <devices/ActuatorBase.h>
#include <sintaxes-lib.h>
#include <Arduino.h>

ActuatorBase::ActuatorBase(uint32_t _uuid, uint8_t _PIN_NUMBER){
	uuid = _uuid;
	PIN_NUMBER = _PIN_NUMBER;
}

bool ActuatorBase::setNewState(bool state, uint32_t _state_duration){

	pinMode(PIN_NUMBER, OUTPUT);
	switch(state){
		case true: {
			digitalWrite(PIN_NUMBER, HIGH);
			break;
		}
		case false: {
			digitalWrite(PIN_NUMBER, LOW);
			break;
		}
		default: {
			return false;
		}
	}

	return true;
}


uint32_t ActuatorBase::getUUID(){
	return uuid;
}

bool ActuatorBase::isActive(){
	return active;
}

const __FlashStringHelper * ActuatorBase::getName(){
	return FSH(name);
}

const __FlashStringHelper * ActuatorBase::getModel(){
	return FSH(model);
}

uint32_t ActuatorBase::getStateDuration(){
	return state_duration;
}
