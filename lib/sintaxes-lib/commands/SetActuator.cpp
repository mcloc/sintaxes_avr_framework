/*
 * SetActuator.cpp
 *
 *  Created on: 21 de out de 2019
 *      Author: mcloc
 */

#include <commands/SetActuator.h>

SetActuator::SetActuator(Responses *_response, uint32_t _device_key) {
	response = _response;
	device_key = _device_key;
	state = '\0';
	state_duration = '\0';
}

bool SetActuator::execute(){
	if(!checkArguments()){
		return false;
	}

	response->writeRaw(F("****************************** R E A L   E X E C U T I O N   V A L U E S *****************************"));
	response->writeRaw(F("INSIDE SetActuatorExecute.... bring to here the set of real actuator in machine state:"));
	response->writeRaw(F("command to execute:"));
	response->write32bitByte(command);
	response->writeRaw(F("command devuce_key::"));
	response->write32bitByte(device_key);
	response->writeRaw(F("command state:"));
	response->writeByte(state);
	response->writeRaw(F("command duration:"));
	response->write32bitByte(state_duration);
	response->writeRaw(F("CALL COMMMAND SET ACTUATOR AND MACHINE STATUS:"));
	response->writeRaw(F("******************************************************************************************************"));

	return true;

}

bool SetActuator::checkArguments(){
	if((state == '\0' && state_duration == '\0') || state_duration == '\0') {
//		error_code = ERROR_COMMAND_EXECUTION_FAILED;
		response->writeRaw(F("ERRO AO EXECUTAR DEVICE:"));
		response->write32bitByte(device_key);
		response->write4BCPCommandArgsMissing();
		return false;
	}


	return true;
}

SetActuator::~SetActuator(){
	response = '\0';
	device_key = '\0';
	state = '\0';
	state_duration = '\0';
}
