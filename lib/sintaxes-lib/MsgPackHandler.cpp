#include <sintaxes-lib.h>

MsgPackHandler::MsgPackHandler(){};

bool MsgPackHandler::processByte(char * _byte){
	byte_received = _byte;

	return true;
}


