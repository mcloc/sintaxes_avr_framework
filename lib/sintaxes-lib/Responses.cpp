
 #include <UIPEthernet.h>
#include <Arduino.h>

#ifndef FSH
typedef const __FlashStringHelper* FSH;
#endif

#include <Responses.h>
#include <LocalBuffers.h>
#include <defines/module_string.h>
#include <defines/errors_code.h>
#include <memory/ApplianceMemmoryHandler.h>

Responses::Responses(){
	response_json_initiated = false;
}

void Responses::setClient(EthernetClient *_client){
	client = _client;
}

//void Responses::setReponseJsonInitiated(){
//	response_json_initiated = true;
//}

void Responses::writeProcess32bitwordERROR(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_32BIT_PROCESSING,  ERROR_32BIT_PROCESSING_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::writeReseting32bitwordERROR(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_32BIT_RESETING,  ERROR_32BIT_RESETING_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::write4BCPWordNotMappedERROR(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_MSGPACK_4BCP_WORD_NOT_MAPPED,  ERROR_MSGPACK_4BCP_WORD_NOT_MAPPED_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::write4BCPCommandExecutionERROR(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_COMMAND_EXECUTION_FAILED,  ERROR_COMMAND_EXECUTION_FAILED_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::write4BCPCommandArgsMissing(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_COMMAND_ARGS_MISSING,  ERROR_COMMAND_ARGS_MISSING_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::write4BCPUnknowCommand(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_COMMAND_EXECUTION_FAILED,  ERROR_COMMAND_EXECUTION_FAILED_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::writeMsgPackError(unsigned long _byte){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_MSGPACK_PROCESSING,  ERROR_MSGPACK_PROCESSING_STR, _byte);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::writeMsgPackUnknowError(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_MSGPACK_UNKNOW,  ERROR_MSGPACK_UNKNOW_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::writeMsgPackUnimplemented(uint8_t _byte){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error_msgpack_unimplemented), ERROR_MSGPACK_UNIMPLEMENTED, _byte);
	client->print(LocalBuffers::string_cpy_buffer);
}


void Responses::writeMsgPack4BCPMethodUnimplemented(uint32_t _byte){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error_msgpack_unimplemented), ERROR_MSGPACK_UNIMPLEMENTED, _byte);
	client->print(LocalBuffers::string_cpy_buffer);
}


void Responses::writeMsgPackProcessingFlowError(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_MSGPACK_4BCP_PROCESSING_FLOW,  ERROR_MSGPACK_4BCP_PROCESSING_FLOW_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::writeMsgPackProcessingFlowError(uint8_t status, uint8_t next, uint8_t prev){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error_msg_pack_flow), ERROR_MSGPACK_4BCP_PROCESSING_FLOW, prev, status, next);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::writeMsgPackProcessingFlowStatus(uint8_t status, uint8_t next, uint8_t prev){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_msg_pack_flow_status), prev, status, next);
	client->print(LocalBuffers::string_cpy_buffer);
}


void Responses::writeMsgPackProcessingMap(uint8_t status, uint8_t next, uint8_t prev){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error_msgpack_processing_map), ERROR_MSGPACK_4BCP_ELEMENT_PROCESSING, prev, status, next);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::writeErrorProcessingStream(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_PROCESSING_STREAM,  ERROR_PROCESSING_STREAM_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}


void Responses::writeMsgPackUnknownType(uint8_t _byte){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_msgpack_unknown_type), ERROR_MSGPACK_UNKNOW_TYPE, _byte);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::writeErrorMsgPack4BCPElementHasNoKey(uint8_t _byte){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_msgpack_hash_no_key), ERROR_MSGPACK_4BCP_ELEMENT_HAS_NO_KEY, _byte);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::writeErrorMsgPack4BCPElementKeyProcessing(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_MSGPACK_4BCP_ELEMENT_KEY_PROCESSING,  ERROR_MSGPACK_4BCP_ELEMENT_KEY_PROCESSING_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::writeErrorMsgPack4BCPHasNoCommandFlag(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_MSGPACK_4BCP_NO_COMMAND_FLAG,  ERROR_MSGPACK_4BCP_NO_COMMAND_FLAG_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}


void Responses::writeErrorMsgPack4BCPZeroElementMap(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_MSGPACK_4BCP_MAP_ZERO_ELEMENTS,  ERROR_MSGPACK_4BCP_MAP_ZERO_ELEMENTS_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}



void Responses::writeErrorMsgPackHasFinishedWithBytes(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_MSGPACK_4BCP_IN_FINISHED_STATE_WITH_REMAINING_BYTES,  ERROR_MSGPACK_4BCP_IN_FINISHED_STATE_WITH_REMAINING_BYTES_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::writeErrorMsgPackHasNotFinishedStatus(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_MSGPACK_NOT_IN_FINISHED_STATE,  ERROR_MSGPACK_NOT_IN_FINISHED_STATE_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::writeErrorMsgPack4BCPExecuteFlagError(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_MSGPACK_4BCP_EXECUTE_FLAG,  ERROR_MSGPACK_4BCP_EXECUTE_FLAG_STR);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::write4BCPUnknowError(uint8_t prev, uint8_t status, uint8_t next){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_4BCP_unknown_error), ERROR_MSGPACK_4BCP_UNKNOW,  prev, status, next);
	client->print(LocalBuffers::string_cpy_buffer);
}

void Responses::write4BCPMalFormedRequest(uint8_t byte,uint8_t status){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_4BCP_malformed_request_error), ERROR_MAL_FORMED_MSGPCK,  byte, status);
	client->print(LocalBuffers::string_cpy_buffer);
}


