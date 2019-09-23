#include <sintaxes-lib.h>
#include "module_string.h"
#include "errors_code.h"
#include<stdio.h>

Responses::Responses(Commands *_commands, LocalBuffers *_local_buffer){
	commands = _commands;
	localBuffers = _local_buffer;
}

Responses::Responses(Commands *_commands){
	commands = _commands;
}

void Responses::setCommands(Commands *_commands){
	commands = _commands;
}

void Responses::setClient(EthernetClient *_client){
	client = _client;
}

void Responses::writeError_MAX_SIZE_REQUEST(){
	writeModule500DataHeaders();
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), REQUEST_MAX_LENGHT_ERROR,  REQUEST_MAX_LENGHT_ERROR_STR);
	client->print(LocalBuffers::string_cpy_buffer);

}

void Responses::writeSTXError(){
	writeModule500DataHeaders();
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), REQUEST_MISSING_STX_ERROR,  REQUEST_MISSING_STX_ERROR_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::writeByte(uint8_t byte){
	client->println(byte, HEX);
}

void Responses::write32bitByte(unsigned long _byte){
	client->println(_byte, HEX);
}

void Responses::writeModule200DataHeaders(){
	client->println(FSH(header_response_200));
	client->print(FSH(json_module_new_line));
	client->print(FSH(header_content_type_json));
	client->print(FSH(json_module_new_line));
	client->print(FSH(header_server));
	client->print(FSH(json_module_new_line));
	client->print(FSH(header_connection)); // the connection will be closed after completion of the response
	client->print(FSH(json_module_new_line));
	client->print(FSH(json_module_new_line));

}

void Responses::writeModule500DataHeaders(){
	client->println(FSH(header_response_200));
	client->print(FSH(json_module_new_line));
	client->print(FSH(header_content_type_json));
	client->print(FSH(json_module_new_line));
	client->print(FSH(header_server));
	client->print(FSH(json_module_new_line));
	client->print(FSH(header_connection)); // the connection will be closed after completion of the response
	client->print(FSH(json_module_new_line));
	client->print(FSH(json_module_new_line));

}

void Responses::writeModuleDataResponse(){
	writeModule200DataHeaders();

	//begin the construction of Json
	client->print(FSH(json_module_braces_open));
	//Message Header
	client->print(FSH(json_module_name));
	client->print(FSH(json_module_comma_separator));
	client->print(FSH(json_module_uptime));
	client->print(FSH(json_module_comma_separator));
	//Data Object
	client->print(FSH(json_module_data_key));
	client->print(FSH(json_module_braces_open));

	//TODO: loop trough sensors Array in the PROGMEM
	//Data key Sensors
	client->print(FSH(json_module_sensors_key));
	client->print(FSH(json_module_brackets_open));


	//FIXME: how to get rid of local buffer, this is due sensor need to get called sepratly
	// since everything uses the samebuffer LocalBuffers::string_cpy_buffer,
	char *str;
	// DTH21#1 ouput
	str = commands->getSensor1();
	client->print(str); //json object of the Sensor 1
	client->print(FSH(json_module_comma_separator));
	// DTH21#2 ouput
	str = commands->getSensor2();
	client->print(str); //json object of the Sensor 2

	// close Sensors array
	client->print(FSH(json_module_brackets_close));
	client->print(FSH(json_module_comma_separator));

	//Data key Actuator
	client->print(FSH(json_module_actuators_key));
	client->print(FSH(json_module_brackets_open));
	client->print(FSH(json_module_actuator1));
	client->print(FSH(json_module_brackets_close));

	//Close data Object
	client->print(FSH(json_module_braces_close));
	client->print(FSH(json_module_comma_separator));
	client->print(FSH(json_module_status));
	client->print(FSH(json_module_comma_separator));

	//Errors data
	client->print(FSH(json_module_error));


	//end the construction of Json
	client->print(FSH(json_module_braces_close));
	client->print(FSH(json_module_new_line));
}
