/**
 * This file should have the definitions of commands and it's arguments
 * A news protocol is beeing developed for message communication between
 * devices with le 2KB RAM.
 * 
 * Itś built in top of MessagePack, but no strings allowed. That's why
 * we need definition of this constants on both sides, server appliance side
 * and embedded side. The embedded side receives the new 6Bytes-msgpack message
 * which is able to receive maps and array of maps all based on this constants.
 * 
 * Bouth side must share the same constants.
 * 
 * All constants get packed as uint32_t wich is 8bytes, so for a convinience
 * functions arguments can use 6 bytes of a 8 bytes words, since until 0xff is
 * reserved for messagePack itself. 
 * 
 * All commands are executated as per received data on the fly, so we need:
 *      1 - validate COMMAND it self;
 *      2 - validate and buffer ALL ARGS for each COMMAND it self, we can tune this by 
 *          increasing or descrising the number of args BUFFER
 *      3 - one command is executed per time and response is beeing generated on the fly as well
 *          TODO: a response interruption to write the proper error response
 *      4 - All state of the ALL Module should be always update same time more then onece on one
 *          loop() execution in a STATE BUFFER with msgpacked serializaition
 *      5 - Befor any execution save the STATE BUFFER as previous state so we can rollback if there's 
 *          a comman execution error
 *      6 - All COMMAND communication response must include the OLD STATE and the new STATE msgpack 
 * 
 * TODO: verify if we can use the 6 or the all 8 bytes
 * 
 * Besides messagePack packa this constants as uint32_t we should define them as hex
 * 
 * Author: Mario Caseiro <mcl.caseiro@gmail.com>
 * 
 */



#ifndef __MODULE_COMMANDS_MAP_H_
#define __MODULE_COMMANDS_MAP_H_

//COMMANDS by convention from 0xFFFF0001 to 0xFFFF0FFF
#define MODULE_COMMMAND_FLAG         0xFFFF0001 // control byte to advice a command is comming
#define MODULE_COMMMAND_ARGS_FLAG    0xFFFF0002 // control byte to advice a argument array is comming
#define MODULE_COMMMAND_EXECUTE_FLAG 0xFFFFFF13 // control byte to advice command to execute
#define MODULE_COMMMAND_GET_STATE    0xFFFF0010
#define MODULE_COMMMAND_GET_DATA     0xFFFF0011
#define MODULE_COMMMAND_SET_ACTUATOR 0xFFFF0012

//COMMANDS ARGS by convention from 0xFFFFF001 to 0xFFFFFFFF
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
