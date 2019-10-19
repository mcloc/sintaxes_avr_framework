/**
 * This is NOT a oficial messagePack full implementation. It's implementation goes
 * @ the END of our appliance needs. In this case we are using a new Layer 7 (Application Layer)
 * on TOP of MessagePack protocol layer.
 *
 * We call it "4Bytes COMMAND PROTOCOL" (4BCP). It's composed of only 32bit unsigned long (0xffff0000 ~ 0xfffffff)
 * Note that first 2 bytes still reserved for improving the communications of 4Bytes CmdProtocol.
 *
 * This Layer is almost a implementation of RPC but built with a flow of execution designed to
 * execute commands at real time on processing message, with exceptions thrown @ real time trough TCP
 * client stream as JSON formated and rollback support with previous state guarded on SD Card.
 * We can response JSON because all strings are stored @ flash memory with PROGMEM, so it's
 * quite simple to make a JSON response. But parse JSON requires too much memory
 * and this framework is based on the minimal memory pattern (designed for 2KB RAM), with no malloc()
 * no string pointers (except the FlashStringPointers* which is a powerful tool that allows
 * snprintf_P() formated strings with run time values or other FlashStringPointer*).
 * So once again the most efficient way to send requests to 8bit controller devices (2KB RAM)
 * is MSGPACK with 4Bytes COMMAND PROTOCOL on top of it, acting like a RPC
 *
 * commands_map.h, devices.h, is the main essentials headers which #defines all mapping for this
 * appliance at per se. So on the other hand, one whose going to consume and operate this appliance
 * must have the same commands and devices mapped, defined, so the 4bytes CmdProtocol can work properly.
 *
 * Don't worry on any request breaking your sketch this implementation have taken care of unmapped
 * commands, the only problem is to check the maps defines, maybe we should hash the contents of the map
 * in check if match to ensure we talking about the same things on both sides.
 *
 */

#include <Arduino.h>
#include <defines/commands_map.h>
#include <defines/devices.h>
#include <defines/errors_code.h>
#include <defines/module_string.h>
#include <defines/msgpack_defines.h>
#include <msgpack/MsgPackDataTypes.h>
#include <4BCProtocol/4BCPElement.h>
#include <MachineState.h>
#include <MsgPackHandler.h>
#include <stdlib.h>
//TODO: sintax-framework namespace
//namespace sintax-iot-framework{
//
//}

//static class members must be declared also in cpp.
static const uint8_t MsgPackHandler::MSGPACK4BCPProcessFlow[MSGPACK4BCPProcessFlow_SIZE] PROGMEM
= {
MSGPACK_STATE_IDLE,
MSGPACK_STATE_BEGIN,
MSGPACK_STATE_COMMAND_SET,
MSGPACK_STATE_COMMAND_SETTING_ARGS,
MSGPACK_STATE_COMMAND_WATING_ARG_VALUE,
MSGPACK_STATE_COMMAND_ARGS_READY,
MSGPACK_STATE_COMMAND_ASSEMBLY,
MSGPACK_STATE_COMMAND_EXECUTED,
MSGPACK_STATE_COMMAND_FINISHED };

static const uint8_t MsgPackHandler::MSGPACK4BCPProcessFlow2[MSGPACK4BCPProcessFlow2_SIZE] PROGMEM
= {
MSGPACK_STATE_IDLE,
MSGPACK_STATE_BEGIN,
MSGPACK_STATE_COMMAND_SET,
MSGPACK_STATE_COMMAND_ASSEMBLY,
MSGPACK_STATE_COMMAND_EXECUTED,
MSGPACK_STATE_COMMAND_FINISHED };

/**
 * we will need Response Object to write on TCP client JSON objects and Commands to execute them on the fly.
 */
MsgPackHandler::MsgPackHandler(Responses *_responses, Commands *_commands,
		SintaxesLib *_sintaxes_lib) {
	response = _responses;
	commands = _commands;
	sintaxesLib = _sintaxes_lib;

//	element4BCP_1 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//	element4BCP_2 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//	element4BCP_3 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//	element4BCP_4 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//	element4BCP_5 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//	element4BCP_6 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//	element4BCP_7 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//	element4BCP_8 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//
//
//	value_1 = (_4BCPElementValue *)malloc(sizeof(_4BCPElementValue));
//	value_2 = (_4BCPElementValue *)malloc(sizeof(_4BCPElementValue));
//	value_3 = (_4BCPElementValue *)malloc(sizeof(_4BCPElementValue));
//	value_4 = (_4BCPElementValue *)malloc(sizeof(_4BCPElementValue));
//	value_5 = (_4BCPElementValue *)malloc(sizeof(_4BCPElementValue));
//	value_6 = (_4BCPElementValue *)malloc(sizeof(_4BCPElementValue));

	setStatus(MSGPACK_STATE_IDLE);
}

/**
 * reset module communication and get the request buffer. The size of the buffer is defined in
 * sintaxes-framework-defines.h and safe guard is taken care on main.cpp main() function
 * avoiding overflow. BEAWARE all settings is defined for minimal RAM usage to fit into 2KB,
 * if using bigger RAM you can increase the size of the request buffer
 * in sintaxes-framework-defines.h
 */
bool MsgPackHandler::init(Stream *_stream, int size,
		MachineState *_machine_state) {
	if (!setStatus(MSGPACK_STATE_BEGIN)) {
		response->writeError_on_INIT();
		response_headers_already_sent = true;
		response_headers_code = 500;
		return false;
	}

	reset_32bit_processing();
	buffer_position = 0;
	stream = _stream;
	buffer_lenght = stream->readBytes(LocalBuffers::client_request_buffer,
			size);
	buffer_bytes_remaining = buffer_lenght;

	machine_state = _machine_state;

	//IF BUFFER LEN == 0 ERROR NO MSG must post with no readers messagePack with the devices ptrocol
	response->writeModule200DataHeaders();
	response->initJsonResponse();
	response_headers_already_sent = true;
	response_headers_code = 200;

	return true;
}

