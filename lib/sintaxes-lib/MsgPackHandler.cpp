#include <sintaxes-lib.h>

MsgPackHandler::MsgPackHandler(Responses * _responses){
	MsgPackHandler::response = _responses;
};

bool MsgPackHandler::processByte(char * _byte){
	MsgPackHandler::byte_received = _byte;

	if(!MsgPackHandler::started) {
		if(_byte != 0x02){
			MsgPackHandler::response->writeSTXError();
			return false;
		}

		MsgPackHandler::started = true;
	}

	return true;
}

bool MsgPackHandler::whatForNext(){

}


