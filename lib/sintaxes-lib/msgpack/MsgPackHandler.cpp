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

#include <memory/ApplianceMemmoryHandler.h>
#include <4BCProtocol/4BCPContainer.h>
#include <Arduino.h>
#include <defines/commands_map.h>
#include <defines/devices.h>
#include <defines/errors_code.h>
#include <defines/module_string.h>
#include <defines/msgpack_defines.h>
#include <msgpack/MsgPackDataTypes.h>
#include <MachineState.h>
#include <MsgPackHandler.h>
#include <commands/CommandBase.h>
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
MSGPACK_STATE_COMMAND_ARGS_READY,
MSGPACK_STATE_COMMAND_ASSEMBLY,
MSGPACK_STATE_COMMAND_EXECUTING,
MSGPACK_STATE_COMMAND_FINISHED };

static const uint8_t MsgPackHandler::MSGPACK4BCPProcessFlow2[MSGPACK4BCPProcessFlow2_SIZE] PROGMEM
= {
MSGPACK_STATE_IDLE,
MSGPACK_STATE_BEGIN,
MSGPACK_STATE_COMMAND_SET,
MSGPACK_STATE_COMMAND_ASSEMBLY,
MSGPACK_STATE_COMMAND_EXECUTING,
MSGPACK_STATE_COMMAND_FINISHED };

/**
 * we will need Response Object to write on TCP client JSON objects and Commands to execute them on the fly.
 */
MsgPackHandler::MsgPackHandler() {
//	response = ApplianceMemmoryHandler::responses;
//	commands_handler = ApplianceMemmoryHandler::commands_handler;
//	sintaxesLib = ApplianceMemmoryHandler::sintaxes_lib;

	setStatus(MSGPACK_STATE_IDLE);
}

/**
 * reset module communication and get the request buffer. The size of the buffer is defined in
 * sintaxes-framework-defines.h and safe guard is taken care on main.cpp main() function
 * avoiding overflow. BEAWARE all settings is defined for minimal RAM usage to fit into 2KB,
 * if using bigger RAM you can increase the size of the request buffer
 * in sintaxes-framework-defines.h
 */