/**
 * MAIN FUNCTION ON THE PROCESSING, IS THE DESERALIZATION
 *
 * REMIND: 	ALL EXECUTION ON THE STREAM IS @ RTIME, FROM HERE TO THERE
 * 			YOU MUST REMIND THAT RETURNS CAN BE ALWAYS FALSE
 * 			AND IF FALSE  YOUT MUST RETURN FALSE SO THE MAIN.C main() FUNC CAN KNOW
 * 			WHAT TODO. THE EXCEPTIONS MESSAGES ARE THRONW REAL TIME ON THE TCP STREAMS
 * 			AS JSON FORMATED
 */

/**
 * ON COMMAND FRAMEWORK PROTOCOL WILL ALWAYS BE LOOKING FOR 32BIT UNSINED LONG
 * THE INTERPRETATION OF THE 'RPC' IS ALL BASED ON 4BYTES CONSTANTS;
 * THE 4BYTES CONSTANTS COMMAND PROTOCOL IS DEFINED IN OTHERS FILES SUCH commands_map.h
 * AND devices.h WHICH SHOULD BE WRITTEN SPECIFIC per APPLIANCE et se, FOLLOWING THE
 * 4BCP GUIDELINES.
 *
 *
 * if it is'nt array or map (which was checked before this method beeing called),
 * then it should be the 4Bytes command protocol, like a COMMAND_FLAG a COMMAND per se,
 * known mapped ARGUMENTS or even some argument values for the COMMAND call.
 *
 * 	1 - unpack 4Bytes Commends Protocol (4BCP)
 * 	2 - look for arguments values for 4BPC
 * 	3 - report error on 4Bytes Commands Protocol with rollback action
 * 		to previous state guarded on SD card by returning false so method processStream() can return false as well
 * 		and let to main() function do what's necessary on processing fail
 */

bool MsgPackHandler::processStream() {
	while (buffer_bytes_remaining > 0) {
		//Get next byte on buffer
		uint8_t _byte = next();
//		response->writeByte(_byte);

		uint8_t array_size = 0;
		uint8_t map_elements_size = 0;
		/**
		 * If it's an array or a map the LAYER 7 4B COMMANDS FRAMEWORK (RPC)
		 * is already been processed, remember that this machine is suppose
		 * to interpret commands as defined per use et per si.
		 */
		//THIS IS AN ARRAY - [COMMANDS FRAMEWORK] STATUS ALREAY PROCESSING
		array_size = isArray(_byte);
		if (array_size > 0) {
			//TODO: not implemented
			error_code = ERROR_MSGPACK_UNIMPLEMENTED;
			response->writeMsgPackUnimplemented(_byte);
			response->closeJsonResponse();
			setStatus(MSGPACK_STATE_IDLE);
			return false;
		}

		/**
		 * 4BCP determines that it's message should be encapsulated inside a msgpack MAP (0x80 ~ 0x8f)
		 * Have a look at processMap() comments to have the idea of how we translate 4BCP messages.
		 * It's the main function for processing 4BCP messages. Remember that 4BCP Messages is sent
		 * on top of msgpack protocol
		 */
		map_elements_size = isMap(_byte);
		if (map_elements_size > 0) {
			if (!assembleMap(_byte, map_elements_size)) {
				response->closeJsonResponse();
				setStatus(MSGPACK_STATE_IDLE);
				return false;
			}

			if (!processMap()) {
				response->closeJsonResponse();
				setStatus(MSGPACK_STATE_IDLE);
				return false;
			}

		} else {
			error_code = ERROR_MAL_FORMED_MSGPCK;
			response->write4BCPMalFormedRequest(_byte, status);
			response->closeJsonResponse();
			setStatus(MSGPACK_STATE_IDLE);
			return false;
		}

		/**
		 * ON COMMAND FRAMEWORK PROTOCOL WILL ALWAYS BE LOOKING FOR 32BIT UNSINED LONG
		 * THE INTERPRETATION OF THE 'RPC' IS ALL BASED ON 4BYTES CONSTANTS;
		 * THE 4BYTES CONSTANTS COMMAND PROTOCOL IS DEFINED IN ANOTHER FILE;
		 * if it is'nt array or map or the 4bytes command protocol it must be
		 * some argument value for the COMMAND call.
		 *
		 * This switch is intent to:
		 * 	1 - unpack 4bytes Commends Protocol
		 * 	2 - look for arguments values for 4bytes Commands Protocol
		 * 	3 - report error on 4bytes commands Protocol with rollback action
		 * 		for the previous state guarded on SD card
		 */
//		if(!processByte(_byte)) {
//			response->writeMsgPackProcessingFlowStatus(status, next_status, prev_status);//DEBUG
//			return false;
//		}
		if (status == MSGPACK_STATE_COMMAND_EXECUTED) {
			if (buffer_bytes_remaining > 0) {
				error_code =
				ERROR_MSGPACK_4BCP_IN_FINISHED_STATE_WITH_REMAINING_BYTES;
				response->writeErrorMsgPackHasFinishedWithBytes();
				response->closeJsonResponse();
				setStatus(MSGPACK_STATE_IDLE);
				return false;
			}
			if (!setStatus(MSGPACK_STATE_COMMAND_FINISHED)) {
				response->closeJsonResponse();
				return false;
			}
			break;
		}
	}

	//END OF EXEUTION OF COMMANDS ON THE FLY WITH RESPECTIVES RESPONSES DONE
	//NOTHING MORE TO DO JUST RETURN true TO MAIN FUNCTION SAVE THE MACHINE STATE ON SD Card
	//AND ALL 4BCP FOOTPRINT OF THIS REQUEST FOR  LOG PROPOUSES

	//return after execution of each command with status and guard the new machine state on SD Card
	//we will use SD Card as log. So must implement methods for getting this log if requested
	if (status == MSGPACK_STATE_COMMAND_FINISHED) {
		if (!setStatus(MSGPACK_STATE_IDLE)) {
			response->closeJsonResponse();
			return false;
		}

		return true;
	} else {
		error_code = ERROR_MSGPACK_NOT_IN_FINISHED_STATE;
		response->writeErrorMsgPackHasNotFinishedStatus();
		response->closeJsonResponse();
		setStatus(MSGPACK_STATE_IDLE);
		return false;
	}
}

