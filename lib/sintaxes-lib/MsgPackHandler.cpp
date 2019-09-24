#include <sintaxes-lib.h>

MsgPackHandler::MsgPackHandler(Responses *_responses) {
	MsgPackHandler::response = _responses;
}


void MsgPackHandler::assemble32bitByte(uint8_t _byte) {
	//		UINT8 var1 = 0x01; //0000 0001
	//		UINT8 var2 = 0x03; //0000 0011
	//		UINT8 var3 = 0x07; //0000 0111
	//		UINT8 var4 = 0x0F; //0000 1111
	//		UINT32 bigvar = (var1 << 24) + (var2 << 16) + (var3 << 8) + var4;

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
//		response->writeByte(_byte);
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

bool MsgPackHandler::init(Stream *_stream, int size, Commands *_commands) {
	commands = _commands;
	status = MSGPACK_STATE_BEGIN;
	reset_32bit_processing();
	buffer_position = 0;
	stream = _stream;
	buffer_lenght = MsgPackHandler::stream->readBytes(LocalBuffers::client_request_buffer, size);
	buffer_bytes_remaining = buffer_lenght;

	MsgPackHandler::response->writeModule200DataHeaders();

	return true;
}

unsigned long MsgPackHandler::get32bitByte(){
	return _32bitword;
}

bool MsgPackHandler::process32bitword(){
	switch(get32bitByte()){
	case MODULE_COMMMAND_FLAG:
		uint8_t _word = MsgPackHandler::next();
		MsgPackHandler::assemble32bitByte(_word);
		commands->command_executing = get32bitByte();
		status = MSGPACK_STATE_COMMAND_SETED;
		return true;
		break;
	}

	//TODO: if 32bitword is not mapped return false;
	return true;
}

bool MsgPackHandler::processStream() {
	while (buffer_bytes_remaining > 0) {
		uint8_t _word = MsgPackHandler::next();
		response->writeByte(_word);

		//THIS IS AN ARRAY
		if(int array_size = MsgPackHandler::isArray(_word) > 0){
//			MsgPackHandler::processArray(_word, array_size);
			continue;
		}
		//THIS IS AN MAP
		if(int map_size = MsgPackHandler::isMap(_word) > 0){
//			MsgPackHandler::processArray(_word, array_size);
			continue;
		}


		switch (_word) {
//		case isArray(_word):
//				response->client->println(F("must implement array type."));
//				break;
//		case isMap(_word):
//				response->client->println(F("must implement map type."));
//				break;
		case MSGPACK_UINT32:
			MsgPackHandler::assemble32bitByte(_word);
			response->writeDEBUG_INT(_32bitword);
			if(!MsgPackHandler::process32bitword()) {
				//response->writeProcess32bitwordERROR(); //response error on the process32bitword()
//				response->writeDEBUG_CHAR((const __FlashStringHelper*) F("erro no process 32bit"));
				return false;
			}
			response->write32bitByte(_32bitword);
			reset_32bit_processing();
			break;
		case MSGPACK_TRUE:
			response->writeByte(true);
			break;
		case MSGPACK_FALSE:
			response->writeByte(false);
			break;

		default:
			error_code = ERROR_MSGPACK_PROCESSING;
			response->writeMsgPackError(_word);
			return false;
		}
	}
	return true;
}

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

bool MsgPackHandler::processArray(uint8_t _word, int array_size) {

	return true; //DEBUG
	uint8_t next_word = MsgPackHandler::whatForNext();

	while (array_size > 0) {
		array_size--;
		uint8_t _word = MsgPackHandler::next();
		response->writeByte(_word);

		//THIS IS A MAP
		if (_word > MSGPACK_MAP_INITIAL && _word <= MSGPACK_MAP_FINAL) {
			uint8_t map_size = _word - MSGPACK_MAP_INITIAL;
			while (map_size > 0) {
				uint8_t _word = MsgPackHandler::next();
				map_size--;
				switch (_word) {
				case MSGPACK_UINT32:
					while (_32bitword_remaining > 0) {
						uint8_t _word = MsgPackHandler::next();
						MsgPackHandler::assemble32bitByte(_word);
					}
					response->write32bitByte(_32bitword);
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

	return true;
}

uint8_t MsgPackHandler::whatForNext() {
	LocalBuffers::client_request_buffer[MsgPackHandler::buffer_position+1];
}

uint8_t MsgPackHandler::next(){
	buffer_bytes_remaining--;
	buffer_position++;
	last_byte = LocalBuffers::client_request_buffer[MsgPackHandler::buffer_position];
	return last_byte;
}

