/*
ApplianceMemmoryHandler.cpp

Created on: 25 de out de 2019
Author: mcloc
*/

#include <memory/ApplianceMemmoryHandler.h>

SintaxesLib *ApplianceMemmoryHandler::sintaxes_lib = (SintaxesLib*) malloc(sizeof(SintaxesLib));
LocalBuffers *ApplianceMemmoryHandler::localBuffers = (LocalBuffers*) malloc(
		sizeof(LocalBuffers));
MachineState *ApplianceMemmoryHandler::machine_state = (MachineState*) malloc(
		sizeof(MachineState));
Responses *ApplianceMemmoryHandler::responses = (Responses*) malloc(
		sizeof(Responses));
CommandsHandler *ApplianceMemmoryHandler::commands_handler =
		(CommandsHandler*) malloc(sizeof(CommandsHandler));
MsgPackHandler *ApplianceMemmoryHandler::msgpack_handler =
		(MsgPackHandler*) malloc(sizeof(MsgPackHandler));
_4BCPContainer *ApplianceMemmoryHandler::container_4BCP =
		(_4BCPContainer*) malloc(sizeof(_4BCPContainer));

//Appliance specific allocations
DHT *ApplianceMemmoryHandler::dht1 = (DHT*) malloc(sizeof(DHT));
DHT *ApplianceMemmoryHandler::dht2 = (DHT*) malloc(sizeof(DHT));

ActuatorBase *ApplianceMemmoryHandler::dn20_1= (ActuatorBase*) malloc(sizeof(ActuatorBase));
ActuatorBase *ApplianceMemmoryHandler::dn20_2= (ActuatorBase*) malloc(sizeof(ActuatorBase));
ActuatorBase *ApplianceMemmoryHandler::dn20_3= (ActuatorBase*) malloc(sizeof(ActuatorBase));

ApplianceMemmoryHandler::ApplianceMemmoryHandler() {


}