//		case MODULE_COMMMAND_EXECUTE_FLAG: {
//			uint8_t _byte = next();
//			if(!_byte == MSGPACK_TRUE){
//				error_code = ERROR_MSGPACK_4BCP_EXECUTE_FLAG;
//				response->writeErrorMsgPack4BCPExecuteFlagError();
//				setStatus(MSGPACK_STATE_COMMAND_FINISHED);
//				return false;
//			}

/**
 *	processMap() is a state machine based processor. Every package must be encoded with 4BCP protocol which defines
 *	that always will be sent in the request a msgpack format MAP with 1 (one) and only ONE type of command per request.
 *	Each command can be executed multiple times with different devices, p.e. one can send a request to SET different actuators
 *	with different arguments values. The command is the same 'MODULE_COMMMAND_SET_ACTUATOR' and it can be repeated for
 *	multiples actuators devices (which is mapped by 4BCP) with different values. Or it can be a simples GET command with no
 *	arguments at all, p.e. 'MODULE_COMMMAND_GET_DATA', 'MODULE_COMMMAND_GET_STATE', 'MODULE_COMMAND_GET_PROCESS_FLOW'.
 *
 *	The main effort is to understand 4BCP, which is described in our wiki. But for a glance here's a simple example of 4BCP structure
 *	message on top of msgpack protocol.
 *
 *
 * Our PHP test client pack the 4BCP on top of msgpack protocol. Just have in mind that the main goal of 4BCP is to have a lot
 * of mapping devices, sensors, commands, in 32bit unsigned long hex words constants, avoid String parsing for processing requests.
 *
 *
 * $payload = $packer->pack([
 *     MODULE_COMMMAND_FLAG => MODULE_COMMMAND_SET_ACTUATOR,
 *     MODULE_ACTUATOR_DN20_1_1 => array(
 *       MODULE_COMMMAND_SET_ARGS1 => true,
 *       MODULE_COMMMAND_SET_ARGS2 => 57.88
 *     ),
 *     MODULE_ACTUATOR_DN20_1_2 => array(
 *       MODULE_COMMMAND_SET_ARGS1 => false,
 *       MODULE_COMMMAND_SET_ARGS2 => 788633
 *     ),
 *     MODULE_ACTUATOR_DN20_1_3 => array(
 *       MODULE_COMMMAND_SET_ARGS1 => true,
 *       MODULE_COMMMAND_SET_ARGS2 => 788.63
 *     ),
 *     MODULE_COMMMAND_EXECUTE_FLAG => true
 *   ]
 *	);
 *
 *	Note that 4BCP is constants defined based. Each constant has a unique uint32bit_t hex value and it's used to process RPC's
 *	In the example above, the first thing you'll notice is that the first tuple is the COMMAND definition. The key tells
 *	the processor that it's value is a command, so it will look on commands_map.h to check if it's implemented.
 *
 *	IT'S MANDATORY THAT FIRST TUPLE BE THE COMMAND SET INSTRUCTIONS
 *
 *	In this case the command is SET ACTUATORS values. Since we can have several actuators in one module, each must have a
 *	unique signature which is the 4BCP uint32_t hex value defined in 'devices.h'. This message tell us that the command is
 *	MODULE_COMMMAND_SET_ACTUATOR and the other tuples is the definition of each actuator that must run the command with
 *	another msgpack MAP containing the actuators set values. The max number of commands to execute and the maximum numbers
 *	of arguments is defined in 'sintaxes-framework-defines.h' For example we can set a command to execute the maximum of 4 times,
 *	and the maximum of 4 arguments per command.This is defined in the constants 'MAX_MSGPACK_COMMANDS' and 'MAX_MSGPACK_ARGS'
 *
 *	On the case of our example for set actuators we defined another 3 tuples with the actuators UUID as key for the arguments MAP
 *	of this actuator itself. The number of arguments must be defined per COMMAND function. In this case only 2. 1 - if it
 *	should be ON/OFF (true/false) and 2 - the time it must be on that state. There will be soon a watchdog for proposes like this.
 *
 *	A simpler request could be to get the sensors data of this module like this:
 *
 * $payload = $packer->pack([
 *     MODULE_COMMMAND_FLAG => MODULE_COMMMAND_GET_DATA,
 *     MODULE_COMMMAND_EXECUTE_FLAG => true
 *   ]
 * );
 *
 * As before, first tuple is to set command, in this case 'MODULE_COMMMAND_GET_DATA'. Note that the same
 * way as before the last tuple must be MODULE_COMMMAND_EXECUTE_FLAG 4BCP constant with value true.
 *
 * This last tuple is used by the processor to tell the module RUN COMMAND, everything has been set.
 *
 * Be aware that we still have to process the msgpack protocol for arguments values which can be any of msgpack types.
 *
 */
