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
#include <MsgPackHandler.h>
#include <MsgPackDataTypes.h>
#include <MsgPack4BCPMapElement.h>
#include <MsgPack4BCPMap.h>
#include <msgpack_defines.h>
#include <module_string.h>
#include <errors_code.h>
#include <commands_map.h>
#include <devices.h>


//TODO: sintax-framework namespace
//namespace sintax-iot-framework{
//
//}



//static class members must be declared also in cpp.
static const uint8_t MsgPackHandler::MSGPACK4BCPProcessFlow[MSGPACK4BCPProcessFlow_SIZE] PROGMEM = {
		MSGPACK_STATE_IDLE,
		MSGPACK_STATE_BEGIN,
		MSGPACK_STATE_COMMAND_SET,
		MSGPACK_STATE_COMMAND_SETTING_ARGS,
		MSGPACK_STATE_COMMAND_WATING_ARG_VALUE,
		MSGPACK_STATE_COMMAND_EXECUTING,
		MSGPACK_STATE_COMMAND_EXECUTED,
		MSGPACK_STATE_COMMAND_FINISHED
	};

static const uint8_t MsgPackHandler::MSGPACK4BCPProcessFlow2[MSGPACK4BCPProcessFlow2_SIZE] PROGMEM = {
		MSGPACK_STATE_IDLE,
		MSGPACK_STATE_BEGIN,
		MSGPACK_STATE_COMMAND_SET,
		MSGPACK_STATE_COMMAND_EXECUTING,
		MSGPACK_STATE_COMMAND_EXECUTED,
		MSGPACK_STATE_COMMAND_FINISHED
	};

/**
 * we will need Response Object to write on TCP client JSON objects and Commands to execute them on the fly.
 */
