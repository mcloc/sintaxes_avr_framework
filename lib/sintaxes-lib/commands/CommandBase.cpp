/*
 * CommandBase.cpp
 *
 *  Created on: 21 de out de 2019
 *      Author: mcloc
 */

#include <memory/ApplianceMemmoryHandler.h>
#include <commands/CommandBase.h>
#include <commands/SetActuatorCommand.h>
#include <defines/commands_map.h>
#include <MachineState.h>
#include <Responses.h>


CommandBase::CommandBase() = default;

CommandBase::CommandBase(uint32_t _command, uint32_t _device_key) {
	// TODO Auto-generated constructor stub
	command = _command;
	device_key = _device_key;

}

CommandBase::~CommandBase(){

}

//bool CommandBase::setCommandObj(){
//	switch(command){
//	case MODULE_COMMMAND_GET_STATE: {
//		ApplianceMemmoryHandler::responses->writeMsgPack4BCPMethodUnimplemented(command);
//		return '\0';
//	}
//	case MODULE_COMMMAND_GET_DATA: {
//		ApplianceMemmoryHandler::responses->writeMsgPack4BCPMethodUnimplemented(command);
//		return '\0';
//	}
//	case MODULE_COMMAND_GET_PROCESS_FLOW: {
//		ApplianceMemmoryHandler::responses->writeMsgPack4BCPMethodUnimplemented(command);
//		return '\0';
//	}
//	case MODULE_COMMMAND_SET_ACTUATOR: { // BAD temporry pointer FIXME:
//		cmd_ptr = (SetActuator**) malloc(sizeof(SetActuator));
//		cmd_ptr = &SetActuator(device_key);
//
//	}
//	default: {
//		ApplianceMemmoryHandler::responses->writeMsgPack4BCPMethodUnimplemented(command);
//		return '\0';
//	}
//
//	}
//}

