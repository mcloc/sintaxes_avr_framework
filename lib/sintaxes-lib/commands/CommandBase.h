/*
 * CommandBase.h
 *
 *  Created on: 21 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_COMMANDS_COMMANDBASE_H_
#define LIB_SINTAXES_LIB_COMMANDS_COMMANDBASE_H_

#include <Arduino.h>
#include <Responses.h>
#include <MachineState.h>

class CommandBase {
	uint32_t device_key;

public:
	CommandBase();
	CommandBase(Responses *reponse, uint32_t command, uint32_t device_key);
	CommandBase **getCommandObj();
	virtual bool execute();
	virtual bool checkArguments();

	static  uint32_t command;
	Responses *response;


};

#endif /* LIB_SINTAXES_LIB_COMMANDS_COMMANDBASE_H_ */