MsgPackHandler::MsgPackHandler(Responses *_responses, Commands *_commands, SintaxesLib *_sintaxes_lib) {
	response = _responses;
	commands = _commands;
	sintaxesLib = _sintaxes_lib;
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
	if(!setStatus(MSGPACK_STATE_BEGIN)) {
		response->writeError_on_INIT();
		response_headers_already_sent = true;
		response_headers_code = 500;
		return false;
	}

	reset_32bit_processing();
	buffer_position = 0;
	stream = _stream;
	buffer_lenght = stream->readBytes(LocalBuffers::client_request_buffer, size);
	buffer_bytes_remaining = buffer_lenght;

	map = MsgPack4BCPMap();
	MsgPack4BCPMapElement elements[MAX_MSGPACK_ARGS];

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
bool MsgPackHandler::processStream() {
	while (buffer_bytes_remaining > 0) {
		//Get next byte on buffer
		uint8_t _byte = MsgPackHandler::next();
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
		if( array_size > 0){
//			processArray(_byte, array_size);
//			response->writeRaw(F("MAP:"));
//			response->writeByte(_byte); //DEBUG
//			response->writeRaw(F("SIZE:"));
//			response->writeByte(array_size); //DEBUG

			continue;
		}

		//THIS IS AN MAP- [COMMANDS FRAMEWORK] STATUS ALREAY PROCESSING
		map_elements_size = isMap(_byte);
		if(map_elements_size > 0) {
			processMap(_byte, map_elements_size);
//			response->writeRaw(F("MAP:"));
//			response->writeByte(_byte); //DEBUG
//			response->writeRaw(F("SIZE:"));
//			response->writeByte(map_elements_size); //DEBUG
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
		if(!processByte(_byte)) {
			response->writeMsgPackProcessingFlowStatus(status, next_status, prev_status);//DEBUG
			return false;
		}

		if(status == MSGPACK_STATE_COMMAND_EXECUTED){
			if(buffer_bytes_remaining > 0){
				error_code = ERROR_MSGPACK_4BCP_IN_FINISHED_STATE_WITH_REMAINING_BYTES;
				response->writeErrorMsgPackHasFinishedWithBytes();
				response->closeJsonResponse();
				setStatus(MSGPACK_STATE_IDLE);
				return false;
			}
			if(!setStatus(MSGPACK_STATE_COMMAND_FINISHED))
				return false;
			break;
		}
	}

//	response->writeMsgPackProcessingFlowError(status, next_status, prev_status); //DEBUG

	//END OF EXEUTION OF COMMANDS ON THE FLY WITH RESPECTIVES RESPONSES DONE
	//NOTHING MORE TO DO JUST RETURN true TO MAIN FUNCTION SAVE THE MACHINE STATE ON SD Card
	//AND ALL 4BCP FOOTPRINT OF THIS REQUEST FOR  LOG PROPOUSES

	//return after execution of each command with status and guard the new machine state on SD Card
	//we will use SD Card as log. So must implement methods for getting this log if requested
	if(status == MSGPACK_STATE_COMMAND_FINISHED) {
		if(!setStatus(MSGPACK_STATE_IDLE)) {
			response->closeJsonResponse();
			return false;
		}

		return true;
	} else{
//		response->writeByte(status);
		error_code = ERROR_MSGPACK_NOT_IN_FINISHED_STATE;
		response->writeErrorMsgPackHasNotFinishedStatus();
		response->closeJsonResponse();
		setStatus(MSGPACK_STATE_IDLE);
		return false;
	}
}


/**
 * ON COMMAND FRAMEWORK PROTOCOL WILL ALWAYS BE LOOKING FOR 32BIT UNSINED LONG
 * THE INTERPRETATION OF THE 'RPC' IS ALL BASED ON 4BYTES CONSTANTS;
 * THE 4BYTES CONSTANTS COMMAND PROTOCOL IS DEFINED IN OTHERS FILES SUCH commands_map.h
 * AND devices.h WHICH SHOULD BE WRITTEN SPECIFIC per APPLIANCE et si, FOLLOWING THE
 * 4BCP GUIDELINES.
 *
 *
 * if it is'nt array or map (which was checked before this method beeing called),
 * then it should be the 4Bytes command protocol, like a COMMAND_FLAG a COMMAND per se,
 * known mapped ARGUMENTS or even some argument values for the COMMAND call.
 *
 * This switch is intent to:
 * 	1 - unpack 4Bytes Commends Protocol (4BCP)
 * 	2 - look for arguments values for 4BPC
 * 	3 - report error on 4Bytes Commands Protocol with rollback action
 * 		to previous state guarded on SD card by returning false so method processStream() can return false as well
 * 		and let to main() function do what's necessary on processing fail
 */
bool MsgPackHandler::processByte(uint8_t _byte) {

	//remind that the proposal scope of this switch() is the 3 guidelines mentioned above in the method
	switch (_byte) {
		//LAYER 7 - 4Bytes COMMANDS FRAMEWORK PROCESSING - (RPC) [messagePack 0xce packet]
		case MSGPACK_UINT32: {
//			sintaxesLib->buzz(5000, 800,1);
			if(!assemble32bitByte(_byte))
				return false;

			//this method process4BytesCmdProtocol() is the one in charge for assemble the Command Class
			//in order to process our 4BCP requests
			if(!process4BytesCmdProtocol())
				return false;


			//always remember to reset _32bitword which is a 1 buffer of uint32 which care the 4BCP word
			//it's always reused on going processing
			reset_32bit_processing();
			return true;
		}

		case MSGPACK_TRUE: {
			//TODO set ARG VALUE
//			response->writeByte(true);//DEBUG
			return true;
		}

		case MSGPACK_FALSE: {
			//TODO set ARG VALUE
//			response->writeByte(false);//DEBUG
			return true;
		}

		case true: {
			//TODO set ARG VALUE
//			response->writeByte(false);//DEBUG
			return true;
		}

		case false: {
			//TODO set ARG VALUE
//			response->writeByte(false);//DEBUG
			return true;
		}

		//TODO: implement more MessagePack types specification that could came as argument
		//such as Float, ints 8 16 sign and unsigned and maybe strings for guardings literals on SD Card

		default: {
			response->writeByte(_byte);
			error_code = ERROR_MSGPACK_PROCESSING;
			response->writeMsgPackError(_byte);
//			response->writeByte(_byte); //DEBUG
			//TODO: rollback from SD Card previous state and then send another message with rollback
			//		current state machine (maybe rollback should be on the previous function
			//		or the previous previous, check it)
			return false;
		}
	}
	error_code = ERROR_MSGPACK_UNKNOW;
	response->writeMsgPackUnknowError();
	return false;
}

/**
 * 4Bytes Command Protocol
 * here we assemble the COMMANDS and wait for code EXECUTE! 0xffffff13
 *
 * Remember that this method should only be called exactly after assemble32bitByte(_byte) so
 * the field _32bitword should be ready
 *
 */
bool MsgPackHandler::process4BytesCmdProtocol(){
	switch(_32bitword){
		case MODULE_COMMMAND_FLAG: {
			//next byte must be a MSGPACK_UINT32 MessagePack 0xce with the COMMAND to be executed
			uint8_t _byte = next();
			if(!assemble32bitByte(_byte))
				return false;

			//This is a new _32bit_word the command to be executed. Now assemble arguments and/or expect for MODULE_COMMMAND_EXECUTE_FLAG ()
			commands->command_executing = _32bitword;
			if(!setStatus(MSGPACK_STATE_COMMAND_SET))
				return false;

			return true;
		}

		case MODULE_COMMMAND_EXECUTE_FLAG: {
			uint8_t _byte = next();
			if(!_byte == MSGPACK_TRUE){
				error_code = ERROR_MSGPACK_4BCP_EXECUTE_FLAG;
				response->writeErrorMsgPack4BCPExecuteFlagError();
				setStatus(MSGPACK_STATE_COMMAND_FINISHED);
				return false;
			}
			//FIXME: POG to keep flow right
//			setStatus(MSGPACK_STATE_COMMAND_SETTING_ARGS);
//			setStatus(MSGPACK_STATE_COMMAND_WATING_ARG_VALUE);

			if(!setStatus(MSGPACK_STATE_COMMAND_EXECUTING))
				return false;


			if(!commands->execute()){
				setStatus(MSGPACK_STATE_COMMAND_FINISHED);
				return false;
			}

			//TODO: LOG AND RETURN COMMAND STATUS MESSAGE THEN change status to FINISHED
			setStatus(MSGPACK_STATE_COMMAND_EXECUTED);
			if(!setStatus(MSGPACK_STATE_COMMAND_FINISHED))
				return false;

			return true;
		}

		case MODULE_COMMMAND_SET_ARGS1:{
			if(!setStatus(MSGPACK_STATE_COMMAND_SETTING_ARGS))
				return false;
//			if(commands->setArgumentField(MODULE_COMMMAND_SET_ARGS1)){
				if(!setStatus(MSGPACK_STATE_COMMAND_WATING_ARG_VALUE))
					return false;
//				return true;
//			}
			return true; //DEBUG
		}

		case MODULE_COMMMAND_SET_ARGS2:{
			if(!setStatus(MSGPACK_STATE_COMMAND_SETTING_ARGS))
				return false;
//			if(commands->setArgumentField(MODULE_COMMMAND_SET_ARGS2)){
				if(!setStatus(MSGPACK_STATE_COMMAND_WATING_ARG_VALUE))
					return false;
//				return true;
//			}
			return true; //DEBUG
		}

		default: {
			//TODO: case in the commands or devices or arguments
			unsigned long resource = isMapped();
			if(resource > 0 ){
				if(processMappedResource(resource))
					return true;
			}

			//Everything possible in _32bitword must be Mapped  :: A T T E N T I O N :: what about 32bit arguments values
			//this default is not true check Machine status
			if(status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE){
				//it's a 32bit argument value
				//TODO: set the value for the current argument
				response->write32bitByte(_32bitword); // DEBUG
				return true;
			}

			//If it's not a argument value it's an inconsistent on 4BCP
			error_code = ERROR_MSGPACK_4BCP_UNKNOW;
			response->writeMsgPackProcessingFlowError(status, next_status, prev_status);
			return false;
		}
	}

	error_code = ERROR_MSGPACK_4BCP_UNKNOW;
	response->write4BCPUnknowError(prev_status, status, next_status);
	//TODO: if 32bitword is not mapped return false; send responses [ remember, throw is always @front  each false must than be taken CARE]
	return false;
}


bool MsgPackHandler::processMappedResource(unsigned long resource){
	unsigned long waiting_for = _4BCPCheckForNext(resource);
	//TODO: implement a table of with ENUN on DEFINED RESOURCES to flash PROGMEM
	return true;
}

/**
 * Check in 4bytes Command Protocol what's the next 4BCP resource his waiting for
 */
unsigned long MsgPackHandler::_4BCPCheckForNext(unsigned long resource){
	//TODO: implement a table of execution flow with ENUN on the flash PROGMEM
	//on error
//	error_code = ERROR_MSGPACK_4BCP_WORD_EXPECTED;
//	response->writeMsgPackError(_32bitword);
	return resource; //DEBUG MUST BE THE NEXT WORD 4BCP kind of is awaiting
}


/**
 *	processMap() is a state machine based processor. Every package must be encoded with 4BCP protocol which defines
 *	that always will be sent in the request a msgpack format MAP with 1 (one) and only ONE type of command per request.
 *	It command can be executed multiple times with different devices, p.e. one can send a request to SET different actuators
 *	with different arguments values. The command is the same 'MODULE_COMMMAND_SET_ACTUATOR' and it can be repeated for
 *	multiples actuators devices (which is mapped by 4BCP) with different values. Or it can be a simples GET command with no
 *	arguments at all, p.e. 'MODULE_COMMMAND_GET_DATA', 'MODULE_COMMMAND_GET_STATE', 'MODULE_COMMAND_GET_PROCESS_FLOW'.
 *
 *	The main effort is to understand 4BCP, which is described in our wiki. But for a glance here's a simple example of 4BCP structure
 *	message on top of msgpack protocol.
 *
 *
 * Our PHP client test pack the 4BCP on top of msgpack protocol. Just have in mind that the main goal of 4BCP is to have a lot
 * of mapping devices, sensors, commands, in 32bit unsigned long hex words:
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
 *	Note that 4BCP is constant defined based. Each constant has a uint32bit_t hex value and it's used to process RPC's
 *	In the example above, the first thing you'll notice is that the first tuple is the COMMAND definition. The key tells
 *	the processor that it's value is a command, so it will look on commands_map.h to check if it's implemented.
 *
 *	IT'S MANDATORY THAT FIRST TUPLE BE THE COMMAND SET INSTRUCTIONS
 *
 *	In this case the command is SET ACTUATORS values. Since we can have several actuators in one module, each must have a
 *	unique signature which is the 4BCP uint32_t hex value defined in 'devices.h'. This message tell us that the command is
 *	MODULE_COMMMAND_SET_ACTUATOR and the other tuples is the definition of each actuator that must be run the command for with
 *	another msgpack MAP containing the actuators set values. The max number of commands to execute and the maximum numbers
 *	of arguments is defined in 'sintaxes-framework-defines.h' For example we can set a command to execute the maximum of 4 times,
 *	and the maximum of 4 arguments per command.This is defined in the constants 'MAX_MSGPACK_COMMANDS' and 'MAX_MSGPACK_ARGS'
 *
 *	On the case of our example for set actuators we defined another 3 tuples with the actuators UUID as key for the arguments MAP
 *	of this actuator itself. The number of arguments must be defined per COMMAND function. In this case only 2. 1 - if it
 *	should be ON/OFF (true/false) and the time it must be on that state. There will be soon a watchdog for proposes like this.
 *
 *	A simpler request could be to get the sensors data of this module like this:
 *
 * $payload = $packer->pack([
 *     MODULE_COMMMAND_FLAG => MODULE_COMMMAND_GET_DATA,
 *     MODULE_COMMMAND_EXECUTE_FLAG => true
 *   ]
 * );
 *
 * As before, first tuple to set command, in this case 'MODULE_COMMMAND_GET_DATA'. Note that the same
 * way as before the last tuple must be MODULE_COMMMAND_EXECUTE_FLAG 4BCP constant with value true.
 *
 * This last tuple is used by the processor to tell the module RUN COMMAND, everything has been set.
 *
 */
bool MsgPackHandler::processMap(uint8_t _byte, int map_elements_size) {

	uint8_t element_number = 0;
	map.size = map_elements_size;
	map.position = 0;

	while(map_elements_size > 0) {

		//SET Command with first element of MAP which is mandatory to be COMMAND_FLAG WITH VALUE AS COMMAND_TO_EXECUTE
		if(status == MSGPACK_STATE_BEGIN) {
			processCommandHeader(_byte);
			map_elements_size--;
			continue;
		}

		//This will be the first tuple
		MsgPack4BCPMapElement element;

		//Time to get ARGS for COMMAND. as 4BCP explicit, beginning with the COMMAND_ARG_TYPE it self, it can be an actuator for instance
		if(status == MSGPACK_STATE_COMMAND_SET) {
			_byte = getNextType();

			//get key uint32_t
			if(!assemble32bitByte(_byte))
				return false;

			if(checkModulesMap()){
				element.setKey(_32bitword);
				if(!setStatus(MSGPACK_STATE_COMMAND_SETTING_ARGS))
					return false;
			}
		}

		//We already have the key on tuple for executing the command
		if(status == MSGPACK_STATE_COMMAND_SETTING_ARGS ){
			_byte = getNextType();
			uint8_t map_size = isMap(_byte);
			if(map_size > 0) {
				//This is each new array() on the msgpack generally 0x81 0x82 0x83 this are the values for SETTING Actuators
				while(map_size > 0){
					if(!setStatus(MSGPACK_STATE_COMMAND_WATING_ARG_VALUE))
						return false;
					_byte = getNextType();
					MsgPack4BCPMapElement element_1;

					//get key uint32_t
					if(!assemble32bitByte(_byte))
						return false;

					if(checkModulesMap()){
						element_1.setKey(_32bitword);
						_byte = getNextType();
						element_1.setType(_byte);
						setElementValue(&element_1, _byte);
						if(!element.setElement(element_1)){
							//TODO:
	//						error_code = ERROR_MSGPACK_4BCP_UNKNOW;
	//						response->writeMsgPackProcessingFlowError(status, next_status, prev_status);
							return false;
						}
					}
					map_size--;
				}
				if(!setStatus(MSGPACK_STATE_COMMAND_SETTING_ARGS))
					return false;
			}
		}
		map.elements[element_number] = element;
		element_number++;
		map_elements_size--;
	}
	return true; //DEBUG
}

bool MsgPackHandler::setElementValue(MsgPack4BCPMapElement *element, uint8_t _byte){
	element->value_type = _byte;
	switch(_byte){
		case MSGPACK_NIL: {
			element->value_bool = false;
			return true;
		}

		case MSGPACK_FALSE: {
			element->value_bool = false;
			return true;
		}

		case MSGPACK_TRUE: {
			element->value_bool = true;
			return true;
		}

//		case MSGPACK_BIN8: {
//			return true;
//		}
//
//		case MSGPACK_BIN16: {
//			return true;
//		}
//
//		case MSGPACK_BIN32: {
//			return true;
//		}
//
//		case MSGPACK_EXT8: {
//			return true;
//		}
//
//		case MSGPACK_EXT16: {
//			return true;
//		}
//
//		case MSGPACK_EXT32: {
//			return true;
//		}

		case MSGPACK_FLOAT32: {
//			if(!assemble32bitFloat(_byte))
//				return false;
//			element->value_float = _32bitword;
			return true;
		}

		case MSGPACK_FLOAT64: {
			return true;
		}

		case MSGPACK_UINT8: {
			element->value_uint8 = next();
			return true;
		}

		case MSGPACK_UINT16: {
			return true;
		}

		case MSGPACK_UINT32: {
			if(!assemble32bitByte(_byte))
				return false;
			element->value_uint32 = _32bitword;
			return true;
		}

		case MSGPACK_UINT64: {
			return true;
		}

		case MSGPACK_INT8: {
			element->value_int8 = next();
			return true;
		}

		case MSGPACK_INT16: {
			return true;
		}

		case MSGPACK_INT32: {
//			if(!assemble32bitByte(_byte))
//				return false;
//			element->value_uint32 = _32bitword;
			return true;
		}

//		case MSGPACK_INT64: {
//			return true;
//		}
//
//		case MSGPACK_FIXEXT1: {
//			return true;
//		}
//
//		case MSGPACK_FIXEXT2: {
//			return true;
//		}
//
//		case MSGPACK_FIXEXT4: {
//			return true;
//		}
//
//		case MSGPACK_FIXEXT8: {
//			return true;
//		}
//
//		case MSGPACK_FIXEXT16: {
//			return true;
//		}

		case MSGPACK_STR8: {
			element->value_fixstring = next();
			return true;
		}

//		case MSGPACK_STR16: {
////			element->value_fixstring = next();
//			return true;
//		}
//
//		case MSGPACK_STR32: {
////			element->value_fixstring = next();
//			return true;
//		}

//		case MSGPACK_ARRAY16: {
//			return true;
//		}
//
//		case MSGPACK_ARRAY32: {
//			return true;
//		}
//
//		case MSGPACK_MAP16: {
//			return true;
//		}
//
//		case MSGPACK_MAP32: {
//			return true;
//		}

		default:{
			return false;
		}
	}

	return false;
	}
}

uint8_t  MsgPackHandler::getNextType() {
	uint8_t _byte = next();

	if(!MsgPackDataTypes::checkDataType(_byte)){
		//If it's not a recognized type on MsgPack definitions
		error_code = ERROR_MSGPACK_UNKNOW_TYPE;
		response->writeMsgPackUnknownType(_byte);
		return false;
	}

	return _byte;
}

bool MsgPackHandler::checkModulesMap(){
	//TODO loop into devices sensors actuators map
	return true;
}

bool MsgPackHandler::processCommandHeader(uint8_t _byte){
	//PROCESS KEY
	if(!MsgPackDataTypes::checkDataType(_byte)){
		//If it's not a recognized type on MsgPack definitions
		error_code = ERROR_MSGPACK_UNKNOW_TYPE;
		response->writeMsgPackUnknownType(_byte);
		return false;
	}

	//get key uint32_t
	if(!assemble32bitByte(_byte))
		return false;

	if(_32bitword != MODULE_COMMMAND_FLAG){
//				error_code = ERROR_32BIT_PROCESSING;
//				response->writeProcess32bitwordERROR();
		return false;
	}

	_byte = next();
	if(!assemble32bitByte(_byte))
		return false;

	commands->command_executing = _32bitword;
	if(!setStatus(MSGPACK_STATE_COMMAND_SET))
		return false;
	return true;

//	error_code = ERROR_MSGPACK_4BCP_ELEMENT_KEY_PROCESSING;
//	response->writeErrorMsgPack4BCPElementKeyProcessing();
//	return false;
}

MsgPack4BCPMapElement MsgPackHandler::processMapKeyElement(_4BCP_MAP_ELEMENT *element, uint8_t _byte){
	if(!MsgPackDataTypes::checkDataType(_byte)){
		//If it's not a recognized type on MsgPack definitions
		error_code = ERROR_MSGPACK_UNKNOW_TYPE;
		response->writeMsgPackUnknownType(_byte);
		return false;
	}

	if(element->key == NULL || element->key <= 0) {
		if(_byte != MSGPACK_UINT32) {
			error_code = ERROR_MSGPACK_4BCP_ELEMENT_HAS_NO_KEY;
			response->writeErrorMsgPack4BCPElementHasNoKey(_byte);
			return false;
		}

		if(!assemble32bitByte(_byte))
			return false;

		element->key = _32bitword;
		return true;
	}

	error_code = ERROR_MSGPACK_4BCP_ELEMENT_KEY_PROCESSING;
	response->writeErrorMsgPack4BCPElementKeyProcessing();
	return false;
}

bool MsgPackHandler::processMapValueElement(_4BCP_MAP_ELEMENT *element, uint8_t _byte){
	if(!MsgPackDataTypes::checkDataType(_byte)){
		//If it's not a recognized type on MsgPack definitions
		error_code = ERROR_MSGPACK_UNKNOW_TYPE;
		response->writeMsgPackUnknownType(_byte);
		return false;
	}

	if(element->key == NULL || element->key <= 0) {
			error_code = ERROR_MSGPACK_4BCP_ELEMENT_HAS_NO_KEY;
			response->writeErrorMsgPack4BCPElementHasNoKey(_byte);
			return false;
	}

	switch(_byte){

	}

	error_code = ERROR_MSGPACK_4BCP_ELEMENT_KEY_PROCESSING;
	response->writeErrorMsgPack4BCPElementKeyProcessing();
	return false;
}

bool MsgPackHandler::processArray(uint8_t _word, int array_size) {
	return true; //DEBUG
//	uint8_t next_word = whatNext();
//
//	while (array_size > 0) {
//		array_size--;
//		uint8_t _word = next();
//		response->writeByte(_word);
//
//		//THIS IS A MAP
//		if (_word > MSGPACK_MAP_INITIAL && _word <= MSGPACK_MAP_FINAL) {
//			uint8_t map_size = _word - MSGPACK_MAP_INITIAL;
//			while (map_size > 0) {
//				uint8_t _word = next();
//				map_size--;
//				switch (_word) {
//				case MSGPACK_UINT32:
//					while (_32bitword_remaining > 0) {
//						uint8_t _word = next();
//						assemble32bitByte(_word);
//					}
//					response->write32bitByte(_32bitword); // DEBUG
//					reset_32bit_processing();
//					break;
//				case MSGPACK_TRUE:
//					response->writeByte(true);
//					break;
//				case MSGPACK_FALSE:
//					response->writeByte(false);
//					break;
//				}
//			}
//		}
//	}

	return true; //return false if/but all errors had been took care and reponses has already trasmitted
}



/**
 * BUFFER HANDLE's
 */
uint8_t MsgPackHandler::whatNext() {
	return LocalBuffers::client_request_buffer[buffer_position+1];
}
uint8_t MsgPackHandler::next(){
	buffer_bytes_remaining--;
	buffer_position++;
	//FIXME: last_byte has no function
	last_byte = LocalBuffers::client_request_buffer[buffer_position];
	return last_byte;
}



/**
 * 		UINT8 var1 = 0x01; //0000 0001
 *		UINT8 var2 = 0x03; //0000 0011
 *		UINT8 var3 = 0x07; //0000 0111
 *		UINT8 var4 = 0x0F; //0000 1111
 *		UINT32 bigvar = (var1 << 24) + (var2 << 16) + (var3 << 8) + var4;
 */
bool MsgPackHandler::assemble32bitByte(uint8_t _byte) {
	//IF NOT MSGPACK_UINT32 (unsigned long) MessaPack 0xce which is 4Bytes CmdProtocol relies
	if(_byte != MSGPACK_UINT32) {
		error_code = ERROR_32BIT_PROCESSING;
		response->writeProcess32bitwordERROR();
		return false;
	}

	//ensure that _32bitword and it's associated fields are reseted;
	reset_32bit_processing();

	uint8_t actual_status = status;
	status = MSGPACK_STATE_WORKING_32BIT;
	while(_32bitword_remaining > 0){
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
			//The arithmetic should be performed with the 4 bytes at one. Does'nt work storing in the array already shifted
			_32bitword = (_32bitword_array[0] << 24) +
				(_32bitword_array[1] << 16) + (_32bitword_array[2] << 8) + _byte;
			_32bitword_remaining--;
			status = actual_status; //return machine status to the previous status
//			response->write32bitByte(_32bitword); // DEBUG
			break;
		}
	}

	return true;
}

