/*
 * MachineState.cpp
 *
 *  Created on: 14 de out de 2019
 *      Author: mcloc
 */

#include <MachineState.h>
#include <sintaxes-framework-defines.h>

MachineState::MachineState() {

}

uint32_t MachineState::getUptime() {
	return millis() / 1000; //seconds
}

bool MachineState::init() {
//	for(uint8_t i =0; i< MAX_ACTUATORS;i++){
//
//	}
}

void MachineState::setStateTime() {
	//TODO: millis
}

bool MachineState::addActuator(ActuatorBase **actuator) {

	if (actuator_list_index > MAX_ACTUATORS) {
		return false;
	}

	actuator_list[actuator_list_index] = &(*actuator);
	actuator_list_index++;
	actuator_list_total++;

	return true;
}

ActuatorBase** MachineState::getActuator(uint8_t index) {
	return &(*actuator_list[index]);
}

uint8_t MachineState::getActuatorListSize() {
	return actuator_list_index + 1; // index begins @ 0
}

void MachineState::incRequests() {
	if((sizeof(total_requests_processed + 1)) > sizeof(uint32_t)){
		total_requests_processed = 1;
		return;
	}

	total_requests_processed++;
}
uint32_t MachineState::getTotalRequests() {
	return total_requests_processed;
}