bool MsgPackHandler::init(Stream *_stream, int size) {
	if (!setStatus(MSGPACK_STATE_BEGIN)) {
		ApplianceMemmoryHandler::responses->writeError_on_INIT();
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

//	machine_state = ApplianceMemmoryHandler::machine_state;

//IF BUFFER LEN == 0 ERROR NO MSG must post with no readers messagePack with the devices ptrocol
	ApplianceMemmoryHandler::responses->writeModule200DataHeaders();
	ApplianceMemmoryHandler::responses->initJsonResponse();
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
	while (buffer_bytes_remaining > 0
			|| status != MSGPACK_STATE_COMMAND_FINISHED) {
		//Get next byte on buffer
		uint8_t _byte = next();
		ApplianceMemmoryHandler::responses->writeRaw(F("processStream nextByte:"));
		ApplianceMemmoryHandler::responses->writeByte(_byte);
		ApplianceMemmoryHandler::responses->writeRaw(F("processStream status:"));
		ApplianceMemmoryHandler::responses->writeByte(status);

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
			ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
					_byte);
			ApplianceMemmoryHandler::responses->closeJsonResponse();
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
				ApplianceMemmoryHandler::responses->closeJsonResponse();
				setStatus(MSGPACK_STATE_IDLE);
				return false;
			}

			ApplianceMemmoryHandler::responses->writeRaw(F("processStream calling assembleMap() ok. STATUS::"));
			ApplianceMemmoryHandler::responses->writeByte(status);
			continue;

		} else if (buffer_bytes_remaining == 0 || status == MSGPACK_STATE_COMMAND_ARGS_READY) {
			ApplianceMemmoryHandler::responses->writeRaw(F("processStream calling processMap():"));
			if (!processMap()) {
				ApplianceMemmoryHandler::responses->closeJsonResponse();
				setStatus(MSGPACK_STATE_IDLE);
				return false;
			}

		} else {
			ApplianceMemmoryHandler::responses->writeRaw(F("processStream else:"));
			return false;
			error_code = ERROR_MAL_FORMED_MSGPCK;
			ApplianceMemmoryHandler::responses->write4BCPMalFormedRequest(_byte,
					status);
			ApplianceMemmoryHandler::responses->closeJsonResponse();
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
//			 ApplianceMemmoryHandler::responses->writeMsgPackProcessingFlowStatus(status, next_status, prev_status);//DEBUG
//			return false;
//		}
		if (status == MSGPACK_STATE_COMMAND_EXECUTING) {
			if (buffer_bytes_remaining > 0) {
				error_code =
				ERROR_MSGPACK_4BCP_IN_FINISHED_STATE_WITH_REMAINING_BYTES;
				ApplianceMemmoryHandler::responses->writeErrorMsgPackHasFinishedWithBytes();
				ApplianceMemmoryHandler::responses->closeJsonResponse();
				setStatus(MSGPACK_STATE_IDLE);
				return false;
			}
			if (!setStatus(MSGPACK_STATE_COMMAND_FINISHED)) {
				ApplianceMemmoryHandler::responses->closeJsonResponse();
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
			ApplianceMemmoryHandler::responses->closeJsonResponse();
			return false;
		}

		return true;
	} else {
		error_code = ERROR_MSGPACK_NOT_IN_FINISHED_STATE;
		ApplianceMemmoryHandler::responses->writeErrorMsgPackHasNotFinishedStatus();
		ApplianceMemmoryHandler::responses->closeJsonResponse();
		setStatus(MSGPACK_STATE_IDLE);
		return false;
	}
}

