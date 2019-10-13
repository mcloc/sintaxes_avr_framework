/*
 * msgpack_defines.h
 *
 *  Created on: 22 de set de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_DEFINES_MSGPACK_DEFINES_H_
#define LIB_SINTAXES_LIB_DEFINES_MSGPACK_DEFINES_H_

#define MSGPACK_NIL 		0xc0
#define MSGPACK_FALSE 		0xc2
#define MSGPACK_TRUE 		0xc3
#define MSGPACK_BIN8		0xc4
#define MSGPACK_BIN16		0xc5
#define MSGPACK_BIN32		0xc6
#define MSGPACK_EXT8		0xc7
#define MSGPACK_EXT16		0xc8
#define MSGPACK_EXT32		0xc9
#define MSGPACK_FLOAT32		0xca
#define MSGPACK_FLOAT64		0xcb
#define MSGPACK_UINT8		0xcc
#define MSGPACK_UINT16		0xcd
#define MSGPACK_UINT32		0xce
#define MSGPACK_UINT64		0xcf
#define MSGPACK_INT8		0xd0
#define MSGPACK_INT16		0xd1
#define MSGPACK_INT32		0xd2
#define MSGPACK_INT64		0xd3
#define MSGPACK_FIXEXT1		0xd4
#define MSGPACK_FIXEXT2		0xd5
#define MSGPACK_FIXEXT4		0xd6
#define MSGPACK_FIXEXT8		0xd7
#define MSGPACK_FIXEXT16	0xd8
#define MSGPACK_STR8		0xd9
#define MSGPACK_STR16		0xda
#define MSGPACK_STR32		0xdb
#define MSGPACK_ARRAY16		0xdc
#define MSGPACK_ARRAY32		0xdd
#define MSGPACK_MAP16		0xde
#define MSGPACK_MAP32		0xdf
#define MSGPACK_TOTAL_ELEMENTS 31

#define MSGPACK_MAP_INITIAL		0x80
#define MSGPACK_MAP_FINAL		0x8f
#define MSGPACK_ARRAY_INITIAL 	0x90
#define MSGPACK_ARRAY_FINAL		0x9f
#define MSGPACK_STR_INITIAL		0xa0
#define MSGPACK_STR_FINAL		0xbf
#define MSGPACK_FIXSTR_LEGNT	0xbf-0xa0

//#define MSGPACK_ARRAY[] = {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,\
//	0x8a,0x8b,0x8c,0x8d,0x8e,0x8f}
//#define MSGPACK_MAP[] = {0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x99,0x99,\
//	0x9a,0x9b,0x9c,0x9d,0x9e,0x9f}


#define MSGPACK_STATE_IDLE 					0x00
#define MSGPACK_STATE_BEGIN 				0x10
#define MSGPACK_STATE_COMMAND_SET			0x20
#define MSGPACK_STATE_COMMAND_SETTING_ARGS 	0x30
#define MSGPACK_STATE_COMMAND_WATING_ARG_VALUE 	0x31
#define MSGPACK_STATE_COMMAND_ARGS_READY 	0x40
#define MSGPACK_STATE_COMMAND_EXECUTING 	0x50
#define MSGPACK_STATE_COMMAND_EXECUTED 		0x60
#define MSGPACK_STATE_WORKING_64BIT			0x70
#define MSGPACK_STATE_WORKING_32BIT 		0x71
#define MSGPACK_STATE_WORKING_16BIT			0x72

#define MSGPACK_STATE_COMMAND_FINISHED 		0x80






#endif /* LIB_SINTAXES_LIB_DEFINES_MSGPACK_DEFINES_H_ */