bool MsgPackHandler::reset_32bit_processing() {
	//This should never happen, only if your programming made some mistake on the PROGRAM STRUCTURE FLOW
	if(status == MSGPACK_STATE_WORKING_32BIT){
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

bool MsgPackHandler::check4BCPProcesFlow(const uint8_t *msgPack4BCPProcessFlow, uint8_t array_size){
	uint8_t last_process, actual_process;
	uint8_t position = 0;

	while (array_size > 0) {
		actual_process = pgm_read_word(&msgPack4BCPProcessFlow[position]);
//		response->writeRaw(F("LOOPING STATUS:"));
//		response->writeByte(actual_process);
//		response->writeRaw(F("PREV STATUS:"));
//		response->writeByte(prev_status);

		if(status == actual_process) {

			//DEBUG
//			response->writeRaw(F("IDENTIFIED STATUS:"));
//			response->writeByte(actual_process);
//			response->writeRaw(F("PREV STATUS:"));
//			response->writeByte(prev_status);
//			next_status = pgm_read_word(&msgPack4BCPProcessFlow[position]+1);
//			response->writeRaw(F("NEXT STATUS:"));
//			response->writeByte(next_status); //DEBUG

			//LOOP for setting argument and wating arg value. goes from 30 to 31 then 30 again to 31 then again until no more args is left
			if(status == MSGPACK_STATE_COMMAND_SETTING_ARGS && prev_status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE &&
				next_status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE)
				return true;

			if(status == MSGPACK_STATE_COMMAND_WATING_ARG_VALUE && prev_status ==  MSGPACK_STATE_COMMAND_SETTING_ARGS)
				next_status = MSGPACK_STATE_COMMAND_EXECUTING;

			if(last_process != NULL && last_process != prev_status){
				error_code = ERROR_MSGPACK_4BCP_PROCESSING_FLOW;
				response->writeMsgPackProcessingFlowError(status, next_status, prev_status);
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

bool MsgPackHandler::checkFlow(){

	if(check4BCPProcesFlow(MSGPACK4BCPProcessFlow, MSGPACK4BCPProcessFlow_SIZE))
		return true;

	if(check4BCPProcesFlow(MSGPACK4BCPProcessFlow2, MSGPACK4BCPProcessFlow2_SIZE))
		return true;

	return false;
}


unsigned int MsgPackHandler::isArray(uint8_t _byte){
	if(_byte > MSGPACK_ARRAY_INITIAL && _byte <= MSGPACK_ARRAY_FINAL){
		return _byte - MSGPACK_ARRAY_INITIAL ;
	}

	return 0;
}

unsigned int MsgPackHandler::isMap(uint8_t _byte){
	if(_byte > MSGPACK_MAP_INITIAL && _byte <= MSGPACK_MAP_FINAL){
		return _byte - MSGPACK_MAP_INITIAL ;
	}

	return 0;
}

unsigned long MsgPackHandler::isMapped(){

//#infdef _32bitword
//	error_code = ERROR_MSGPACK_4BC_WORD_NOT_MAPPED;
//	response->write32bitByte(_32bitword);
//	response->writeRaw(F("processByte()->process4BytesCmdProtocol() false"));
//	response->writeMsgPackError(_32bitword);
//	return 0;
//#endif
	//TODO: check if it's in the ENUN in Flash PROGMEM
	return _32bitword; //DEBUG t needs to return the type of 4BCP
}

bool MsgPackHandler::setStatus(uint8_t _status){
//	//DEBUG
//	if(status != MSGPACK_STATE_IDLE) {
//		response->writeRaw(F("MEM BEFORE CHANGE STATUS:"));//DEBUG
//		response->writeByte(status); //DEBUG
//	}

	prev_status = status;
	status = _status;

	//DEBUG
//	if(status != MSGPACK_STATE_IDLE) {
//		response->writeRaw(F("CHANGED STATUS TO:"));//DEBUG
//		response->writeByte(status); //DEBUG
//	}


	if(_status == MSGPACK_STATE_IDLE)
		return true;

	if(!checkFlow())
		return false;

	return true;
}

/**
 * we wont use pointer to another pointer just for standards [&](){ cout << F(x)} ...
 */
//unsigned long MsgPackHandler::get32bitByte(){
//	return _32bitword;
//}
