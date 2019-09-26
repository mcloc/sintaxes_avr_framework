/**
 * This file should have the definitions of sensors, actuators and it's arguments
 * A news protocol is beeing developed for message communication between
 * devices with le 2KB RAM.
 * 
 * Itś built in top of MessagePack, but no strings allowed. That's why
 * we need definition of this constants on bouth sides, server appliance side 
 * and embedded side. The embedded side receives the new 6Bytes-msgpack message
 * which is able to receive maps and array of maps all based on this constants.
 * 
 * Bouth side must share the same constants.
 * 
 * All constants get packed as uint32_t wich is 8bytes, so for a convinience
 * functions arguments can use 6 bytes of a 8 bytes words, since until 0xff is
 * reserved for messagePack itself. 
 * 
 * TODO: verify if we can use the 6 or the all 8 bytes
 * 
 * Besides messagePack packa this constants as uint32_t we should define them as hex
 * 
 * 
 * MODULE_ACTUATOR_<actuator_model+board+number>
 * for example:
 * 
 * #define MODULE_SENSOR_DTH21_1_1 0xFFFF1001
 * 
 * define MODLE_SENSOR DHT21 for board 1 and number 1 sensors with unique hex id
 * this case: 0xFFFF1001
 * 
 * 
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


#ifndef __MODULE_DEVIVES_H_
#define  __MODULE_DEVIVES_H_

//SENSORS by convention from   0xFFFF[1]001 to 0xFFFF[1]FFF
#define MODULE_SENSOR_DTH21_1_1 0xFFFF1001
#define MODULE_SENSOR_DTH21_1_2 0xFFFF1002
#define MODULE_SENSOR_DTH21_1_3 0xFFFF1003

//ACTUATORS by convention from 0xFFFF[2]001 to 0xFFFF[2]FFF
#define MODULE_ACTUATOR_DN20_1_1 0xFFFF2001
#define MODULE_ACTUATOR_DN20_1_2 0xFFFF2002
#define MODULE_ACTUATOR_DN20_1_3 0xFFFF2003

#endif
