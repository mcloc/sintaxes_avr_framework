#include <Arduino.h>
#include <commands/CommandsHandler.h>
#include <defines/commands_map.h>
#include <defines/devices.h>
#include <defines/module_string.h>
#include <LocalBuffers.h>
#include <Responses.h>
#include <commands/SetActuator.h>
//#include <StandardCplusplus.h>
//#include <sintaxes-lib.h>
/**
 * command to execute / in execution
 */
 uint32_t CommandsHandler::command_executing;

/**
 * uint32_t 8 bytes arguments for commands to be called
 */
// uint32_t CommandsHandler::command_argument1;
// uint32_t CommandsHandler::command_argument2;
// uint32_t CommandsHandler::command_argument3;
// uint32_t CommandsHandler::command_argument4;
// uint32_t CommandsHandler::command_argument5;
// uint32_t CommandsHandler::command_argument6;
// uint32_t CommandsHandler::command_argument7;
// uint32_t CommandsHandler::command_argument8;

CommandsHandler::CommandsHandler(LocalBuffers *_localBuffers, Responses *_response){
	localBuffers = _localBuffers;
	response = _response;
//	machineState = &_machine_state;
}


void CommandsHandler::setMachineState(MachineState ** _machine_state){
	machineState = _machine_state;

}


//void Commands::setActuator(uint32_t actuator_id, bool state, uint16_t duration){
//
//}

bool CommandsHandler::get_data(){
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

	response->sendFullStatusData(sensor1_data, sensor2_data);

	return true;

}

void CommandsHandler::setDHT1(DHT *_dht1, uint8_t dht_pin, uint8_t type){
    dht1 = _dht1;
    //RESET THE DHT#1 SENSOR
	digitalWrite(dht_pin, LOW); // sets output to gnd
	pinMode(dht_pin, OUTPUT); // switches power to DHT on
	delay(1200); // delay necessary after power up for DHT to stabilize
	(*dht1).begin();
}
void CommandsHandler::setDHT2(DHT *_dht2,uint8_t dht_pin, uint8_t type){
    dht2 = _dht2;
    //RESET THE DHT#2 SENSOR
	digitalWrite(dht_pin, LOW); // sets output to gnd
	pinMode(dht_pin, OUTPUT); // switches power to DHT on
	delay(1200); // delay necessary after power up for DHT to stabilize
    (*dht2).begin();
}

char *  CommandsHandler::getSensor1(){
    float readed_value = (*dht1).readHumidity();
    dtostrf(readed_value, 5, 2, localBuffers->float2char_buffer1);
    readed_value = (*dht1).readTemperature();
    dtostrf(readed_value, 5, 2, localBuffers->float2char_buffer2);
    snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_sensor1), localBuffers->float2char_buffer1, localBuffers->float2char_buffer2);
    return LocalBuffers::string_cpy_buffer;
}

char *  CommandsHandler::getSensor2(){
    float readed_value = (*dht2).readHumidity();
    dtostrf(readed_value, 5, 2, localBuffers->float2char_buffer1);
    readed_value = (*dht2).readTemperature();
    dtostrf(readed_value, 5, 2, localBuffers->float2char_buffer2);
    snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_sensor2), localBuffers->float2char_buffer1, localBuffers->float2char_buffer2);
    return LocalBuffers::string_cpy_buffer;
}

bool CommandsHandler::set_actuator(bool state, uint32_t duration){
//	CommandBase command_base = CommandBase::CommandBase(response, element->key, nested_element->key);
//	CommandBase ** cmd_execute = command_base.getCommandObj();
//	if((* cmd_execute)->execute()){
//		return false;
//	}

//
//	SetActuator command = SetActuator(response);
//	command.state = state;
//	command.state_duration = duration;
//
//	machineState->getActuator()
//
//
//	if (!command.execute()) {
////		error_code = ERROR_COMMAND_EXECUTION_FAILED;
//		response->write4BCPCommandExecutionERROR();
//		return false;
//	}

	return true;
}

bool CommandsHandler::execute(){

	switch(command_executing){
		case MODULE_COMMMAND_GET_DATA: {
			if(!get_data())
				return false;

			return true;
		}

//		case MODULE_COMMMAND_SET_ACTUATOR: {
//			if(!set_actuator())
//				return false;
//
//			return true;
//		}

		default:{
//			error_code = ERROR_MSGPACK_4BCP_UNKNOW_COMMAND;
			response->write4BCPUnknowCommand();
			return false;
		}
	}

	return false;
	//TODO 	GET THE ASSEMBLED COMMAND ON COMMANDS.buffers AND CAL THE COMMAND FUNCTION
	//		on the function execution send json with assignature of 4bytes command executing and
	//		all it's args
}
