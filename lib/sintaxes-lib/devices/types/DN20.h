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

class DN20 {
public:

protected:
	const char name[] PROGMEM = "Solenoid HydroValve DN20";
	const char model[] PROGMEM = "DN20";
};

#endif /* LIB_SINTAXES_LIB_DEVICES_TYPES_DN20_H_ */
