#ifndef __SINTAXES_MSGPACK_HANDLER_H_
#define __SINTAXES_MSGPACK_HANDLER_H_

#include <Stream.h>
#include <Commands.h>
#include <sintaxes-framework-defines.h>
#include <msgpack_defines.h>
#include <Arduino.h>


class MsgPackHandler;

class MsgPackHandler {
public:
	MsgPackHandler();
	MsgPackHandler(Responses *_response, Commands *commands);
	bool init(Stream * _stream, int size);
	bool processStream();

private:
	Stream *stream;
	Responses *response;
	Commands *commands;

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

	unsigned int response_headers_code;
	bool response_headers_already_sent = false;

	//4Bytes Command Protocol buffers
	unsigned long ext_command;
	unsigned long ext_command_args1;
	unsigned long ext_command_args2;
	unsigned long ext_command_args3;
	unsigned long ext_command_args4;

	//FIXME: PROGMEM is ignored due
	 static const uint8_t MSGPACK4BCPProcessFlow[];

//
//	uint8_t length = sizeof(some_array) / sizeof(some_array[0]);
//
//	for (uint8_t i = 0; i < length; i++) {
//		Process(pgm_read_byte(&some_array[i]));
//	}


	uint8_t whatNext();
	uint8_t next();
	bool processByte(uint8_t _byte);
	bool processArray(uint8_t _byte, int array_size);
	bool processMap(uint8_t _byte, int map_elements_size);
	bool check4BCPProcesFlow();

	bool assemble32bitByte(uint8_t _byte);
	bool reset_32bit_processing();
	unsigned long _4BCPCheckForNext(unsigned long resource);
	bool process4BytesCmdProtocol();
	bool processMappedResource(unsigned long resource);

	void setStatus(uint8_t _status);
	unsigned int isArray(uint8_t _byte);
	unsigned int isMap(uint8_t _byte);
	unsigned long isMapped();

	/**
	 * we wont use pointer to another pointer just for standards [&](){ cout << F(x)} ...
	 */
	//unsigned long get32bitByte();


//	auto command_args1;
//	auto command_args2;
//	auto command_args3;
//	auto command_args4;




//	unsigned long ext_next_type;
//	unsigned long ext_byte_type;
//	unsigned long ext_byte;
//	unsigned long ext_byte_stack[MAX_MSGPACK_COMMANDS + MAX_MSGPACK_ARGS];
};




#endif
