/**
 * This is NOT a oficial messagePack full implementation. It's implementation goes
 * @ the END of our appliance needs. In this case we are using a new Layer 7 (Application Layer)
 * on TOP of MessagePack protocol layer.
 *
 * We call it "4Bytes COMMAND PROTOCOL". It's composed of only 32bit unsigned long (0xffff0000 to0xfffffff)
 * Note that first 2 bytes still reserved for improving the communications of 4Bytes CmdProtocol.
 *
 * This Layer is almost a implementation of RPC but built with a flow of execution designed to
 * be execution at real time processing message, with exceptions thrown @ real time trough TCP
 * client stream as JSON formated and rollback support with previous state guarded on SD Card.
 * We can response Json because all strings are stored @ flash memory with PROGMEM, so it's
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

//typedef const __FlashStringHelper* FSH;








/**
 * we will need Reponse Object to write on TCP client JSON objects and Commands to execute them on the fly.
 */
MsgPackHandler::MsgPackHandler(Responses *_responses, Commands *_commands) {
	response = _responses;
	commands = _commands;
	status = MSGPACK_STATE_IDLE;
}


/**
 * reset module communication adn get the request buffer. The size of the buffer is defined in
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
 * 			WHAT TODO. THE EXCEPTIONS MESSAGES ARE THRONW REAL TIME ON THE STREAMS
 */
bool MsgPackHandler::processStream() {
	while (buffer_bytes_remaining > 0) {
		uint8_t _byte = MsgPackHandler::next();
		response->writeByte(_byte);


		/**
		 * If it's an array or a map the LAYER 2 COMMANDS FRAMEWORK (RPC)
		 * is already been processed, remember that this machine is suppouse
		 * to interpret commands as defined per use et si.
		 */
		//THIS IS AN ARRAY - [COMMANDS FRAMEWORK] STATUS ALREAY PROCESSING
		if(int array_size = isArray(_byte) > 0){
			processArray(_byte, array_size);
			continue;
		}
		//THIS IS AN MAP- [COMMANDS FRAMEWORK] STATUS ALREAY PROCESSING
		if(int map_size = isMap(_byte) > 0){
//			processMap(_word, map_size);
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
	//return after execution of each command with status and guard the new machine state on SD Card
	//we will use SD Card as log. So must implement methods for getting this log if requested
	return true;
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
bool MsgPackHandler::processByte(uint8_t _byte) {

	switch (_byte) {
		case MSGPACK_UINT32:
		//LAYER 2 - COMMANDS FRAMEWORK PROCESSING - (RPC)
		assemble32bitByte(_byte);
		response->writeDEBUG_INT(_32bitword);
		if(!process32bitword()) {
			//TODO: response->writeProcess32bitwordERROR(); //response error on the process32bitword()
			return false;
		}
		response->write32bitByte(_32bitword); //DEBUG
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
 */
bool MsgPackHandler::process32bitword(){
	switch(_32bitword){

	case MODULE_COMMMAND_FLAG:
		uint8_t _word = next();
		assemble32bitByte(_word);
		commands->command_executing = _32bitword;
		status = MSGPACK_STATE_COMMAND_SET;
		return true;

	case MODULE_COMMMAND_GET_DATA:
		commands->command_executing = _32bitword;
		commands->get_data();
		status = MSGPACK_STATE_COMMAND_EXECUTED;
		return true;

	case MODULE_COMMMAND_EXECUTE_FLAG:
		commands->execute();
		status = MSGPACK_STATE_COMMAND_EXECUTED;
		return true;

	}
	//TODO: if 32bitword is not mapped return false; send responses [ remember, throw is always @front  each false must than be taken CARE]
	return true;
}




bool MsgPackHandler::processArray(uint8_t _word, int array_size) {
	return true; //DEBUG
	uint8_t next_word = whatNext();

	while (array_size > 0) {
		array_size--;
		uint8_t _word = next();
		response->writeByte(_word);

		//THIS IS A MAP
		if (_word > MSGPACK_MAP_INITIAL && _word <= MSGPACK_MAP_FINAL) {
			uint8_t map_size = _word - MSGPACK_MAP_INITIAL;
			while (map_size > 0) {
				uint8_t _word = next();
				map_size--;
				switch (_word) {
				case MSGPACK_UINT32:
					while (_32bitword_remaining > 0) {
						uint8_t _word = next();
						assemble32bitByte(_word);
					}
					response->write32bitByte(_32bitword); // DEBUG
					reset_32bit_processing();
					break;
				case MSGPACK_TRUE:
					response->writeByte(true);
					break;
				case MSGPACK_FALSE:
					response->writeByte(false);
					break;
				}
			}
		}
	}

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
void MsgPackHandler::assemble32bitByte(uint8_t _byte) {


	if(_byte != MSGPACK_UINT32) {
		error_code = ERROR_32BIT_PROCESSING;
		response->writeProcess32bitwordERROR();
		return;
	}
	reset_32bit_processing();
	_32bitword_processing = true;
	status = MSGPACK_STATE_WORKING_32BIT;
	while(_32bitword_remaining > 0){
		uint8_t _byte = MsgPackHandler::next();
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
				_32bitword = (_32bitword_array[0] << 24) +
					(_32bitword_array[1] << 16) + (_32bitword_array[2] << 8) + _byte;
				_32bitword_remaining--;
				status = prev_status;
				break;
			}
	}

}

void MsgPackHandler::reset_32bit_processing() {
	_32bitword_remaining = 4; // 4 bytes for 32bit unsigned long
	_32bitword_processing = false;
	_32bitword = '\0';
	_32bitword_array[0] = '\0';
	_32bitword_array[1] = '\0';
	_32bitword_array[2] = '\0';
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





/**
 * we wont use pointer to another pointer just for standards [&](){ cout << F(x)} ...
 */
//unsigned long MsgPackHandler::get32bitByte(){
//	return _32bitword;
//}