//		case MODULE_COMMMAND_EXECUTE_FLAG: {
//			uint8_t _byte = next();
//			if(!_byte == MSGPACK_TRUE){
//				error_code = ERROR_MSGPACK_4BCP_EXECUTE_FLAG;
//				 ApplianceMemmoryHandler::responses->writeErrorMsgPack4BCPExecuteFlagError();
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
			|| counter > MAX_MSGPACK_4BCP_ELEMENTS) {

		/******************************************************************************
		 * status  MSGPACK_STATE_BEGIN time to set command
		 */
		//SET Command with first element of MAP which is mandatory to be COMMAND_FLAG WITH VALUE AS COMMAND_TO_EXECUTE
		if (status == MSGPACK_STATE_BEGIN) {
			//process the first tuple (COMMAND SET)
			processStatusMSGPACK_STATE_BEGIN();
			//Two elements from MAP goes on commandHeader, The COMMAND_FLAG and the COMMAND itself.
			map_elements_size--;
			continue;
		}



		/*****************************************************************************/

		//This will be the first tuple after COMMAND SET (command definition tuples, like devices and devices arguments)
		//p.e. device MODULE_ACTUATOR_DN20_1_1 is a element which contains for example 2 nested elements, one for status (ON/OFF)
		//and other for time period of the state. Each loop in this while can be considered a device that will be set or read.
//		_4BCPMapElement *element;
		/******************************************************************************
		 * status  MSGPACK_STATE_COMMAND_SET time to set arguments and it's values
		 */
		//Time to get ARGS for COMMAND. as 4BCP explicit, beginning with the COMMAND_ARG_TYPE it self, it can be an actuator for instance
		if (status == MSGPACK_STATE_COMMAND_SET) {

			_4BCPMapElement **e = processStatusNextElement();
//			 ApplianceMemmoryHandler::responses->writeRaw(F("COMMAND_SET element key before putting in MainMAP"));
//			 ApplianceMemmoryHandler::responses->write32bitByte((*e)->key);

			//FIRST DEICE ARGUMENT KEY
			_4BCPContainer::map4BCP.elements[element4BCP_number] = (*e);
			//now we ready to set arguments for this device, p.e. MODULE_ACTUATOR_DN20_1_1
			if (!setStatus(MSGPACK_STATE_COMMAND_SETTING_ARGS))
				return false;

			_4BCPContainer::map4BCP.size++;

//			 ApplianceMemmoryHandler::responses->writeRaw(F("COMMAND_SET element in MainMAP"));
//			 ApplianceMemmoryHandler::responses->write32bitByte(_4BCPContainer::map4BCP.elements[element4BCP_number]->key);

			// ELEMENT is READY to RECEVICE ARGS status = MSGPACK_STATE_COMMAND_SETTING_ARGS
			continue;
			//we can't continue we are setting the element and we must set the element values
		}
		/*****************************************************************************/

		/******************************************************************************
		 * status  MSGPACK_STATE_COMMAND_SETTING_ARGS must hae while status different commands args ready
		 *
		 *
		 * must get the key of device (actuator) and dispach to WAITING ARGS to set nested elements
		 *
		 */
		if (status == MSGPACK_STATE_COMMAND_SETTING_ARGS) {
			_byte = getNextType();

			// Check always if it's a map to allocate the nested elements
			uint8_t map_size = isMap(_byte);
			if (!(map_size > 0)) {
				//TODO: error
				error_code = ERROR_MSGPACK_4BCP_MAP_ZERO_ELEMENTS;
				ApplianceMemmoryHandler::responses->writeErrorMsgPack4BCPZeroElementMap();
				ApplianceMemmoryHandler::responses->closeJsonResponse();
				return false;
			}

			while (map_size > 0) {
				_byte = getNextType();
				if (_byte != MSGPACK_UINT32) {
					//TODO: error
					error_code = ERROR_MSGPACK_4BCP_ELEMENT_KEY_PROCESSING;
					ApplianceMemmoryHandler::responses->writeErrorMsgPack4BCPElementKeyProcessing();
					ApplianceMemmoryHandler::responses->closeJsonResponse();
					return false;
				}

				//KEY for NEXT ACTUATOR
				if (!assemble_uint32_Byte(_byte))
					return false;

				//Max number of nested elements into one element, which means by 4BCP Max number of executing elements (devices)
				if (total_element4BCP >= MAX_MSGPACK_4BCP_ELEMENTS) {
					error_code =
					ERROR_MSGPACK_4BCP_NESTED_ELEMENTS_OUT_OF_BOUNDS;
					ApplianceMemmoryHandler::responses->write4BCPNestedElementsOutOfBound();
					return false;
				}

				_4BCPMapElement **nested_element;
				nested_element = setElementPointer(true);

				_byte = getNextType();
				//Acording to definition MAX_MSGPACK_NESTED_ELEMENTS in this case 8 elements maximum
				(*nested_element)->key = _32bitword;
				(*nested_element)->value_type = _byte;

				//			 ApplianceMemmoryHandler::responses->writeRaw(F("_32bitword to assign key of device *element "));
				//			 ApplianceMemmoryHandler::responses->write32bitByte(_32bitword);

//				 ApplianceMemmoryHandler::responses->writeRaw(F("key of device *element "));
//				 ApplianceMemmoryHandler::responses->write32bitByte((*nested_element)->key);

				setElementValue(nested_element);

				_4BCPContainer::map4BCP.elements[element4BCP_number]->nested_elements[nested_element4BCP] =
						(*nested_element);
				_4BCPContainer::map4BCP.elements[element4BCP_number]->total_nested_elements =
						nested_element4BCP + 1;

				nested_element4BCP++;
				map_size--;
			}

			element4BCP_number++;
			nested_element4BCP = 0;

			_4BCPMapElement **e = processStatusNextElement();

			if ((*e)->key == MODULE_COMMMAND_EXECUTE_FLAG) {
				_byte = getNextType();
				if (_byte != MSGPACK_TRUE) {
					error_code = ERROR_MSGPACK_4BCP_EXECUTE_FLAG;
					ApplianceMemmoryHandler::responses->writeErrorMsgPack4BCPExecuteFlagError();
					return false;
				}

				if (!setStatus(MSGPACK_STATE_COMMAND_ARGS_READY))
					return false;

				//DONE ALL DEVICES AND ARGS ARE SET
				return true;
			}

			//OTHERS DEVICE ARGUMENT KEY
			_4BCPContainer::map4BCP.elements[element4BCP_number] = (*e);
			//now we ready to set arguments for this device, p.e. MODULE_ACTUATOR_DN20_1_1
//			if (!setStatus(MSGPACK_STATE_COMMAND_SETTING_ARGS))
//				return false;

			_4BCPContainer::map4BCP.size++;

			continue;

		}
		/*****************************************************************************/

//		//DEBUG3
//		return false;

		counter++;


	}

	ApplianceMemmoryHandler::responses->writeRaw(
			F("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"));
	ApplianceMemmoryHandler::responses->writeRaw(F("map4BCP size:"));
	ApplianceMemmoryHandler::responses->writeByte(_4BCPContainer::map4BCP.size);
	ApplianceMemmoryHandler::responses->writeRaw(
			F("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"));

	return true;
}

