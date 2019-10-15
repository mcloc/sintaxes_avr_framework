/*
 * MachineState.cpp
 *
 *  Created on: 14 de out de 2019
 *      Author: mcloc
 */

#include <MachineState.h>
#include <sintaxes-framework-defines.h>

MachineState::MachineState() {
	// TODO Auto-generated constructor stub

}
bool MachineState::addActuatorList(ActuatorBase actuator){

	if(actuator_list_index > MAX_ACTUATORS){
		return false;
	}

	actuator_list[actuator_list_index] = actuator;
	actuator_list_index++;

	return true;
}

uint8_t MachineState::getActuatorListSize(){
	return actuator_list_index+1; // index begins @ 0
}
