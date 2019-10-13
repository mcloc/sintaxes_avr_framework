#ifndef __SINTAXES_MSGPACK_HANDLER_H_
#define __SINTAXES_MSGPACK_HANDLER_H_

#include <4BCProtocol/MsgPack4BCPMap.h>
#include <4BCProtocol/MsgPack4BCPMapElement.h>
#include <sintaxes-lib.h>
#include <Stream.h>
#include <Commands.h>
#include <Arduino.h>
#include <defines/msgpack_defines.h>
#include <defines/sintaxes-framework-defines.h>

#define MSGPACK4BCPProcessFlow_SIZE 	9
#define MSGPACK4BCPProcessFlow2_SIZE 	7

class MsgPackHandler;

class MsgPackHandler {
public:
	MsgPackHandler();
	MsgPackHandler(Responses *_response, Commands *commands, SintaxesLib *sintax_lib);
	bool init(Stream * _stream, int size);
	bool processStream();

private:
	Stream *stream;
	Responses *response;
	Commands *commands;
	SintaxesLib *sintaxesLib;

	uint8_t status;
	uint8_t prev_status;
	uint8_t next_status;
//	static uint8_t process_state_stack[]; //Pointer to array in the SD Card, LOG PROCESS, and LOG in Commands execution footprint

	unsigned int error_code;

	//TCP request buffer
	int buffer_lenght;
	int buffer_position;
	int buffer_bytes_remaining;
	char buffer_processsing_byte;

	uint8_t last_byte; // may be we'll use to know if we are inseide array, but i think other controll should be done for that

	//32bit word of the 4Bytes Command Protocol
	unsigned long _32bitword; // this buffer is utilized directly from inside methods, beaware to don't overwrite it
	uint8_t _32bitword_remaining = 4; // 4 8 bit bytes to achieve 32bits unsignedLong
	unsigned long _32bitword_array[4]; //4th index is the NULL terminator


	//msgpack values data type > 8bit
//	uint16_t _16bitword;



	unsigned int response_headers_code;
	bool response_headers_already_sent = false;



//	//4Bytes Command Protocol map
//	MsgPack4BCPMap map;
	//old_c_structs
	typedef struct _4BCPMapElement {
		uint8_t key;
		uint8_t value_type;
		uint8_t total_nested_elemtns = 0;
		void *value;
		_4BCPMapElement *nested_elements[];
	} _4BCPMapElement;
	typedef struct _4BCPMap {
		uint8_t size;
		_4BCPMapElement elements[];
	} _4BCPMap;

	_4BCPMap map4BCP;

	//4Bytes Command Protocol buffers
	unsigned long ext_command;
	unsigned long ext_command_args1;
	unsigned long ext_command_args2;
	unsigned long ext_command_args3;
	unsigned long ext_command_args4;



	//FIXME: PROGMEM is ignored due FIXED: PROGMEM is on cpp file
	 static const uint8_t MSGPACK4BCPProcessFlow[MSGPACK4BCPProcessFlow_SIZE];
	 static const uint8_t MSGPACK4BCPProcessFlow2[MSGPACK4BCPProcessFlow2_SIZE];
//	 static const uint8_t MSGPACK4BCPProcessFlow3[];
	 static const uint8_t MSGPACKTYPES[MSGPACK4BCPProcessFlow2_SIZE];

//
//	uint8_t length = sizeof(some_array) / sizeof(some_array[0]);
//
//	for (uint8_t i = 0; i < length; i++) {
//		Process(pgm_read_byte(&some_array[i]));
//	}


	uint8_t whatNext();
	uint8_t next();
	bool processArray(uint8_t _byte, uint8_t array_size);
	bool assembleMap(uint8_t _byte, uint8_t map_elements_size);
	bool processMap();
	bool processCommandHeader(uint8_t _word);
	bool checkModulesMap();
	uint8_t getNextType();
	bool setElementValue(_4BCPMapElement *element);

	bool check4BCPProcesFlow(const uint8_t *flow_array_ptr, uint8_t array_size);
	bool checkFlow();

	bool assemble_uint32_Byte(uint8_t _byte);
	bool assemble_uint16_Byte(uint8_t _byte);
	bool reset_32bit_processing();

	bool setStatus(uint8_t _status);
	unsigned int isArray(uint8_t _byte);
	unsigned int isMap(uint8_t _byte);
	unsigned long isMapped();

	/**
	 * we wont use pointer to another pointer (at least minimal when talking to other classes due to scope)
	 * just for standards [&](){ cout << F(x)} ...
	 */
	//unsigned long get32bitByte();


};




#endif
