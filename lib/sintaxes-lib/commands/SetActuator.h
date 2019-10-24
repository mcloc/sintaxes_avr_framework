/*
 * SetActuator.h
 *
 *  Created on: 21 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_COMMANDS_SETACTUATOR_H_
#define LIB_SINTAXES_LIB_COMMANDS_SETACTUATOR_H_

#include <Arduino.h>
#include <defines/commands_map.h>
#include <commands/CommandBase.h>
#include <Responses.h>


class SetActuator : public CommandBase{
public:
	SetActuator();
	SetActuator(Responses *_response, uint32_t device_key, MachineState * _machine_state);
	bool execute();
	bool checkArguments();
	~SetActuator();
	bool state;
	uint32_t state_duration; //in seconds overflow in 231 MAX days approximately

protected:
	static const uint32_t command = MODULE_COMMMAND_SET_ACTUATOR;
	uint32_t device_key;
	Responses *responses;
	MachineState *machine_state;


};

#endif /* LIB_SINTAXES_LIB_COMMANDS_SETACTUATOR_H_ */