bool MsgPackHandler::processStatusMSGPACK_STATE_BEGIN() {
	uint8_t _byte = next();
	if (!processCommandHeader(_byte))
		return false;

	return true;

}

_4BCPMapElement** MsgPackHandler::processStatusNextElement() {

	_4BCPMapElement **element;

	//GET FIRST DEVICE KEY (ACTUATOR) TO ASSEMBLE THE FIRST ELEMENT;
	//This should be 4BCP key (0xce)
	uint8_t _byte = getNextType();

	//KEY for NEXT ACTUATOR
	if (!assemble_uint32_Byte(_byte))
		return NULL;

	//Check if word if mapped by this module
	if (checkModulesMap()) {

		//THIS SET THE CORRENT POINTER TO ELEMENT OF THE ALREADY INSTATIATED STRUCTS, FOR NOW WE HAVE MAXIMUM OF 8 ELEMENTS
		element = setElementPointer();

		//set element key for example the UUID of the actuator to be set: MODULE_ACTUATOR_DN20_1_1
//				memcpy(&element->key, &_32bitword, sizeof(uint32_t));
		(*element)->key = _32bitword;
		(*element)->value_type = MSGPACK_UINT32;

	} else {
		//TODO: error
//		error_code = ERROR_MSGPACK_4BCP_NOT_MAPPED;
//		 ApplianceMemmoryHandler::responses->writeErrorMsgPackHasFinishedWithBytes();
//		ApplianceMemmoryHandler::responses->closeJsonResponse();
		return NULL;
	}

//	 ApplianceMemmoryHandler::responses->writeRaw(F("dentro do processStatusMSG_STATE_COMMAND_SET key"));
//	 ApplianceMemmoryHandler::responses->write32bitByte((*element)->key);
//	 ApplianceMemmoryHandler::responses->writeByte(_32bitword);
//	 ApplianceMemmoryHandler::responses->writeRaw(F("dentro do processStatusMSG_STATE_COMMAND_SET value_type"));
//	 ApplianceMemmoryHandler::responses->writeByte((*element)->value_type);

	return element;

}