bool MsgPackHandler::assembleMap(uint8_t _byte, uint8_t map_elements_size) {

	uint8_t counter = 0;
	//While there're tuples to process
	while (status != MSGPACK_STATE_COMMAND_ARGS_READY
			|| counter > MAX_MSGPACK_NESTED_ELEMENTS) {
		response->writeRaw(F("-----------------------"));
		response->writeRaw(F("first map_elements_size"));
		response->writeByte(map_elements_size);

		/**
		 * status  MSGPACK_STATE_BEGIN time to set command
		 */
		//SET Command with first element of MAP which is mandatory to be COMMAND_FLAG WITH VALUE AS COMMAND_TO_EXECUTE
		if (status == MSGPACK_STATE_BEGIN) {
			//process the first tuple (COMMAND SET)
			processStatusMSGPACK_STATE_BEGIN();
			//Two elements from MAP goes on commandHeader, The COMMAND_FLAG and the COMMAND et si.
			map_elements_size--;
			map_elements_size--;
			continue;
		}

		//This will be the first tuple after COMMAND SET (command definition tuples, like devices and devices arguments)
		//p.e. device MODULE_ACTUATOR_DN20_1_1 is a element which contains for example 2 nested elements, one for status (ON/OFF)
		//and other for time period of the state. Each loop in this while can be considered a device that will be set or read.
//		_4BCPMapElement *element;

		/**
		 * status  MSGPACK_STATE_COMMAND_SET time to set arguments and it's values
		 */
		//Time to get ARGS for COMMAND. as 4BCP explicit, beginning with the COMMAND_ARG_TYPE it self, it can be an actuator for instance
		if (status == MSGPACK_STATE_COMMAND_SET) {

			_4BCPMapElement **e =  processStatusMSGPACK_STATE_COMMAND_SET();
			response->writeRaw(F("element key before putting in MainMAP"));
			response->write32bitByte((*e)->key);

			_4BCPElement::map4BCP.elements[element4BCP_number] = (*e);

			//				memcpy(_4BCPElement::map4BCP.elements[element4BCP_number], element, sizeof(_4BCPMapElement *));
			//ACTUATORS KEY MAP inside it will have a pointer to another struct same type for arguments
			//				memcpy(map4BCP.elements[element4BCP_number], element, sizeof(_4BCPMapElement));
			//				memcpy(map4BCP.elements[element4BCP_number].nested_elements[nested_element4BCP], &element, sizeof(_4BCPMapElement));
			_4BCPElement::map4BCP.size++;
			continue;
			//we can't continue we are setting the element and we must set the element values
		}

		response->writeRaw(
				F("Antes do segundo IF, verifique o key do element:"));
		response->write32bitByte(
				_4BCPElement::map4BCP.elements[element4BCP_number]->key);
		response->writeRaw(F("Map size"));
		response->write32bitByte(_4BCPElement::map4BCP.size);

//		DEBUG
		return false;

		/**
		 * status  MSGPACK_STATE_COMMAND_SETTING_ARGS must hae while status different commands args ready
		 */
		//We already have the key on tuple for executing the command
		// MAP     MAP_ELEMTNS					NESTED ELEMENT 					VALUE
		//map4BCP.elements[element4BCP_number].nested_elements[nested_element4BCP].value
		// MAP		DN20_1						ARGUMENT1						VALUE
		//map4BCP.elements[element4BCP_number].nested_elements[nested_element4BCP++].value
		// MAP		DN20_1						ARGUMENT2						VALUE
		// AFTER ALL ARGUMENTS OF THE element4BCP_number INCRESE THE map4BCP.elements[element4BCP_number++] FOR THE NEXT DEVICE
		// FOR EXAMPLE DN20_2
		//So a MAP have a list of DEVICES elements to SET. And Devices elements have a list of ARGUMENTS_MAPPED and it's value
		while ((status == MSGPACK_STATE_COMMAND_SETTING_ARGS)
				|| (status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE)) {

			// 1- ITŚ A MAP, 2 - IT'S ANOTHER CE KEY FOR ARGUMENT, 3 - IT'S ANOTHER DEVICE ELEMENT KEY _32bitword
			//get next msgpack data type which should the key of a MAP of arguments value
			_byte = getNextType();
			response->writeRaw(
					F("NEXT IN MSGPACK_STATE_COMMAND_SETTING_ARGS (2o. if):"));
			response->writeByte(_byte);

			uint8_t map_size = isMap(_byte);

			//NOT MAP  this is the nested tuple with arguments value of the outside tuple which is the UUID of device to be set.
			if (!(map_size > 0)) {

				//				get argument key uint32_t
				_byte = getNextType();
				if (!assemble_uint32_Byte(_byte))
					return false;
				//
				//				response->writeRaw(F("nextByte _32bitword:"));
				//				response->write32bitByte(_32bitword);
				//Max number of nested elements into one element, which means by 4BCP Max number of executing elements (devices)
				if (nested_element4BCP
						+ element4BCP_number>= MAX_MSGPACK_NESTED_ELEMENTS) {
					error_code =
					ERROR_MSGPACK_4BCP_NESTED_ELEMENTS_OUT_OF_BOUNDS;
					response->write4BCPNestedElementsOutOfBound();
					return false;
				}

				_4BCPMapElement **nested_element;
				nested_element = setElementPointer(true);
				//Acording to definition MAX_MSGPACK_NESTED_ELEMENTS in this case 8 elements maximum
				_byte = getNextType();
				(*nested_element)->value_type = _byte;
				setElementValue(&(*nested_element));



				_4BCPElement::map4BCP.elements[element4BCP_number]->nested_elements[nested_element4BCP] = (*nested_element);
				_4BCPElement::map4BCP.elements[element4BCP_number]->total_nested_elements++;

				response->writeRaw(F("NESTED key element:"));
				response->write32bitByte(
						_4BCPElement::map4BCP.elements[element4BCP_number]->nested_elements[nested_element4BCP]->key);
				response->writeRaw(F("Total Nested Element for this element:"));
				response->write32bitByte(
						_4BCPElement::map4BCP.elements[element4BCP_number]->total_nested_elements);

				nested_element4BCP++;

				//HERE THE NESTED_ELEMENT IF FULLY SET. TIME TO INCREMENT nested_element

				//Now it's supposed to come the arguments of this device
				//				_byte = getNextType();
				//				map_size = isMap(_byte);
				//				response->writeRaw(F("NEXT IN MSGPACK_STATE_COMMAND_SETTING_ARGS (2o. if) after setting element value, get next():"));
				//				response->writeByte(_byte);

				//				if(map_size > 0)
				//					map_elements_size = map_size;

				//				element->nested_elements[element4BCP_number] = nested_element;
				//				memcpy((_4BCPMapElement *)_4BCPMap  nested_elements[element4BCP_number], &nested_element, sizeof(_4BCPMapElement));
				//				map4BCP.elements[element4BCP_number].nested_elements = nested_element;
				//				memcpy(&map4BCP.elements[element4BCP_number].nested_elements, &element, sizeof(_4BCPMapElement));
				//				element4BCP_number++;
				continue;
			}

			//This is each new arguments map() on the msgpack generally 0x81 0x82 0x83 this are the values for SETTING Actuators p.e.
			while (map_size > 0) {
				_4BCPMapElement **nested_element;

				if (!setStatus(MSGPACK_STATE_COMMAND_WATING_ARG_VALUE))
					return false;

				response->writeRaw(F("----------SSSSSSS---------"));
				response->writeRaw(F("TOTAL ELEMENTS AND NESTED :"));
				response->writeByte(element4BCP_number);
				response->writeByte(nested_element4BCP);
				response->writeRaw(F("ELEMENT pointer:"));
				response->writeByte(total_element4BCP);
				response->writeRaw(F("----------SSSSSSS---------"));

				//Max number of nested elements into one element, which means by 4BCP Max number of executing elements (devices)
				if (nested_element4BCP
						+ element4BCP_number>= MAX_MSGPACK_NESTED_ELEMENTS) {
					error_code =
					ERROR_MSGPACK_4BCP_NESTED_ELEMENTS_OUT_OF_BOUNDS;
					response->write4BCPNestedElementsOutOfBound();
					return false;
				}

				nested_element = setElementPointer(true);
//				_4BCPMapElement value_element;
				_byte = getNextType();

				response->writeRaw(F("nextByte:"));
				response->writeByte(_byte);

//				if(_byte == MSGPACK_UINT32){
//					if(!assemble_uint32_Byte(_byte))
//						return false;
//				}

				(*nested_element)->value_type = _byte;
				setElementValue(nested_element);

				//Max number of nested elements into one element, which means by 4BCP Max number of executing elements (devices)
				if (element4BCP_number >= MAX_MSGPACK_NESTED_ELEMENTS) {
					error_code =
					ERROR_MSGPACK_4BCP_NESTED_ELEMENTS_OUT_OF_BOUNDS;
					response->write4BCPNestedElementsOutOfBound();
					return false;
				}
//				element->nested_elements[element4BCP_number] = &nested_element;
				_4BCPElement::map4BCP.elements[element4BCP_number]->nested_elements[nested_element4BCP] =(*nested_element);
						;
				_4BCPElement::map4BCP.elements[element4BCP_number]->total_nested_elements++;

				response->writeRaw(F("NESTED key element:"));
				response->write32bitByte(
						_4BCPElement::map4BCP.elements[element4BCP_number]->nested_elements[nested_element4BCP]->key);
				response->writeRaw(F("NESTED value_type element:"));
				response->write32bitByte(
						_4BCPElement::map4BCP.elements[element4BCP_number]->nested_elements[nested_element4BCP]->value_type);
				response->writeRaw(F("Total Nested Element for this element:"));
				response->write32bitByte(
						_4BCPElement::map4BCP.elements[element4BCP_number]->total_nested_elements);

				//Check if next element is a Map
				if (!(whatNext() > MSGPACK_MAP_INITIAL
						&& whatNext() <= MSGPACK_MAP_FINAL)) {
					if (!setStatus(MSGPACK_STATE_COMMAND_SETTING_ARGS))
						return false;
				} else {
					//NOT MAP
//					_byte = next();
////					map_size = isMap(_byte);
//					continue;
				}

				map_size--;
				response->writeRaw(F("######################"));
			} // END of WHILE arguments MAP of a device key

			//go for the next argument of the outside element p.e. MODULE_ACTUATOR_DN20_1_1
			element4BCP_number++;
		} // END if(status == MSGPACK_STATE_COMMAND_SETTING_ARGS )


//		map4BCP.elements[element4BCP_number] = element4BCP_main;
//		response->writeRaw(F("element4BCP_number:"));
//		response->writeByte(element4BCP_number);
		counter++;

		// it's awaiting the arg MAP
		if (status == MSGPACK_STATE_COMMAND_SETTING_ARGS) {

		} // END of WHILE arguments MAP of a device key

		//First device tuple is all set, go for the next tuple (device) p.e. MODULE_ACTUATOR_DN20_1_2, MODULE_ACTUATOR_DN20_1_3
		//back status to MSGPACK_STATE_COMMAND_ARGS_READY so the loop can roll again for other devices that must be set.
		if (!setStatus(MSGPACK_STATE_COMMAND_ARGS_READY)) //now the status should be execute command
			return false;

//	element4BCP_number++;
//	map_elements_size--;

		response->writeRaw(F("+++++++++++++++++++++++"));

		response->writeRaw(F("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"));
		response->writeRaw(F("map4BCP size:"));
		response->writeByte(_4BCPElement::map4BCP.size);
		response->writeRaw(F("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"));

		//now the status should be execute command
//	if(!setStatus(MSGPACK_STATE_COMMAND_ARGS_READY))
//		return false;

		return true;
	}
}

