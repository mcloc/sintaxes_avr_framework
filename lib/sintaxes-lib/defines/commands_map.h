/**
 * This file should have the definitions of commands and it's arguments
 * A news protocol is being developed for message communication between
 * devices with less 2KB RAM.
 * 
 * It'ś built in top of MessagePack, but no strings allowed. That's why
 * we need definition of this constants on both sides, server appliance side
 * and embedded side. The embedded side receives the new 6Bytes-msgpack message
 * which is able to receive maps and array of maps all based on this constants.
 * 
 * Both side must share the same constants.
 * 
 * All constants get packed as uint32_t which is 8bytes, so for a convenience
 * functions arguments can use 6 bytes of a 8 bytes words, since until 0xff is
 * reserved for messagePack itself. 
 * 
 * All commands are executed as per received data on the fly, so we need:
 *      1 - validate COMMAND it self;
 *      2 - validate and buffer ALL ARGS for each COMMAND it self, we can tune this by 
 *          increasing or decreasing the number of args BUFFER
 *      3 - one command is executed per time and response is being generated on the fly as well
 *          TODO: a response interruption to write the proper error response
 *      4 - All state of the ALL Module should be always update same time more then once on one
 *          loop() execution in a STATE BUFFER with msgpacked serialization
 *      5 - Before any execution save the STATE BUFFER as previous state so we can rollback if there's
 *          a command execution error
 *      6 - All COMMAND communication response must include the OLD STATE and the new STATE msgpack 
 * 
 * Besides messagePack this constants as uint32_t we should define them as hex
 * 
 * Author: Mario Caseiro <mcl.caseiro@gmail.com>
 * 
 */

/**
 * COMMANDS CAN GO FROM 				0xFFFF[0]001 to 0xFFFF[0]FFF
 * COMMANDS ARGS CAN GO FROM 			0xFFFF[F]001 to 0xFFFF[F]FFF
 * DEVICES SENSORS CAN GO FROM 			0xFFFF[1]001 to 0xFFFF[1]FFF
 * DEVICES CTUATORS CAN GO FROM 		0xFFFF[2]001 to 0xFFFF[2]FFF
 * MODULES ID CAN GO FROM		 		0xFFFF[3]001 to 0xFFFF[3]FFF
 */


#ifndef __MODULE_COMMANDS_MAP_H_
#define __MODULE_COMMANDS_MAP_H_

//COMMANDS by convention from 0xFFFF[0]001 to 0xFFFF[0]FFF
#define MODULE_COMMMAND_FLAG         	0xFFFF0001 // control byte to advice a command is comming
#define MODULE_COMMMAND_ARGS_FLAG    	0xFFFF0002 // control byte to advice a argument array is comming
#define MODULE_COMMMAND_EXECUTE_FLAG 	0xFFFF0013 // control byte to advice command to execute
// GOES UNTIL 0xFFFF0019 FOR CONTROLS WORDS

//GET COMMANDS FROM 0xFFFF[0]020 to 0xFFFF[0]8FF
#define MODULE_COMMMAND_GET_STATE   	0xFFFF0020
#define MODULE_COMMMAND_GET_DATA     	0xFFFF0021
#define MODULE_COMMAND_GET_PROCESS_FLOW 0xFFFF0022

//SET COMMANDS 0xFFFF[0]801	to 0xFFFF[0]EFF
#define MODULE_COMMMAND_SET_ACTUATOR 	0xFFFF0801

//COMMANDS ARGS by convention from 0xFFFF[F]001 to 0xFFFF[F]FFF
#define MODULE_COMMMAND_SET_ARGS1       0xFFFFF001
#define MODULE_COMMMAND_SET_ARGS2       0xFFFFF002
#define MODULE_COMMMAND_SET_ARGS3       0xFFFFF003
#define MODULE_COMMMAND_SET_ARGS4       0xFFFFF004
#define MODULE_COMMMAND_SET_ARGS5       0xFFFFF005
#define MODULE_COMMMAND_SET_ARGS6       0xFFFFF006
#define MODULE_COMMMAND_SET_ARGS7       0xFFFFF007
#define MODULE_COMMMAND_SET_ARGS8       0xFFFFF008




//Function arguments validation below

#include "avr/pgmspace.h"

/**
 * Defines the necessary arguments for each function
 * 
 */

const uint32_t command_set_actuator_required_args[2] PROGMEM = {
    MODULE_COMMMAND_SET_ARGS1,
    MODULE_COMMMAND_SET_ARGS2
};

#endif