bool MsgPackHandler::processMap() {


//	ApplianceMemmoryHandler::responses->writeRaw(F("inside processMap status:"));
//	ApplianceMemmoryHandler::responses->writeByte(status);
//	return false;


//	ApplianceMemmoryHandler::responses->writeRaw(
//			F("Total BCP  elements instatiated"));
//	ApplianceMemmoryHandler::responses->writeByte(total_element4BCP);
//
//	ApplianceMemmoryHandler::responses->writeRaw(
//			F("How many elements in the array"));
//	ApplianceMemmoryHandler::responses->writeByte(element4BCP_number);



	//check both MSGPACK4BCPProcessFlow with arguments without arguments
	if (status != MSGPACK_STATE_COMMAND_ARGS_READY
			&& status != MSGPACK_STATE_COMMAND_SET
			&& status != MSGPACK_STATE_COMMAND_ASSEMBLY) {
		//TODO:
		error_code = ERROR_MSGPACK_4BCP_PROCESSING_FLOW;
		ApplianceMemmoryHandler::responses->writeMsgPackProcessingFlowError(
				status, next_status, prev_status);
		return false;
	}
	if (!(_4BCPContainer::map4BCP.size > 0)) {
		//TODO:
		error_code = ERROR_MSGPACK_4BCP_MAP_ZERO_ELEMENTS;
		ApplianceMemmoryHandler::responses->writeErrorMsgPack4BCPZeroElementMap();
		return false;
	}

	if (!setStatus(MSGPACK_STATE_COMMAND_ASSEMBLY))
		return false;

//	ApplianceMemmoryHandler::responses->writeRaw(F("BYTES REMAINING:"));
//	ApplianceMemmoryHandler::responses->writeByte(buffer_bytes_remaining); // must be zeto it's FF why FIXME:
//
//
//
//	ApplianceMemmoryHandler::responses->writeRaw(F("Calling commands_handler init"));
	ApplianceMemmoryHandler::commands_handler->initCommand();

	//NOW IT's the time to get Devices, must get a element key which is suppoused to be
	//a device and trasverse MachineState actuators_list to match same and set it's values
	//TODO: set Command object and execute it

//	_4BCPMapElement ** elements_ptr = & _4BCPContainer::map4BCP.elements;
	//process all devices for this command;
	ApplianceMemmoryHandler::commands_handler->assembleCommand();
	if (!setStatus(MSGPACK_STATE_COMMAND_EXECUTING))
		return false;

	ApplianceMemmoryHandler::commands_handler->execute(); // execute one by one in a loop

	if (!setStatus(MSGPACK_STATE_COMMAND_FINISHED))
		return false;

	return true;
}

_4BCPMapElement** MsgPackHandler::setElementPointer(bool add_value) {

//	ApplianceMemmoryHandler::responses->writeRaw(F("DEBUG in setElementPointer"));
//	return false; //DEBUG

	_4BCPElementValue **element_value;
	switch (total_element4BCP) {
	case 0: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_1->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_1;
		//		nested_element = _4BCPElement::element4BCP_1;
//		element4BCP_number++;
		break;
	}
	case 1: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_2->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_2;
//		element4BCP_number++;
		break;
	}
	case 2: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_3->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_3;
//		element4BCP_number++;
		break;
	}
	case 3: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_4->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_4;
//		element4BCP_number++;
		break;
	}
	case 4: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_5->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_5;
//		element4BCP_number++;
		break;
	}
	case 5: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_6->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_6;
//		element4BCP_number++;
		break;
	}
	case 6: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_7->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_7;
//		element4BCP_number++;
		break;
	}
	case 7: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_8->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_8;
//		element4BCP_number++;
		break;
	}
	case 8: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_9->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_9;
		break;
	}
	case 9: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_10->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_10;
		break;
	}
	case 10: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_11->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_11;
		break;
	}
	case 11: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_12->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_12;
		break;
	}
	case 12: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_13->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_13;
		break;
	}
	case 13: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_14->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_14;
		break;
	}
	case 14: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_15->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_15;
		break;
	}
	case 15: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_16->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_16;
		break;
	}
	case 16: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_17->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_17;
		break;
	}
	case 17: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_18->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_18;
		break;
	}
	case 18: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_19->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_19;
		break;
	}
	case 19: {
		total_element4BCP++;
		elements_remaining--;
		if (add_value) {
			element_value = setStructElementValue();
			_4BCPContainer::element4BCP_20->value = (*element_value);
		}
		return &_4BCPContainer::element4BCP_20;
		break;
	}
	default: {
//		return   _4BCPElement::element4BCP_9;
		error_code = ERROR_MSGPACK_4BCP_NESTED_ELEMENTS_OUT_OF_BOUNDS;
		ApplianceMemmoryHandler::responses->write4BCPNestedElementsOutOfBound();
		return '\0';
	}
	}
}

