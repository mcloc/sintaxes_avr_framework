#pragma once

#ifndef __MODULE_COMMANDS_H_
#define __MODULE_COMMANDS_H_

#include <defines/sintaxes-framework-defines.h>
#include <DHT.h>
#include <LocalBuffers.h>
#include <Responses.h>
#include <MachineState.h>
#include <4BCProtocol/4BCPContainer.h>

//class Commands;

typedef struct CommandStruct {
	uint32_t command;
	_4BCPMapElement *devices_element_list[MAX_MSGPACK_COMMAND_LOOP * sizeof(int *)];
	uint8_t total_devices_executed = 0; // mapSize
} CommandStruct;

class CommandsHandler {

public:

	/**
	 * MACINE ACTUAL STATE packed with messagePack
	 */
	//static char module_actual_state[200]; MAde on the fly
//    static char module_old_state[200]; // made before execution commands for rollback
	/**
	 * command to execute / in execution
	 */
	static uint32_t command_executing;

	CommandsHandler();
	CommandsHandler(LocalBuffers *localBuffers, Responses *_response);

	void setMachineState(MachineState **_machine_state);
	bool get_data();
	void initSensors();
	bool set_actuator();

	bool assembleCommand();
	bool assembleCommand(uint32_t command_key);
	bool assembleCommand(_4BCPMapElement *nested_element_list[MAX_MSGPACK_COMMAND_LOOP * sizeof(int *)], uint8_t size_devices);

	DHT *dht1;
	DHT *dht2;
	void setDHT1(DHT *dht1, uint8_t dht_pin, uint8_t type);
	void setDHT2(DHT *dht2, uint8_t dht_pin, uint8_t type);
//    void setActuator(uint32_t actuator_id, bool state, uint16_t duration);

	char* getSensor1();
	char* getSensor2();
	bool execute();

private:
	LocalBuffers *localBuffers;
	Responses *response;
	DHT getDHT1();
	DHT getDHT2();
	MachineState **machineState;
	CommandStruct command_struct;
	_4BCPMapElement *args_list[];

};

#endif
