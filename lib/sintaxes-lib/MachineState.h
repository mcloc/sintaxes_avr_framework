/*
 * MachineState.h
 *
 *  Created on: 14 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_MACHINESTATE_H_
#define LIB_SINTAXES_LIB_MACHINESTATE_H_

#include <Arduino.h>
#include <devices/SintaxesSensors.h>
#include <devices/types/DN20.h>
#include <sintaxes-framework-defines.h>

class MachineState {
public:
	MachineState();
	uint32_t runtime_hash;
	uint32_t start_time;
	uint8_t last_error_code;
	uint8_t status;
	uint8_t prev_status;
	bool command_set = false;
	bool processing_command = false;
	uint8_t total_execution_of_command = 0;
	bool addActuator(ActuatorBase **actuator);
	uint8_t getActuatorListSize();
	bool init();
	void setStateTime();
	uint32_t getUptime();
	ActuatorBase ** getActuator(uint8_t index);
	void incRequests();
	uint32_t getTotalRequests();
	void clean();

	uint8_t actuator_list_total = 0;


private:
	uint8_t actuator_list_index = 0;
	uint32_t total_requests_processed = 0;
	ActuatorBase** actuator_list[MAX_ACTUATORS];
	uint32_t state_time;



};

#endif /* LIB_SINTAXES_LIB_MACHINESTATE_H_ */