_4BCPElementValue** MsgPackHandler::setStructElementValue() {
//	ApplianceMemmoryHandler::responses->writeRaw(F("DEBUG in setStructElementValue ptr"));
//	return false; //DEBUG
	switch (total_elementValue4BCP) {
	case 0: {
		total_elementValue4BCP++;
		return &_4BCPContainer::value_1;
		break;
	}
	case 1: {
		total_elementValue4BCP++;
		return &_4BCPContainer::value_2;
		break;
	}
	case 2: {
		total_elementValue4BCP++;
		return &_4BCPContainer::value_3;
		break;
	}
	case 3: {
		total_elementValue4BCP++;
		return &_4BCPContainer::value_4;
		break;
	}
	case 4: {
		total_elementValue4BCP++;
		return &_4BCPContainer::value_5;
//		value_number++;
		break;
	}
	case 5: {
		total_elementValue4BCP++;
		return &_4BCPContainer::value_6;
		break;
	}
	default: {
//		return   _4BCPElement::element4BCP_9;
		error_code = ERROR_MSGPACK_4BCP_NESTED_ELEMENTS_OUT_OF_BOUNDS;
		ApplianceMemmoryHandler::responses->write4BCPNestedElementsOutOfBound();
		return '\0';
	}
	}
	return '\0';
}

bool MsgPackHandler::setElementValue(_4BCPMapElement **element) {
//	ApplianceMemmoryHandler::responses->writeRaw(F("DEBUG in setElementValue"));
//	return false; //DEBUG
	//Change to function template each case can be a Template Specialization
	//https://www.geeksforgeeks.org/template-specialization-c/

//	 ApplianceMemmoryHandler::responses->writeRaw(
//		F(" value type of nested element inside seElementValue()::"));
//	 ApplianceMemmoryHandler::responses->writeByte((*element)->value_type);

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
		(*element)->value->bool_value = true;
		return true;
	}

	case MSGPACK_BIN8: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_BIN16: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_BIN32: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_EXT8: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_EXT16: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_EXT32: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_FLOAT32: {
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_FLOAT64: {
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
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
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
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
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_FIXEXT1: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_FIXEXT2: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_FIXEXT4: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_FIXEXT8: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_FIXEXT16: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
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
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_STR32: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_ARRAY16: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_ARRAY32: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_MAP16: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	case MSGPACK_MAP32: {
		//TODO:
		error_code = ERROR_MSGPACK_UNIMPLEMENTED;
		ApplianceMemmoryHandler::responses->writeMsgPackUnimplemented(
				(*element)->value_type);
		return false;
	}

	default: {
		//MAP nested element
		if ((*element)->value_type > 0x80 && (*element)->value_type <= 0x8f)
			return true;
		//TODO:
		error_code = ERROR_MSGPACK_UNKNOW_TYPE;
		ApplianceMemmoryHandler::responses->writeMsgPackUnknownType(
				(*element)->value_type);
		return false;
	}
	}

	error_code = ERROR_MSGPACK_UNKNOW;
	ApplianceMemmoryHandler::responses->writeMsgPackUnknowError();
	return false;
}

uint8_t MsgPackHandler::getNextType() {
	uint8_t _byte = next();

	if (!MsgPackDataTypes::checkDataType(_byte)) {
		//If it's not a recognized type on MsgPack definitions
		error_code = ERROR_MSGPACK_UNKNOW_TYPE;
		ApplianceMemmoryHandler::responses->writeMsgPackUnknownType(_byte);
		return false;
	}

	return _byte;
}