bool MsgPackHandler::processStatusMSGPACK_STATE_BEGIN() {
	uint8_t _byte = next();
	if (!processCommandHeader(_byte))
		return false;

	return true;

}

_4BCPMapElement** MsgPackHandler::processStatusMSGPACK_STATE_COMMAND_SET() {

	_4BCPMapElement **element;

	//GET FIRST DEVICE KEY (ACTUATOR) TO ASSEMBLE THE FIRST ELEMENT;
	//This should be 4BCP key (0xce)
	uint8_t _byte = getNextType();
	response->writeRaw(F("NEXT IN MSGPACK_STATE_COMMAND_SET (1o. if)"));
	response->writeByte(_byte);
	//get uint32_t key for this element
	if (!assemble_uint32_Byte(_byte))
		return false;

	//Check if word if mapped by this module
	if (checkModulesMap()) {
		//now we ready to set arguments for this device, p.e. MODULE_ACTUATOR_DN20_1_1
		if (!setStatus(MSGPACK_STATE_COMMAND_SETTING_ARGS))
			return false;

		//THIS SET THE CORRENT POINTER TO ELEMENT OF THE ALREADY INSTATIATED STRUCTS, FOR NOW WE HAVE MAXIMUM OF 8 ELEMENTS
		element = setElementPointer();

		//set element key for example the UUID of the actuator to be set: MODULE_ACTUATOR_DN20_1_1
//				memcpy(&element->key, &_32bitword, sizeof(uint32_t));
		(*element)->key = _32bitword;
		(*element)->value_type = MSGPACK_UINT32;

//		response->writeRaw(F("Command:"));
//		response->write32bitByte(commands->command_executing);
//		response->writeRaw(F("ELEMENT KEY:"));
//		response->write32bitByte(element->key);

	} else {
		//TODO: error
//		error_code = ERROR_MSGPACK_4BCP_NOT_MAPPED;
//		response->writeErrorMsgPackHasFinishedWithBytes();
//		response->closeJsonResponse();
		return false;
	}

	return element;

}