void Responses::write4BCPNestedElementsOutOfBound(){
	if(response_json_initiated)
		client->print(FSH(json_module_comma_separator));
	else
		response_json_initiated = true;
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_4BCP_nested_elements_out_of_bounds), ERROR_MSGPACK_4BCP_NESTED_ELEMENTS_OUT_OF_BOUNDS,  MAX_MSGPACK_4BCP_ELEMENTS);
	client->print(LocalBuffers::string_cpy_buffer);
}



//void Responses::writeDEBUG_INT(unsigned long byte){
//	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(DEBUG_INT), byte);
//	client->println(LocalBuffers::string_cpy_buffer);
//}
//
//void Responses::writeDEBUG_CHAR(__FlashStringHelper  *byte){
//	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(DEBUG_CHAR), byte);
//	client->print(LocalBuffers::string_cpy_buffer);
//}

void Responses::writeError_MAX_SIZE_REQUEST(){
	writeModule500DataHeaders();
	initJsonResponse();
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_REQUEST_MAX_LENGHT,  ERROR_REQUEST_MAX_LENGHT_STR);
	client->print(LocalBuffers::string_cpy_buffer);
	closeJsonResponse();
}

void Responses::writeError_MAL_FORMED_REQUEST(){
//	writeModule500DataHeaders();
	initJsonResponse();
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_MAL_FORMED_REQUEST,  ERROR_MAL_FORMED_REQUEST_STR);
	client->print(LocalBuffers::string_cpy_buffer);
	closeJsonResponse();
}

void Responses::writeError_on_INIT(){
	writeModule500DataHeaders();
	initJsonResponse();
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_INIT_MSGPACK,  ERROR_INIT_MSGPACK_STR);
	client->print(LocalBuffers::string_cpy_buffer);
	closeJsonResponse();
}



//void Responses::writeSTXError(){
//	writeModule500DataHeaders();
//	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_REQUEST_MISSING_STX,  REQUEST_MISSING_STX_ERROR_STR);
//	client->print(LocalBuffers::string_cpy_buffer);
//}

void Responses::writeRaw(char *byte){
	client->println(byte);
}

void Responses::writeRaw(const __FlashStringHelper * string){
	client->println(string);
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
	client->println(FSH(header_response_500));
	client->print(FSH(json_module_new_line));
	client->print(FSH(header_content_type_json));
	client->print(FSH(json_module_new_line));
	client->print(FSH(header_server));
	client->print(FSH(json_module_new_line));
	client->print(FSH(header_connection)); // the connection will be closed after completion of the response
	client->print(FSH(json_module_new_line));
	client->print(FSH(json_module_new_line));

}


void Responses::initJsonResponse(){
	client->print(FSH(json_module_braces_open));
	//Message Header
	client->print(FSH(json_module_name));
	client->print(FSH(json_module_comma_separator));
	client->print(FSH(json_module_uptime));
	client->print(FSH(json_module_comma_separator));
	//Data Object
	client->print(FSH(json_module_reponse_key));
	client->print(FSH(json_module_brackets_open));
//	client->print(FSH(json_module_braces_open));

	//Strings that need to be finished after JSON snippets responses was sent
	//this should call a finish JSON before closing connection.
	// "if response_json_finish_objects > 8;do client-print(FSH(response_json_finish_objects[i]
//	response_json_finish_objects[] += json_module_braces_close;
//	response_json_finish_objects[] += json_module_comma_separator;
//	response_json_finish_objects[] += json_module_status;
//	response_json_finish_objects[] += json_module_braces_close;
//	response_json_finish_objects[] += json_module_comma_separator
//	response_json_finish_objects[] += json_module_braces_close;

}

void Responses::closeJsonResponse(){
	client->print(FSH(json_module_brackets_close));
	client->print(FSH(json_module_braces_close));
	client->println();
}

void Responses::sendFullStatusData(char *sensor1_data, char *sensor2_data){
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

	// DTH21#1 ouput
	client->print(sensor1_data); //json object of the Sensor 1

	client->print(FSH(json_module_comma_separator));

	// DTH21#2 ouput
	client->print(sensor2_data); //json object of the Sensor 2

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

//	Errors data
	client->print(FSH(json_module_error_key));
	snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_error), ERROR_NONE,  ERROR_NONE_STR);
	client->print(LocalBuffers::string_cpy_buffer);

//	end the construction of Json
	client->print(FSH(json_module_braces_close));
	client->print(FSH(json_module_new_line));
}

