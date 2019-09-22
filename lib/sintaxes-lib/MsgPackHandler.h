#ifndef __SINTAXES_MSGPACK_HANDLER_H_
#define __SINTAXES_MSGPACK_HANDLER_H_

#include <sintaxes-lib.h>

//msgPack limit this must complience with ext_command_args1 fields
#define MAX_MSGPACK_COMMANDS 3
#define MAX_MSGPACK_ARGS 4

class MsgPackHandler;

class MsgPackHandler {
public:
	MsgPackHandler();
	bool processByte(char * _byte);

private:
	char * byte_received;
	uint8_t bytes_remaining_for_32bit = 4; // 4 8 bit bytes to achieve 32bits unsignedLong
	unsigned long ext_command;
	unsigned long ext_command_args1;
	unsigned long ext_command_args2;
	unsigned long ext_command_args3;
	unsigned long ext_command_args4;

//	auto command_args1;
//	auto command_args2;
//	auto command_args3;
//	auto command_args4;


	unsigned long ext_next_type;
	unsigned long ext_byte_type;
	unsigned long ext_byte;
	unsigned long ext_byte_stack[MAX_MSGPACK_COMMANDS + MAX_MSGPACK_ARGS];
};


#endif