bool MsgPackHandler::processMap() {
	//check both MSGPACK4BCPProcessFlow with arguments without arguments
	if (status != MSGPACK_STATE_COMMAND_ARGS_READY
			&& status != MSGPACK_STATE_COMMAND_SET) {
		//TODO:
		error_code = ERROR_MSGPACK_4BCP_PROCESSING_FLOW;
		response->writeMsgPackProcessingFlowError(status, next_status,
				prev_status);
		return false;
	}
	if (!(_4BCPElement::map4BCP.size > 0)) {
		//TODO:
		error_code = ERROR_MSGPACK_4BCP_MAP_ZERO_ELEMENTS;
		response->writeErrorMsgPack4BCPZeroElementMap();
		return false;
	}

	if (!setStatus(MSGPACK_STATE_COMMAND_ASSEMBLY))
		return false;

	//NOW IT's the time to get Devices, must get a element key which is suppoused to be
	//a device and trasverse MachineState actuators_list to match same and set it's values
	//TODO: set Command object and execute it
//	for(uint8_t i = 0; i < map4BCP.size;i++){
////		_4BCPMapElement element = map4BCP.elements[i];
//
//		//nested elements
//		if(isMap(element.value_type)){
//			for(uint8_t j = 0; j < element.total_nested_elements; j++){
//				_4BCPMapElement *nested_element = element.nested_elements[j];
////				response->writeRaw(F("nested_element key:"));
////				response->write32bitByte(nested_element->key);
////				response->writeRaw(F("element type:"));
////				response->writeByte(nested_element->value_type);
////				response->writeRaw(F("total nested elements:"));
////				response->writeByte(nested_element->total_nested_elements);
//			}
////			_4BCPMapElement nested_element = element.;
//		} else {
////			response->writeRaw(F("element key:"));
////			response->write32bitByte(element.key);
////			response->writeRaw(F("element type:"));
////			response->writeByte(element.value_type);
////			response->writeRaw(F("total nested elements:"));
////			response->writeByte(element.total_nested_elements);
//		}
//	}

	return true;
}

_4BCPMapElement** MsgPackHandler::setElementPointer(bool add_value = false) {
	_4BCPElementValue **element_value;
	switch (total_element4BCP) {
	case 0: {
		total_element4BCP++;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPElement::element4BCP_1->value = (_4BCPElementValue *) &element_value;
		}
		return &_4BCPElement::element4BCP_1;
		//		nested_element = _4BCPElement::element4BCP_1;
//		element4BCP_number++;
		break;
	}
	case 1: {
		total_element4BCP++;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPElement::element4BCP_1->value  = (_4BCPElementValue *) &element_value;
		}
		return &_4BCPElement::element4BCP_2;
//		element4BCP_number++;
		break;
	}
	case 2: {
		total_element4BCP++;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPElement::element4BCP_1->value  = (_4BCPElementValue *) &element_value;
		}
		return &_4BCPElement::element4BCP_3;
//		element4BCP_number++;
		break;
	}
	case 3: {
		total_element4BCP++;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPElement::element4BCP_1->value  = (_4BCPElementValue *) &element_value;
		}
		return &_4BCPElement::element4BCP_4;
//		element4BCP_number++;
		break;
	}
	case 4: {
		total_element4BCP++;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPElement::element4BCP_1->value  = (_4BCPElementValue *) &element_value;
		}
		return &_4BCPElement::element4BCP_5;
//		element4BCP_number++;
		break;
	}
	case 5: {
		total_element4BCP++;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPElement::element4BCP_1->value  = (_4BCPElementValue *) &element_value;
		}
		return &_4BCPElement::element4BCP_6;
//		element4BCP_number++;
		break;
	}
	case 6: {
		total_element4BCP++;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPElement::element4BCP_1->value  = (_4BCPElementValue *) &element_value;
		}
		return &_4BCPElement::element4BCP_7;
//		element4BCP_number++;
		break;
	}
	case 7: {
		total_element4BCP++;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPElement::element4BCP_1->value  = (_4BCPElementValue *) &element_value;
		}
		return &_4BCPElement::element4BCP_8;
//		element4BCP_number++;
		break;
	}
	case 8: {
		total_element4BCP++;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPElement::element4BCP_1->value  = (_4BCPElementValue *) &element_value;
		}
		return &_4BCPElement::element4BCP_9;
//		element4BCP_number++;
		break;
	}
	default: {
//		return   _4BCPElement::element4BCP_9;
		error_code = ERROR_MSGPACK_4BCP_NESTED_ELEMENTS_OUT_OF_BOUNDS;
		response->write4BCPNestedElementsOutOfBound();
		return false;
	}
	}
}

_4BCPElementValue ** MsgPackHandler::setStructElementValue() {
	switch (total_elementValue4BCP) {
	case 0: {
		total_elementValue4BCP++;
		return &_4BCPElement::value_1;
		break;
	}
	case 1: {
			total_elementValue4BCP++;
			return &_4BCPElement::value_2;
			break;
		}
	case 2: {
		total_elementValue4BCP++;
		return &_4BCPElement::value_3;
		break;
	}
	case 3: {
		total_elementValue4BCP++;
		return &_4BCPElement::value_4;
		break;
	}
	case 4: {
		total_elementValue4BCP++;
		return &_4BCPElement::value_5;
//		value_number++;
		break;
	}
	case 5: {
		total_elementValue4BCP++;
		return &_4BCPElement::value_6;
		break;
	}
	default: {
//		return   _4BCPElement::element4BCP_9;
		error_code = ERROR_MSGPACK_4BCP_NESTED_ELEMENTS_OUT_OF_BOUNDS;
		response->write4BCPNestedElementsOutOfBound();
//		return NULL;
	}
	}

}

