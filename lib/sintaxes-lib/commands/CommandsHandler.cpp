#include <Arduino.h>
#include <commands/CommandsHandler.h>
#include <commands/SetActuatorCommand.h>
#include <defines/commands_map.h>
#include <defines/devices.h>
#include <defines/module_string.h>
#include <LocalBuffers.h>
#include <Responses.h>
#include <defines/sintaxes-framework-defines.h>
#include <memory/ApplianceMemmoryHandler.h>
#include <stdlib.h>
/**
 * command to execute / in execution
 */
uint32_t CommandsHandler::command_executing;
CommandStruct *CommandsHandler::command_struct = (CommandStruct*) malloc(sizeof(CommandStruct));

CommandsHandler::CommandsHandler() {
//	command_struct = (CommandStruct*) malloc(sizeof(CommandStruct)); //once allocated for ever
}

//void CommandsHandler::setMachineState(MachineState **_machine_state) {
//	machineState = _machine_state;
//}

bool CommandsHandler::initCommand() {
	command_struct->command = command_executing;
	command_struct->total_devices_executed = 0;
	return true;
}


void CommandsHandler::reset() {
	command_struct->command = '\0';
	command_struct->total_devices_executed = 0;


}



bool CommandsHandler::assembleCommand() {
//	command_struct->total_devices_executed = _4BCPContainer::map4BCP.size;
//	ApplianceMemmoryHandler::responses->writeRaw(F("assembleCommand() adding elements to command_struct"));
	for (uint8_t i = 0; i < _4BCPContainer::map4BCP.size; i++) {
		command_struct->devices_element_list[i] = _4BCPContainer::map4BCP.elements[i];
		//TODO this for is jsut to copy element_list to command_struct->devices_element_list  which will be used on execute
//		ApplianceMemmoryHandler::responses->writeRaw(F("KEY AND VALUES ON assembleCommand()"));
//		ApplianceMemmoryHandler::responses->write32bitByte(command_struct->devices_element_list[i]->key);
//		ApplianceMemmoryHandler::responses->write32bitByte(
//				command_struct->devices_element_list[i]->nested_elements[0]->value->bool_value);
//		ApplianceMemmoryHandler::responses->write32bitByte(
//				command_struct->devices_element_list[i]->nested_elements[1]->value->uint32_value);
//		ApplianceMemmoryHandler::responses->write32bitByte(command_struct.devices_element_list[i]->key);
	}

	return true;
}

//void Commands::setActuator(uint32_t actuator_id, bool state, uint16_t duration){
//
//}

bool CommandsHandler::get_data() {
	char sensor1_data[MAX_SIZE_ALLOWED_PROGMEM_STRING];
	char sensor2_data[MAX_SIZE_ALLOWED_PROGMEM_STRING];
	char *buffer;

//	// DTH21#1 ouput
	buffer = getSensor1();
//	uint8_t size = strlen(buffer);
	memmove(sensor1_data, buffer, MAX_SIZE_ALLOWED_PROGMEM_STRING);
	buffer = getSensor2();
//	// DTH21#2 ouput
	memmove(sensor2_data, buffer, MAX_SIZE_ALLOWED_PROGMEM_STRING);

	ApplianceMemmoryHandler::responses->sendFullStatusData(sensor1_data, sensor2_data);

//	free(buffer);
	return true;

}

void CommandsHandler::setDHT1(DHT *_dht1, uint8_t dht_pin, uint8_t type) {
	dht1 = _dht1;
	//RESET THE DHT#1 SENSOR
	digitalWrite(dht_pin, LOW); // sets output to gnd
	pinMode(dht_pin, OUTPUT); // switches power to DHT on
	delay(1200); // delay necessary after power up for DHT to stabilize
	(*dht1).begin();
}
void CommandsHandler::setDHT2(DHT *_dht2, uint8_t dht_pin, uint8_t type) {
	dht2 = _dht2;
	//RESET THE DHT#2 SENSOR
	digitalWrite(dht_pin, LOW); // sets output to gnd
	pinMode(dht_pin, OUTPUT); // switches power to DHT on
	delay(1200); // delay necessary after power up for DHT to stabilize
	(*dht2).begin();
}

char* CommandsHandler::getSensor1() {
	float readed_value = (*dht1).readHumidity();
	dtostrf(readed_value, 5, 2, LocalBuffers::float2char_buffer1);
	readed_value = (*dht1).readTemperature();
	dtostrf(readed_value, 5, 2, LocalBuffers::float2char_buffer2);
	snprintf_P(LocalBuffers::string_cpy_buffer,
			sizeof(LocalBuffers::string_cpy_buffer),
			(PGM_P) &(json_module_sensor1), LocalBuffers::float2char_buffer1,
			LocalBuffers::float2char_buffer2);
	return LocalBuffers::string_cpy_buffer;
}

