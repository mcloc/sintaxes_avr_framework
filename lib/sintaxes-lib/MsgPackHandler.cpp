/**
 * This is NOT a oficial messagePack full implementation. It's implementation goes
 * @ the END of our appliance needs. In this case we are using a new Layer 7 (Application Layer)
 * on TOP of MessagePack protocol layer.
 *
 * We call it "4Bytes COMMAND PROTOCOL" (4BCP). It's composed of only 32bit unsigned long (0xffff0000 to0xfffffff)
 * Note that first 2 bytes still reserved for improving the communications of 4Bytes CmdProtocol.
 *
 * This Layer is almost a implementation of RPC but built with a flow of execution designed to
 * be execution at real time processing message, with exceptions thrown @ real time trough TCP
 * client stream as JSON formated and rollback support with previous state guarded on SD Card.
 * We can response JSON because all strings are stored @ flash memory with PROGMEM, so it's
 * quite simple to make a JSON response. But parse JSON requires too much memory
 * and this framework is based on the minimal memory pattern (designed for 2KB RAM), with no malloc()
 * no string pointers (except the FlashStringPointers* which is a powerful tool that allows
 * snprintf_P() formated strings with run time values or other FlashStringPointer*).
 * So once again the most efficient way to send requests to 8bit controller devices (2KB RAM)
 * is MSGPACK with 4bytes COMMAND PROTOCOL on top of it, acting like a RPC
 *
 * commands_map.h, devices.h, is the main essentials headers which #defines all mapping for this
 * appliance at per si. So on the other hand, one whose going to consume and operate this appliance
 * must have the same commands and devices mapped, defined, so the 4bytes CmdProtocol can work properly.
 *
 * Don't worry on any request breaking your sketch this implementation have taken care of unmapped
 * commands, the only problem is to check the maps defines, maybe we should hash the contents of the map
 * to ensure we talking about the same things on both sides.
 *
 */

#include <Arduino.h>
#include <MsgPackHandler.h>
#include <msgpack_defines.h>
#include <module_string.h>
#include <errors_code.h>
#include <commands_map.h>
#include <devices.h>


/**
 * we will need Reponse Object to write on TCP client JSON objects and Commands to execute them on the fly.
 */
MsgPackHandler::MsgPackHandler(Responses *_responses, Commands *_commands) {
	response = _responses;
	commands = _commands;
	status = MSGPACK_STATE_IDLE;
}


/**
 * reset module communication and get the request buffer. The size of the buffer is defined in
 * sintaxes-framework-defines.h and safe guard is taken care on main.cpp main() function
 * avoiding overflow. BEAWARE all settings is defined for minimal RAM usage to fit into 2KB,
 * if using bigger RAM you can increase the size of the request buffer
 * in sintaxes-framework-defines.h
 */