bool MsgPackHandler::checkModulesMap() {
	//TODO loop into devices sensors actuators map

	//TODO: if( error )
//	error_code = ERROR_MSGPACK_4BCP_UNKNOW;
//	 ApplianceMemmoryHandler::responses->writeMsgPackProcessingFlowError(status, next_status, prev_status);
//	return false

	return true;
}

bool MsgPackHandler::processCommandHeader(uint8_t _byte) {
	//PROCESS KEY
	if (!MsgPackDataTypes::checkDataType(_byte)) {
		//If it's not a recognized type on MsgPack definitions
		error_code = ERROR_MSGPACK_UNKNOW_TYPE;
		ApplianceMemmoryHandler::responses->writeMsgPackUnknownType(_byte);
		return false;
	}

	//get key uint32_t
	if (!assemble_uint32_Byte(_byte))
		return false;

	if (_32bitword != MODULE_COMMMAND_FLAG) {
		//TODO:
		error_code = ERROR_MSGPACK_4BCP_NO_COMMAND_FLAG;
		ApplianceMemmoryHandler::responses->writeErrorMsgPack4BCPHasNoCommandFlag();
		return false;
	}

	_byte = next();
	if (!assemble_uint32_Byte(_byte))
		return false;

	ApplianceMemmoryHandler::commands_handler->command_executing = _32bitword;

	ApplianceMemmoryHandler::responses->writeRaw(F("COMMAND TO EXECUTE:"));
	ApplianceMemmoryHandler::responses->write32bitByte(
			ApplianceMemmoryHandler::commands_handler->command_executing);

	if (!setStatus(MSGPACK_STATE_COMMAND_SET))
		return false;

	return true;
}

bool MsgPackHandler::processArray(uint8_t _word, uint8_t array_size) {
	//TODO
//	error_code = ERROR_MSGPACK_NOT_IMPLEMENTED;
//	 ApplianceMemmoryHandler::responses->writeMsgPackUnknownType(_byte);
	return false;//return false if/but all errors had been took care and reponses has already trasmitted
}

/**
 * BUFFER HANDLE's
 */
uint8_t MsgPackHandler::whatNext() {
	return LocalBuffers::client_request_buffer[buffer_position + 1];
}

uint8_t MsgPackHandler::actualBufferByte() {
	return LocalBuffers::client_request_buffer[buffer_position];
}

uint8_t MsgPackHandler::next() {
	buffer_bytes_remaining--;
	if (buffer_bytes_remaining < 0) {
		buffer_bytes_remaining = 0;
		ApplianceMemmoryHandler::responses->writeRaw(
				F("There is no bytes left on buffer to processs."));
		return '\0';
	}

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
		ApplianceMemmoryHandler::responses->writeProcess32bitwordERROR();
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
//			 ApplianceMemmoryHandler::responses->write32bitByte(_32bitword); // DEBUG
			break;
		}
	}

	return true;
}

