#ifndef __SINTAXES_MSGPACK_HANDLER_H_
#define __SINTAXES_MSGPACK_HANDLER_H_

#include <sintaxes-lib.h>
#include <msgpack_defines.h>

//msgPack limit this must complience with ext_command_args1 fields
#define MAX_MSGPACK_COMMANDS 3
#define MAX_MSGPACK_ARGS 4




class MsgPackHandler;

class MsgPackHandler {
public:
	MsgPackHandler(Responses *_response);
	void reset_32bit_processing();
	bool MsgPackHandler::init(Stream * _stream, int size);
	bool processByte(char * _byte);
	bool processStream();
	void process32bitBufferByte(uint8_t _byte);
	uint8_t whatForNext();
	uint8_t next();

private:
//	char * bytes_received;
	int buffer_lenght;
	int buffer_position;
	int buffer_bytes_remaining;
	char buffer_processsing_byte;
	uint8_t last_byte;
	Stream * stream;
	unsigned long _32bitword;
	uint8_t _32bitword_remaining = 4; // 4 8 bit bytes to achieve 32bits unsignedLong
	bool _32bitword_processing = false;
	unsigned long _32bitword_array[4];

	unsigned long ext_command;
	unsigned long ext_command_args1;
	unsigned long ext_command_args2;
	unsigned long ext_command_args3;
	unsigned long ext_command_args4;

//	auto command_args1;
//	auto command_args2;
//	auto command_args3;
//	auto command_args4;

	bool started = false;


	unsigned long ext_next_type;
	unsigned long ext_byte_type;
	unsigned long ext_byte;
	unsigned long ext_byte_stack[MAX_MSGPACK_COMMANDS + MAX_MSGPACK_ARGS];


	Responses *response;

};


#endif
