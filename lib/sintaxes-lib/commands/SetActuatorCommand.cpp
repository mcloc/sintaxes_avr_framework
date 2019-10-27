/*
 * SetActuator.cpp
 *
 *  Created on: 21 de out de 2019
 *      Author: mcloc
 */

#include <commands/SetActuatorCommand.h>
#include <memory/ApplianceMemmoryHandler.h>


SetActuatorCommand::SetActuatorCommand(uint32_t _device_key) {
	device_key = _device_key;
	state = '\0';
	state_duration = '\0';

//	ApplianceMemmoryHandler::responses->writeRaw(F("SetActuator()_constructor device_key"));
//	ApplianceMemmoryHandler::responses->write32bitByte(device_key);
}

bool SetActuatorCommand::execute(){
	if(!checkArguments()){
		return false;
	}

	for(uint8_t i = 0; i < ApplianceMemmoryHandler::machine_state->actuator_list_total;i++){
//		ApplianceMemmoryHandler::responses->writeRaw(F("actuator:"));
//		ApplianceMemmoryHandler::responses->write32bitByte((*ApplianceMemmoryHandler::machine_state->getActuator(i))->uuid);

		if((*ApplianceMemmoryHandler::machine_state->getActuator(i))->uuid == device_key) {
			(*ApplianceMemmoryHandler::machine_state->getActuator(i))->setNewState(state, state_duration );
			ApplianceMemmoryHandler::responses->writeRaw(F("****************************** R E A L   E X E C U T I O N   V A L U E S *****************************"));
				ApplianceMemmoryHandler::responses->writeRaw(F("INSIDE SetActuatorExecute.... bring to here the set of real actuator in machine state:"));
				ApplianceMemmoryHandler::responses->writeRaw(F("command to execute:"));
				ApplianceMemmoryHandler::responses->write32bitByte(command);
				ApplianceMemmoryHandler::responses->writeRaw(F("command devuce_key::"));
				ApplianceMemmoryHandler::responses->write32bitByte(device_key);
				ApplianceMemmoryHandler::responses->writeRaw(F("command state:"));
				ApplianceMemmoryHandler::responses->writeByte(state);
				ApplianceMemmoryHandler::responses->writeRaw(F("command duration:"));
				ApplianceMemmoryHandler::responses->write32bitByte(state_duration);
				ApplianceMemmoryHandler::responses->writeRaw(F("CALL COMMMAND SET ACTUATOR AND MACHINE STATUS:"));
				ApplianceMemmoryHandler::responses->writeRaw(F("******************************************************************************************************"));

			break;
		}
	}

	return true;

}

bool SetActuatorCommand::checkArguments(){
	if((state == '\0' && state_duration == '\0') || state_duration == '\0') {
//		error_code = ERROR_COMMAND_EXECUTION_FAILED;
		ApplianceMemmoryHandler::responses->writeRaw(F("ERRO AO EXECUTAR DEVICE:"));
		ApplianceMemmoryHandler::responses->write32bitByte(device_key);
		ApplianceMemmoryHandler::responses->write4BCPCommandArgsMissing();
		return false;
	}


	return true;
}

SetActuatorCommand::~SetActuatorCommand(){
	device_key = '\0';
	state = '\0';
	state_duration = '\0';
}


