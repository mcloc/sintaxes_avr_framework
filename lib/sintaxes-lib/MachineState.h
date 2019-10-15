/*
 * MachineState.h
 *
 *  Created on: 14 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_MACHINESTATE_H_
#define LIB_SINTAXES_LIB_MACHINESTATE_H_

#include <Arduino.h>
#include <devices/SintaxesActuator.hpp>
#include <devices/SintaxesSensors.h>
#include <devices/types/DN20.h>

class MachineState {
public:
	MachineState();
	uint32_t runtime_hash;
	uint32_t uptime;
	uint8_t last_error_code;
	uint8_t status;
	uint8_t prev_status;
	bool command_set = false;
	bool processing_command = false;
	uint8_t total_execution_of_command = 0;

//	dn20_1 = Sint;

	SintaxesActuator<DN20> actuator_list[];

//	bool



};

#endif /* LIB_SINTAXES_LIB_MACHINESTATE_H_ */
