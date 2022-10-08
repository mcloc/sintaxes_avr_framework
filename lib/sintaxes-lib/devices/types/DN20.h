/*
 * DN20.h
 *
 *  Created on: 14 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_DEVICES_TYPES_DN20_H_
#define LIB_SINTAXES_LIB_DEVICES_TYPES_DN20_H_
//#include <devices/SintaxesActuator.h>
#include <Arduino.h>
#include <devices/ActuatorBase.h>

class DN20 : public ActuatorBase {
public:
	DN20(uint32_t uuid, uint8_t PIN) : ActuatorBase(uuid, PIN){}

protected:
	const char name[22] PROGMEM = "Solenoid HydroValve DN20";
	const char model[4] PROGMEM = "DN20";
};

#endif /* LIB_SINTAXES_LIB_DEVICES_TYPES_DN20_H_ */