bool MsgPackHandler::reset_32bit_processing() {
	//This should never happen, only if your programming made some mistake on the PROGRAM STRUCTURE FLOW
	if (status == MSGPACK_STATE_WORKING_32BIT) {
		error_code = ERROR_32BIT_RESETING;
		ApplianceMemmoryHandler::responses->writeReseting32bitwordERROR();
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
//		 ApplianceMemmoryHandler::responses->writeRaw(F("LOOPING STATUS:"));
//		 ApplianceMemmoryHandler::responses->writeByte(actual_process);
//		 ApplianceMemmoryHandler::responses->writeRaw(F("PREV STATUS:"));
//		 ApplianceMemmoryHandler::responses->writeByte(prev_status);

		if (status == actual_process) {
			next_status = pgm_read_word(&msgPack4BCPProcessFlow[position] + 1); //next position
			//DEBUG
//			 ApplianceMemmoryHandler::responses->writeRaw(F("IDENTIFIED STATUS:"));
//			 ApplianceMemmoryHandler::responses->writeByte(actual_process);
//			 ApplianceMemmoryHandler::responses->writeRaw(F("PREV STATUS:"));
//			 ApplianceMemmoryHandler::responses->writeByte(prev_status);
//			 ApplianceMemmoryHandler::responses->writeRaw(F("NEXT STATUS:"));
//			 ApplianceMemmoryHandler::responses->writeByte(next_status); //DEBUG

			//LOOP for setting argument and waiting arg value. goes from 30 to 31 then 30 again to 31 then again until no more args is left
			if (status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE
					&& prev_status == MSGPACK_STATE_COMMAND_SET)
				return true;

			if (status == MSGPACK_STATE_COMMAND_SETTING_ARGS
					&& prev_status == MSGPACK_STATE_COMMAND_SETTING_ARGS)
				return true;

			if (status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE
					&& prev_status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE
					&& next_status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE)
				return true;

//			if(status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE && prev_status ==  MSGPACK_STATE_COMMAND_SETTING_ARGS)
//				next_status = MSGPACK_STATE_COMMAND_ARGS_READY;

//			 ApplianceMemmoryHandler::responses->writeRaw(F("NEXT STATUS:"));
//			 ApplianceMemmoryHandler::responses->writeByte(next_status); //DEBUG

			if ((last_process != '\0') && last_process != prev_status) {
				error_code = ERROR_MSGPACK_4BCP_PROCESSING_FLOW;
				ApplianceMemmoryHandler::responses->writeMsgPackProcessingFlowError(
						status, next_status, prev_status);
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
//	 ApplianceMemmoryHandler::responses->write32bitByte(_32bitword);
//	 ApplianceMemmoryHandler::responses->writeRaw(F("processByte()->process4BytesCmdProtocol() false"));
//	 ApplianceMemmoryHandler::responses->writeMsgPackError(_32bitword);
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

void MsgPackHandler::reset() {
	status = MSGPACK_STATE_IDLE;
	prev_status = '\0';
	next_status = '\0';

	error_code = '\0';

	//TCP request buffer
	buffer_lenght = 0;
	buffer_position = 0;
	buffer_bytes_remaining = 0;
	;
	buffer_processsing_byte = '\0';

	last_byte = '\0'; // may be we'll use to know if we are inseide array, but i think other controll should be done for that

	//32bit word of the 4Bytes Command Protocol
	_32bitword = '\0';
	; // this buffer is utilized directly from inside methods, beaware to don't overwrite it
	_32bitword_remaining = 4; // 4 8 bit bytes to achieve 32bits unsignedLong
	_32bitword_array[4]; //4th index is the NULL terminator

	response_headers_code = '\0';
	response_headers_already_sent = false;

	//Acording to definition MAX_MSGPACK_NESTED_ELEMENTS
//	for(uint8_t i = 0; i < total_element4BCP;i++){
//		if(_4BCPContainer::map4BCP.elements[i]->total_nested_elements > 0){
//			for(uint8_t j = 0; j < _4BCPContainer::map4BCP.elements[i]->total_nested_elements;j++){
//				free(_4BCPContainer::map4BCP.elements[i]->nested_elements[j]);
//			}
//		}
//		free(_4BCPContainer::map4BCP.elements[i]);
//	}

	_4BCPContainer::map4BCP.size = 0;

	element4BCP_number = 0;
	nested_element4BCP = 0;
	total_element4BCP = 0;
	total_elementValue4BCP = 0;
	elements_remaining = MAX_MSGPACK_4BCP_ELEMENTS;


}

/**
 * we wont use (at least try to) pointer to another pointer just for standards [&](){ cout << F(x)} ...
 */
//unsigned long MsgPackHandler::get32bitByte(){
//	return _32bitword;
//}
