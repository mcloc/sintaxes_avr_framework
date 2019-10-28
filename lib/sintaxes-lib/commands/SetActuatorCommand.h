/*
 * SetActuator.h
 *
 *  Created on: 21 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_COMMANDS_SETACTUATORCOMMAND_H_
#define LIB_SINTAXES_LIB_COMMANDS_SETACTUATORCOMMAND_H_

#include <Arduino.h>
#include <defines/commands_map.h>
#include <commands/CommandBase.h>
#include <Responses.h>


class SetActuatorCommand : public CommandBase{
public:
	SetActuatorCommand();
	SetActuatorCommand(uint32_t device_key);
	bool execute();
	bool checkArguments();
	~SetActuatorCommand();
	bool state;
	uint32_t state_duration; //in seconds overflow in 231 MAX days approximately
	uint32_t device_key;
	SetActuatorCommand ** cmd_ptr;
protected:
	static const uint32_t command = MODULE_COMMMAND_SET_ACTUATOR;


};

#endif /* LIB_SINTAXES_LIB_COMMANDS_SETACTUATORCOMMAND_H_ */
