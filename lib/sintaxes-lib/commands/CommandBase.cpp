/*
 * CommandBase.cpp
 *
 *  Created on: 21 de out de 2019
 *      Author: mcloc
 */

#include <commands/CommandBase.h>
#include <defines/commands_map.h>
#include <MachineState.h>
#include <commands/SetActuator.h>
#include <Responses.h>

uint32_t command;

CommandBase::CommandBase(){

}

CommandBase::CommandBase(Responses *_response, uint32_t _command, uint32_t _device_key) {
	// TODO Auto-generated constructor stub
	command = _command;
	response = _response;
	device_key = _device_key;

}

CommandBase ** CommandBase::getCommandObj(){
	switch(command){
	case MODULE_COMMMAND_GET_STATE: {
		response->writeMsgPack4BCPMethodUnimplemented(command);
		return '\0';
	}
	case MODULE_COMMMAND_GET_DATA: {
		response->writeMsgPack4BCPMethodUnimplemented(command);
		return '\0';
	}
	case MODULE_COMMAND_GET_PROCESS_FLOW: {
		response->writeMsgPack4BCPMethodUnimplemented(command);
		return '\0';
	}
	case MODULE_COMMMAND_SET_ACTUATOR: {
		SetActuator actuator_command = SetActuator(response, device_key);
		SetActuator * cmd_ptr = (SetActuator*) malloc(sizeof(SetActuator));
		return &cmd_ptr;
	}
	default: {
		response->writeMsgPack4BCPMethodUnimplemented(command);
		return '\0';
	}

	}
}

