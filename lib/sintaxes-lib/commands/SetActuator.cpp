/*
 * SetActuator.cpp
 *
 *  Created on: 21 de out de 2019
 *      Author: mcloc
 */

#include <commands/SetActuator.h>

SetActuator::SetActuator(Responses *_reponse) {
	response = _reponse;
}

bool SetActuator::execute(){
	if(!checkArguments()){
		return false;
	}
}

bool SetActuator::checkArguments(){
	if(state == '\0'|| state_duration == '\0') {
//		error_code = ERROR_COMMAND_EXECUTION_FAILED;
		response->write4BCPCommandArgsMissing();
		return false;
	}


	return true;
}
