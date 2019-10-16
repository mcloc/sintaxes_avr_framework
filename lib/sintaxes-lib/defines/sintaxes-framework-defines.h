/*
 * sintaxes-framework-defines.h
 *
 *  Created on: 22 de set de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_DEFINES_SINTAXES_FRAMEWORK_DEFINES_H_
#define LIB_SINTAXES_LIB_DEFINES_SINTAXES_FRAMEWORK_DEFINES_H_

#define MAX_ACTUATORS 8

// 180 bytes allowed in post get incomming requests
#define MAX_SIZE_ALLOWED_REQUEST 220

// 180 bytes for working with flash strings like snprintf() LocalBuffers::string_cpy_buffer
#define MAX_SIZE_ALLOWED_PROGMEM_STRING 220

#define MAX_SIZE_ALLOWED_COMMAND_ARGUMENT_STRING 50

//msgPack max per request. This limit this must at compliance with ext_command_args1 fields
//This aapliance we limitting 3 commands per request and 4 args per command
//Remember a COMMAND with its ARGS is each one per time as it cames on MsgPack 4Bytes CmdProtocol
//and Commands fields will be overwritten for each COMMAND execution
//If you increase this you must change the Commands class to support more or less arguments
#define MAX_MSGPACK_COMMAND_LOOP 4
#define MAX_MSGPACK_ARGS 4
#define MAX_MSGPACK_NESTED_ELEMENTS 8


#endif /* LIB_SINTAXES_LIB_DEFINES_SINTAXES_FRAMEWORK_DEFINES_H_ */