bool MsgPackHandler::init(Stream *_stream, int size) {
	status = MSGPACK_STATE_BEGIN;

	reset_32bit_processing();
	buffer_position = 0;
	stream = _stream;
	buffer_lenght = stream->readBytes(LocalBuffers::client_request_buffer, size);
	buffer_bytes_remaining = buffer_lenght;

	//IF BUFFER LEN == 0 ERROR NO MSG must post with no readers messagePack with the devices ptrocol
	response->writeModule200DataHeaders();

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
		uint8_t _byte = MsgPackHandler::next();
		response->writeByte(_byte);


		/**
		 * If it's an array or a map the LAYER 7 4B COMMANDS FRAMEWORK (RPC)
		 * is already been processed, remember that this machine is suppouse
		 * to interpret commands as defined per use et si.
		 */
		//THIS IS AN ARRAY - [COMMANDS FRAMEWORK] STATUS ALREAY PROCESSING
		if(int array_size = isArray(_byte) > 0){
			processArray(_byte, array_size);
			continue;
		}
		//THIS IS AN MAP- [COMMANDS FRAMEWORK] STATUS ALREAY PROCESSING
		if(int map_elements_size = isMap(_byte) > 0){
			processMap(_byte, map_elements_size);
			continue;
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
		if(processByte(_byte)) {
			continue;
		} else {
			return false;
		}

	}

	//END OF EXEUTION OF COMMANDS ON THE FLY WITH RESPECTIVES RESPONSES DONE
	//NOTHING MORE TO DO JUST RETURN true TO MAIN FUNCTION SAVE THE MACHINE STATE ON SD Card
	//AND ALL 4BCP FOOTPRINT OF THIS REQUEST FOR  LOG PROPOUSES

	//return after execution of each command with status and guard the new machine state on SD Card
	//we will use SD Card as log. So must implement methods for getting this log if requested
	return true;
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
 * then it should be the 4Bytes command protocol, like a COMMAND_FLAG a COMMAND per si,
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
		case MSGPACK_UINT32:
			if(!assemble32bitByte(_byte))
				return false;

			//this method process4BytesCmdProtocol() is the one in charge for assemble the Command Class
			//in order to process our 4BCP requests
			if(!process4BytesCmdProtocol())
				return false;


			//always remember to reset _32bitword which is a 1 buffer of uint32 which care the 4BCP word
			//it's always reused on further processing
			reset_32bit_processing();
			return true;

		case MSGPACK_TRUE:
			//TODO set ARG VALUE
			response->writeByte(true);//DEBUG
			return true;

		case MSGPACK_FALSE:
			//TODO set ARG VALUE
			response->writeByte(false);//DEBUG
			return true;

		default:
			error_code = ERROR_MSGPACK_PROCESSING;
			response->writeByte(_byte);
			response->writeMsgPackError(_byte);
			//TODO: rollback from SD Card previous state and then send another message with rolledback
			//		current state machine (maybe rollback should be on the previous function
			//		or the previous previous, check it)
			return false;
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
			assemble32bitByte(_byte);
			//This is a new _32bit_word the command to be executed
			commands->command_executing = _32bitword;
			status = MSGPACK_STATE_COMMAND_SET;
			return true;
		}
		case MODULE_COMMMAND_GET_DATA: {
			commands->command_executing = _32bitword;
			commands->get_data();
			status = MSGPACK_STATE_COMMAND_EXECUTED;
			return true;
		}
		case MODULE_COMMMAND_EXECUTE_FLAG: {
			commands->execute();
			status = MSGPACK_STATE_COMMAND_EXECUTED;
			return true;
		}
		default: {
			//TODO: case in the commands or devices or arguments
			if(unsigned long resource = isMapped()){
				if(processMappedResource(resource))
					return true;
			}

			//Everything possible in _32bitword must be Mapped  :: A T T E N T I O N :: what about 32bit arguments values
			//this default is not true check Machine status
			if(status = MSGPACK_STATE_COMMAND_SETTING_ARGS){
				//it's a 32bit argument value
				//TODO: set the value for the current argument
				return true;
			}


			//If it's not a argument value it's an inconsistence on 4BCP
			error_code = ERROR_MSGPACK_UNKNOW;
			response->writeMsgPackUnknowError();
			return false;
		}
	}

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
	error_code = ERROR_MSGPACK_4BCP_WORD_EXPECTED;
	response->writeMsgPackError(_32bitword);
	return resource; //DEBUG MUST BE THE NEXT WORD 4BCP kind of is awaiting
}

bool MsgPackHandler::processMap(uint8_t _word, int map_elements_size) {
	return true; //DEBUG
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
	prev_status = status; //save the actual status and set it to MSGPACK_STATE_WORKING_32BIT
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
			status = prev_status; //return machine status to the previous status
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


unsigned int MsgPackHandler::isArray(uint8_t _byte){
	if(_byte > MSGPACK_ARRAY_INITIAL && _byte <= MSGPACK_ARRAY_FINAL){
		return _byte - MSGPACK_ARRAY_INITIAL;
	}

	return 0;
}

unsigned int MsgPackHandler::isMap(uint8_t _byte){
	if(_byte > MSGPACK_MAP_INITIAL && _byte <= MSGPACK_MAP_FINAL){
		return _byte - MSGPACK_MAP_INITIAL;
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



/**
 * we wont use pointer to another pointer just for standards [&](){ cout << F(x)} ...
 */
//unsigned long MsgPackHandler::get32bitByte(){
//	return _32bitword;
//}
