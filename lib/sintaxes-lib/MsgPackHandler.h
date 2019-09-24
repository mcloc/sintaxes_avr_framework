#ifndef __SINTAXES_MSGPACK_HANDLER_H_
#define __SINTAXES_MSGPACK_HANDLER_H_

#include <Stream.h>
#include <Commands.h>
#include <sintaxes-framework-defines.h>





class MsgPackHandler;

class MsgPackHandler {
public:
	MsgPackHandler();
	MsgPackHandler(Responses *_response, Commands *commands);
	void reset_32bit_processing();
	bool init(Stream * _stream, int size);
	bool processByte(uint8_t _byte);
	bool processArray(uint8_t _byte, int array_size);
	bool process4BytesCmdProtocol();
	bool processMappedResource(unsigned long resource);
	unsigned long _4BCPCheckForNext();
	unsigned long get32bitByte();
	unsigned int isArray(uint8_t _byte);
	unsigned int isMap(uint8_t _byte);
	unsigned long isMapped();
	bool processStream();
	void assemble32bitByte(uint8_t _byte);
	uint8_t whatNext();
	uint8_t next();



private:
	Stream * stream;
	Responses *response;
	Commands *commands;


	uint8_t status;
	uint8_t prev_status;
	uint8_t process_state;

	unsigned int error_code;

	int buffer_lenght;
	int buffer_position;
	int buffer_bytes_remaining;
	char buffer_processsing_byte;
	uint8_t last_byte;
	unsigned long _32bitword;
	uint8_t _32bitword_remaining = 4; // 4 8 bit bytes to achieve 32bits unsignedLong
	bool _32bitword_processing = false;
	unsigned long _32bitword_array[4]; //4th index is the NULL terminator

	unsigned long ext_command;
	unsigned long ext_command_args1;
	unsigned long ext_command_args2;
	unsigned long ext_command_args3;
	unsigned long ext_command_args4;

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
