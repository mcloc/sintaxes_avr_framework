/*
 * ActuatorBase.cpp
 *
 *  Created on: 15 de out de 2019
 *      Author: mcloc
 */

#include <devices/ActuatorBase.h>
#include <sintaxes-lib.h>

ActuatorBase::ActuatorBase(uint32_t _uuid){
	uuid = _uuid;
}


uint8_t ActuatorBase::getUUID(){
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
