#include <sintaxes-lib.h>

MsgPackHandler::MsgPackHandler(Responses *_responses) {
	MsgPackHandler::response = _responses;
}
;

//bool MsgPackHandler::processByte(char * _bytes){
//
////	for(byte in bytes {
////			word _byte;
////	}
//	word _byte;
//
//	MsgPackHandler::bytes_received = _bytes;
//
//	if(!MsgPackHandler::started) {
//		if(_byte != 0x02){
//			MsgPackHandler::response->writeSTXError();
//			return false;
//		}
//
//		MsgPackHandler::started = true;
//	}
//
//	return true;
//}

void MsgPackHandler::process32bitBufferByte(uint8_t _byte) {
	//		UINT8 var1 = 0x01; //0000 0001
	//		UINT8 var2 = 0x03; //0000 0011
	//		UINT8 var3 = 0x07; //0000 0111
	//		UINT8 var4 = 0x0F; //0000 1111
	//		UINT32 bigvar = (var1 << 24) + (var2 << 16) + (var3 << 8) + var4;
	_32bitword_processing = true;
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
		break;
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

bool MsgPackHandler::init(Stream *_stream, int size) {

	reset_32bit_processing();
	buffer_position = 0;
	stream = _stream;
	buffer_lenght = MsgPackHandler::stream->readBytes(LocalBuffers::client_request_buffer, size);
	buffer_bytes_remaining = buffer_lenght;
	MsgPackHandler::response->writeModule200DataHeaders();

	return true;
}

bool MsgPackHandler::processStream() {
	while (buffer_bytes_remaining > 0) {
		uint8_t _word = LocalBuffers::client_request_buffer[MsgPackHandler::buffer_position];
		response->writeByte(_word);

//		if (_32bitword_processing) {
//			if (_32bitword_remaining == 0) {
//				reset_32bit_processing();
//				response->write32bitByte(_32bitword);
//			}
//			buffer_bytes_remaining--;
//			buffer_position++;
//			continue;
//		}

		switch (_word) {
		case MSGPACK_UINT32:

			while(_32bitword_remaining > 0){
				buffer_bytes_remaining--;
				buffer_position++;
				_word = LocalBuffers::client_request_buffer[MsgPackHandler::buffer_position];
				MsgPackHandler::process32bitBufferByte(_word);
			}
			response->write32bitByte(_32bitword);
			reset_32bit_processing();
			break;
		}

		buffer_bytes_remaining--;
		buffer_position++;
	}
	return true;
}

bool MsgPackHandler::whatForNext() {

}