bool MsgPackHandler::setElementValue(_4BCPMapElement **element) {
	//Change to function template each case can be a Template Specialization
	//https://www.geeksforgeeks.org/template-specialization-c/
	switch ((*element)->value_type) {
	case MSGPACK_NIL: {
		(*element)->value->bool_value = false;
		return true;
	}

	case MSGPACK_FALSE: {
		(*element)->value->bool_value = false;
		return true;
	}

	case MSGPACK_TRUE: {
		(*element)->value->bool_value = false;
		return true;
	}

	case MSGPACK_BIN8: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_BIN16: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_BIN32: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_EXT8: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_EXT16: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_EXT32: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_FLOAT32: {
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_FLOAT64: {
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_UINT8: {
		uint8_t _byte = next();
		(*element)->value->uint8_value = _byte;
		return true;
	}

	case MSGPACK_UINT16: {
		uint8_t _byte = next();
		uint16_t i = ((uint16_t) _byte) << 8;
		_byte = next();
		i += (uint16_t) _byte;
//			uint16_t *ptr = &i;
		(*element)->value->uint16_value = i;
//			memcpy(element->value, (uint16_t *)&ptr, sizeof(uint16_t)); // TODO: check this

		return true;
	}

	case MSGPACK_UINT32: {
		if (!assemble_uint32_Byte((*element)->value_type))
			return false;

		(*element)->value->uint32_value = _32bitword;
		return true;
	}

	case MSGPACK_UINT64: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_INT8: {
		int8_t _byte = next();
		(*element)->value->int8_value = _byte;
		return true;
	}

	case MSGPACK_INT16: {
		int8_t _byte = next();
		int16_t i = ((int16_t) _byte) << 8;
		_byte = next();
		i += (int16_t) _byte;

		(*element)->value->int16_value = i;
		return true;
	}

	case MSGPACK_INT32: {
		long int32word_array[4];
		long int32bitword;
		uint8_t int32_remaining = 4;
		while (int32_remaining > 0) {
			int8_t _byte = next();
			switch (int32_remaining) {
			case 4:
				int32word_array[0] = _byte;
				int32_remaining--;
				break;
			case 3:
				int32word_array[1] = _byte;
				int32_remaining--;
				break;
			case 2:
				int32word_array[2] = _byte;
				int32_remaining--;
				break;
			case 1:
				//The arithmetic should be performed with the 4 bytes at once. Does'nt work storing in the array bytes already shifted
				int32bitword = (int32word_array[0] << 24)
						+ (int32word_array[1] << 16) + (int32word_array[2] << 8)
						+ _byte;
				int32_remaining--;
				break;
			}
		}

//			int32_t *ptr = &int32bitword;
		(*element)->value->int16_value = int32bitword;
		return true;
	}

	case MSGPACK_INT64: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_FIXEXT1: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_FIXEXT2: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_FIXEXT4: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_FIXEXT8: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_FIXEXT16: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_STR8: {
		char _byte = next();
//			char *ptr = &_byte;

		(*element)->value->char_value = _byte;
//			memcpy((*element)->value, (char *)ptr, sizeof(char)); //TODO: check this

		return true;
	}

	case MSGPACK_STR16: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_STR32: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_ARRAY16: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_ARRAY32: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_MAP16: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	case MSGPACK_MAP32: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		response->writeMsgPackUnimplemented((*element)->value_type);
		return false;
	}

	default: {
		//MAP nested element
		if ((*element)->value_type > 0x80 && (*element)->value_type <= 0x8f)
			return true;
		//TODO:
		error_code = ERROR_MSGPACK_UNKNOW_TYPE;
		response->writeMsgPackUnknownType((*element)->value_type);
		return false;
	}
	}

	error_code = ERROR_MSGPACK_UNKNOW;
	response->writeMsgPackUnknowError();
	return false;
}

uint8_t MsgPackHandler::getNextType() {
	uint8_t _byte = next();

	if (!MsgPackDataTypes::checkDataType(_byte)) {
		//If it's not a recognized type on MsgPack definitions
		error_code = ERROR_MSGPACK_UNKNOW_TYPE;
		response->writeMsgPackUnknownType(_byte);
		return false;
	}

	return _byte;
}

bool MsgPackHandler::checkModulesMap() {
	//TODO loop into devices sensors actuators map

	//TODO: if( error )
//	error_code = ERROR_MSGPACK_4BCP_UNKNOW;
//	response->writeMsgPackProcessingFlowError(status, next_status, prev_status);
//	return false

	return true;
}

bool MsgPackHandler::processCommandHeader(uint8_t _byte) {
	//PROCESS KEY
	if (!MsgPackDataTypes::checkDataType(_byte)) {
		//If it's not a recognized type on MsgPack definitions
		error_code = ERROR_MSGPACK_UNKNOW_TYPE;
		response->writeMsgPackUnknownType(_byte);
		return false;
	}

	//get key uint32_t
	if (!assemble_uint32_Byte(_byte))
		return false;

	if (_32bitword != MODULE_COMMMAND_FLAG) {
		//TODO:
		error_code = ERROR_MSGPACK_4BCP_NO_COMMAND_FLAG;
		response->writeErrorMsgPack4BCPHasNoCommandFlag();
		return false;
	}

	_byte = next();
	if (!assemble_uint32_Byte(_byte))
		return false;

	commands->command_executing = _32bitword;
	if (!setStatus(MSGPACK_STATE_COMMAND_SET))
		return false;

	return true;
}

bool MsgPackHandler::processArray(uint8_t _word, uint8_t array_size) {
	//TODO
//	error_code = ERROR_MSGPACK_NOT_IMPLEMENTED;
//	response->writeMsgPackUnknownType(_byte);
	return false;//return false if/but all errors had been took care and reponses has already trasmitted
}

/**
 * BUFFER HANDLE's
 */
uint8_t MsgPackHandler::whatNext() {
	return LocalBuffers::client_request_buffer[buffer_position + 1];
}
uint8_t MsgPackHandler::next() {
	buffer_bytes_remaining--;
	//FIXME: last_byte has no function
	last_byte = LocalBuffers::client_request_buffer[buffer_position];
	buffer_position++;
	return last_byte;
}

/**
 * 		UINT8 var1 = 0x01; //0000 0001
 *		UINT8 var2 = 0x03; //0000 0011
 *		UINT8 var3 = 0x07; //0000 0111
 *		UINT8 var4 = 0x0F; //0000 1111
 *		UINT32 bigvar = (var1 << 24) + (var2 << 16) + (var3 << 8) + var4;
 */
bool MsgPackHandler::assemble_uint32_Byte(uint8_t _byte) {
	//IF NOT MSGPACK_UINT32 (unsigned long) MessaPack 0xce which is 4Bytes CmdProtocol relies
	if (_byte != MSGPACK_UINT32) {
		error_code = ERROR_32BIT_PROCESSING;
		response->writeProcess32bitwordERROR();
		return false;
	}

	//ensure that _32bitword and it's associated fields are reseted;
	reset_32bit_processing();

	uint8_t actual_status = status;
	status = MSGPACK_STATE_WORKING_32BIT;
	while (_32bitword_remaining > 0) {
		uint8_t _byte = next();
		switch (_32bitword_remaining) {
		case 4:
			_32bitword_array[0] = _byte;
			_32bitword_remaining--;
			break;
		case 3:
			_32bitword_array[1] = _byte;
			_32bitword_remaining--;
			break;
		case 2:
			_32bitword_array[2] = _byte;
			_32bitword_remaining--;
			break;
		case 1:
			//The arithmetic should be performed with the 4 bytes at once. Does'nt work storing in the array bytes already shifted
			_32bitword = (_32bitword_array[0] << 24)
					+ (_32bitword_array[1] << 16) + (_32bitword_array[2] << 8)
					+ _byte;
			_32bitword_remaining--;
			status = actual_status;	//return machine status to the previous status
//			response->write32bitByte(_32bitword); // DEBUG
			break;
		}
	}

	return true;
}

bool MsgPackHandler::reset_32bit_processing() {
	//This should never happen, only if your programming made some mistake on the PROGRAM STRUCTURE FLOW
	if (status == MSGPACK_STATE_WORKING_32BIT) {
		error_code = ERROR_32BIT_RESETING;
		response->writeReseting32bitwordERROR();
		return false;
	}

	_32bitword_remaining = 4; // 4 bytes for 32bit unsigned long
	_32bitword = '\0';
	_32bitword_array[0] = '\0';
	_32bitword_array[1] = '\0';
	_32bitword_array[2] = '\0';

	return true;
}

/**
 * HELPER METHODS
 */

bool MsgPackHandler::check4BCPProcesFlow(const uint8_t *msgPack4BCPProcessFlow,
		uint8_t array_size) {
	uint8_t last_process, actual_process;
	uint8_t position = 0;

	while (array_size > 0) {
		actual_process = pgm_read_word(&msgPack4BCPProcessFlow[position]);
//		response->writeRaw(F("LOOPING STATUS:"));
//		response->writeByte(actual_process);
//		response->writeRaw(F("PREV STATUS:"));
//		response->writeByte(prev_status);

		if (status == actual_process) {
			next_status = pgm_read_word(&msgPack4BCPProcessFlow[position] + 1); //next position
			//DEBUG
//			response->writeRaw(F("IDENTIFIED STATUS:"));
//			response->writeByte(actual_process);
//			response->writeRaw(F("PREV STATUS:"));
//			response->writeByte(prev_status);
//			response->writeRaw(F("NEXT STATUS:"));
//			response->writeByte(next_status); //DEBUG

			//LOOP for setting argument and waiting arg value. goes from 30 to 31 then 30 again to 31 then again until no more args is left
			if (status == MSGPACK_STATE_COMMAND_SETTING_ARGS
					&& prev_status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE
					&& next_status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE)
				return true;

			if (status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE
					&& prev_status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE
					&& next_status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE)
				return true;

//			if(status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE && prev_status ==  MSGPACK_STATE_COMMAND_SETTING_ARGS)
//				next_status = MSGPACK_STATE_COMMAND_ARGS_READY;

//			response->writeRaw(F("NEXT STATUS:"));
//			response->writeByte(next_status); //DEBUG

			if (last_process != NULL && last_process != prev_status) {
				error_code = ERROR_MSGPACK_4BCP_PROCESSING_FLOW;
				response->writeMsgPackProcessingFlowError(status, next_status,
						prev_status);
				return false;
			}
			break;
		}

		last_process = actual_process;
		array_size--;
		position++;
	}

	return true;
}

bool MsgPackHandler::checkFlow() {
	if (check4BCPProcesFlow(MSGPACK4BCPProcessFlow,
	MSGPACK4BCPProcessFlow_SIZE))
		return true;

	if (check4BCPProcesFlow(MSGPACK4BCPProcessFlow2,
	MSGPACK4BCPProcessFlow2_SIZE))
		return true;

	return false;
}

unsigned int MsgPackHandler::isArray(uint8_t _byte) {
	if (_byte > MSGPACK_ARRAY_INITIAL && _byte <= MSGPACK_ARRAY_FINAL) {
		return _byte - MSGPACK_ARRAY_INITIAL;
	}

	return 0;
}

unsigned int MsgPackHandler::isMap(uint8_t _byte) {
	if (_byte > MSGPACK_MAP_INITIAL && _byte <= MSGPACK_MAP_FINAL) {
		return _byte - MSGPACK_MAP_INITIAL;
	}

	return 0;
}

unsigned long MsgPackHandler::isMapped() {
	//TODO: map all defines into PROGMEM array or use #ifdef but by value
//	error_code = ERROR_MSGPACK_4BC_WORD_NOT_MAPPED;
//	response->write32bitByte(_32bitword);
//	response->writeRaw(F("processByte()->process4BytesCmdProtocol() false"));
//	response->writeMsgPackError(_32bitword);
	return _32bitword;			//DEBUG t needs to return the type of 4BCP
}

bool MsgPackHandler::setStatus(uint8_t _status) {
	prev_status = status;
	status = _status;

	if (_status == MSGPACK_STATE_IDLE)
		return true;

	if (!checkFlow())
		return false;

	return true;
}

/**
 * we wont use (at least try to) pointer to another pointer just for standards [&](){ cout << F(x)} ...
 */
//unsigned long MsgPackHandler::get32bitByte(){
//	return _32bitword;
//}
