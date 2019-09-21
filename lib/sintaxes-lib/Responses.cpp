#include "sintaxes-lib.h"
#include "Responses.h"
#include "EchoServer.h"
#include "Commands.h"
#include "module_string.h"

Responses::Responses(Commands *_commands, BufferFiller *_bfill){
	commands = _commands;
	bfill = _bfill;
}

void Responses::setEchoServer(EchoServer *_echoServer){
	echoServer = _echoServer;
}

void Responses::setCommands(Commands *_commands){
	commands = _commands;
}

void Responses::writeModule200DataHeaders(){
	bfill->emit_p(header_response_200);
	bfill->emit_p(json_module_new_line);
	bfill->emit_p(header_content_type_json);
	bfill->emit_p(json_module_new_line);
	bfill->emit_p(header_server);
	bfill->emit_p(json_module_new_line);
	bfill->emit_p(header_connection); // the connection will be closed after completion of the response
	bfill->emit_p(json_module_new_line);
	bfill->emit_p(json_module_new_line);
	
}

void Responses::writeModuleDataResponse(){
	bfill->emit_p(header_response_200);
	bfill->emit_p(json_module_new_line);
	bfill->emit_p(header_content_type_json);
	bfill->emit_p(json_module_new_line);
	bfill->emit_p(header_server);
	bfill->emit_p(json_module_new_line);
	bfill->emit_p(header_connection); // the connection will be closed after completion of the response
	bfill->emit_p(json_module_new_line);
	bfill->emit_p(json_module_new_line);
	
	//begin the construction of Json
	bfill->emit_p(json_module_braces_open);
	//Message Header
	bfill->emit_p(json_module_name);
	bfill->emit_p(json_module_comma_separator);
	bfill->emit_p(json_module_uptime);
	bfill->emit_p(json_module_comma_separator);
	//Data Object
	bfill->emit_p(json_module_data_key);
	bfill->emit_p(json_module_braces_open);

	//TODO: loop trough sensors Array in the PROGMEM
	//Data key Sensors
	bfill->emit_p(json_module_sensors_key);
	bfill->emit_p(json_module_brackets_open);
	char *str;
	// DTH21#1 ouput
	
	//FIXME: how to get local buffer
	str = commands->getSensor1();
	bfill->emit_raw(str, strlen(str));
	bfill->emit_p(json_module_comma_separator);
	// DTH21#2 ouput
	str = commands->getSensor2();
	bfill->emit_raw(str, strlen(str));

	// close Sensors array
	bfill->emit_p(json_module_brackets_close);
	bfill->emit_p(json_module_comma_separator);

	//Data key Actuator
	bfill->emit_p(json_module_actuators_key);
	bfill->emit_p(json_module_brackets_open);
	bfill->emit_p(json_module_actuator1);
	bfill->emit_p(json_module_brackets_close);

	//Close data Object
	bfill->emit_p(json_module_braces_close);
	bfill->emit_p(json_module_comma_separator);
	bfill->emit_p(json_module_status);
	bfill->emit_p(json_module_comma_separator);

	//Errors data
	bfill->emit_p(json_module_error);


	//end the construction of Json
	bfill->emit_p(json_module_braces_close);
	bfill->emit_p(json_module_new_line);
}