char* CommandsHandler::getSensor2() {
	float readed_value = (*dht2).readHumidity();
	dtostrf(readed_value, 5, 2, LocalBuffers::float2char_buffer1);
	readed_value = (*dht2).readTemperature();
	dtostrf(readed_value, 5, 2, LocalBuffers::float2char_buffer2);
	snprintf_P(LocalBuffers::string_cpy_buffer,
			sizeof(LocalBuffers::string_cpy_buffer),
			(PGM_P) &(json_module_sensor2), LocalBuffers::float2char_buffer1,
			LocalBuffers::float2char_buffer2);
	return LocalBuffers::string_cpy_buffer;
}

bool CommandsHandler::set_actuator() {

//	CommandBase command_base = CommandBase::CommandBase(response, element->key, nested_element->key);
//	CommandBase ** cmd_execute = command_base.getCommandObj();
//	if((* cmd_execute)->execute()){
//		return false;
//	}
	uint32_t device_key;

	ApplianceMemmoryHandler::responses->writeRaw(F("inside EXECUTE:"));
	ApplianceMemmoryHandler::responses->writeRaw(F("inside EXECUTE command:"));
	ApplianceMemmoryHandler::responses->write32bitByte(command_executing);
	while (command_struct->total_devices_executed < _4BCPContainer::map4BCP.size) {


		device_key = command_struct->devices_element_list[command_struct->total_devices_executed]->key;
		ApplianceMemmoryHandler::responses->writeRaw(F("DEVICE KEY TO SET::"));
		ApplianceMemmoryHandler::responses->write32bitByte(device_key);

//		ApplianceMemmoryHandler::allocSetActuatorCommand(device_key);
//		SetActuatorCommand *actuator_command = (SetActuatorCommand*) malloc(sizeof(SetActuatorCommand));
//		actuator_command = &SetActuatorCommand(device_key);

		SetActuatorCommand actuator_command = SetActuatorCommand(device_key);

		ApplianceMemmoryHandler::responses->writeRaw(F("device_key just after created SetcActuatorObj(), out side of it"));
		ApplianceMemmoryHandler::responses->write32bitByte(actuator_command.device_key);


		//The problem is in machoneState; //FIXME:

		ApplianceMemmoryHandler::responses->writeRaw(F("machineState Actuator"));
		ApplianceMemmoryHandler::responses->write32bitByte((*ApplianceMemmoryHandler::machine_state->getActuator(0))->uuid);
		ApplianceMemmoryHandler::responses->writeRaw(F("--------------------------------------------"));


		actuator_command.state =
				command_struct->devices_element_list[command_struct->total_devices_executed]->nested_elements[0]->value->bool_value;
		actuator_command.state_duration =
				command_struct->devices_element_list[command_struct->total_devices_executed]->nested_elements[1]->value->uint32_value;

		ApplianceMemmoryHandler::responses->writeRaw(F("DEVICE STATE::"));
		ApplianceMemmoryHandler::responses->writeByte(actuator_command.state);
		ApplianceMemmoryHandler::responses->writeRaw(F("DEVICE DURATION STATE::"));
		ApplianceMemmoryHandler::responses->write32bitByte(actuator_command.state_duration);
		ApplianceMemmoryHandler::responses->writeRaw(
				F("JUST BEFORE SetActuator::EXECUTE()"));


		if (!actuator_command.execute()) { // machineState->getActuator(); and set actuator digital real values
			//		error_code = ERROR_COMMAND_EXECUTION_FAILED;
//			free(actuator_command);
			ApplianceMemmoryHandler::responses->write4BCPCommandExecutionERROR();
			return false;
		}


		//no
//		free(actuator_command);

								// mess up with the heap allocation map structure
//		delete actuator_command;
//		actuator_command->~SetActuator();
//		free(actuator_command);
		command_struct->total_devices_executed++;
	}

	return true;
}

bool CommandsHandler::execute() {

	switch (command_executing) {
	case MODULE_COMMMAND_GET_DATA: {
		if (!get_data())
			return false;

		return true;
	}

	case MODULE_COMMMAND_SET_ACTUATOR: {
		if (!set_actuator())
			return false;

		return true;
	}

	default: {
//			error_code = ERROR_MSGPACK_4BCP_UNKNOW_COMMAND;
		ApplianceMemmoryHandler::responses->write4BCPUnknowCommand();
		return false;
	}
	}

	return false;
	//TODO 	GET THE ASSEMBLED COMMAND ON COMMANDS.buffers AND CAL THE COMMAND FUNCTION
	//		on the function execution send json with assignature of 4bytes command executing and
	//		all it's args
}
