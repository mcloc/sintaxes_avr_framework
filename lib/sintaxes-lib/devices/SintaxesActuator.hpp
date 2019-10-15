/*
 * SintaxesActuator.h
 *
 *  Created on: 14 de out de 2019
 *      Author: mcloc
 */


#pragma once

#ifndef __MODULE_SINTAXES_ACTUATORS_H_
#define __MODULE_SINTAXES_ACTUATORS_H_

#include <Arduino.h>

template <class T>
class SintaxesActuator {
public:
	SintaxesActuator(uint32_t uuid);
	const __FlashStringHelper * getName();
	const __FlashStringHelper * getModel();
	uint8_t getUUID();
	uint32_t getStatePeriod();
	bool isActive();
protected:
	uint32_t uuid;
	bool active = false;
	uint32_t state_period;
	const __FlashStringHelper * name;
	const __FlashStringHelper * model;
};


template <class T>
SintaxesActuator<T>::SintaxesActuator(uint32_t _uuid){
	uuid = _uuid;
}


template <class T>
uint8_t SintaxesActuator<T>::getUUID(){
	return uuid;
}

template <class T>
bool SintaxesActuator<T>::isActive(){
	return active;
}

template <class T>
const __FlashStringHelper * SintaxesActuator<T>::getName(){
	return name;
}

template <class T>
const __FlashStringHelper * SintaxesActuator<T>::getModel(){
	return model;
}

template <class T>
uint32_t SintaxesActuator<T>::getStatePeriod(){
	return state_period;
}

#endif
