/**
 * ApplianceMemmoryHandler.cpp
 * Memory Buffer, defined by each application, try put all classes and data structures in initialization;
 *N O   D  Y N A M I C   A L L O C A T I O N - at least most effort
 *
 *
 *
 * Created on: 25 de out de 2019
 * Author: mcloc
 */

#include <memory/ApplianceMemmoryHandler.h>
#include <MsgPackHandler.h>
#include <4BCProtocol/4BCPContainer.h>
#include <commands/CommandsHandler.h>
#include <Responses.h>
#include <MachineState.h>
#include <LocalBuffers.h>
//#include <memory>
//#include <memory>
//#include <estd/memory.h>

//THIS IS SUPPOUSED TO BE ALLOCATED IN THE ROM
SintaxesLib *ApplianceMemmoryHandler::sintaxes_lib = (SintaxesLib*) malloc(
		sizeof(SintaxesLib*));
LocalBuffers *ApplianceMemmoryHandler::localBuffers = (LocalBuffers*) malloc(
		sizeof(LocalBuffers*));
MachineState *ApplianceMemmoryHandler::machine_state = (MachineState*) malloc(
		sizeof(MachineState*));

Responses *ApplianceMemmoryHandler::responses = (Responses*) malloc(
		sizeof(Responses*));
CommandsHandler *ApplianceMemmoryHandler::commands_handler =
		(CommandsHandler*) malloc(sizeof(CommandsHandler*));
MsgPackHandler *ApplianceMemmoryHandler::msgpack_handler =
		(MsgPackHandler*) malloc(sizeof(MsgPackHandler*));
_4BCPContainer *ApplianceMemmoryHandler::container_4BCP =
		(_4BCPContainer*) malloc(sizeof(_4BCPContainer*));

//*Appliance specific allocations
DHT *ApplianceMemmoryHandler::dht1 = (DHT*) malloc(sizeof(DHT));
DHT *ApplianceMemmoryHandler::dht2 = (DHT*) malloc(sizeof(DHT));
Adafruit_BME280 *ApplianceMemmoryHandler::BME280_1 = (Adafruit_BME280*) malloc(sizeof(Adafruit_BME280));
Adafruit_BME280 *ApplianceMemmoryHandler::BME280_2 = (Adafruit_BME280*) malloc(sizeof(Adafruit_BME280));

ActuatorBase *ApplianceMemmoryHandler::dn20_1 = (ActuatorBase*) malloc(
		sizeof(ActuatorBase*));
ActuatorBase *ApplianceMemmoryHandler::dn20_2 = (ActuatorBase*) malloc(
		sizeof(ActuatorBase*));
ActuatorBase *ApplianceMemmoryHandler::dn20_3 = (ActuatorBase*) malloc(
		sizeof(ActuatorBase*));


////Command Classe::
//SetActuatorCommand *ApplianceMemmoryHandler::command_set_actuator ;




bool ApplianceMemmoryHandler::lock;
//uint32_t* ApplianceMemmoryHandler::total_requests = (uint32_t*) malloc(sizeof(uint32_t*));
//long unsigned int* ApplianceMemmoryHandler::loop_time = (long unsigned int*) malloc(sizeof(long unsigned int*));
//uint32_t* ApplianceMemmoryHandler::uptime = (uint32_t*) malloc(sizeof(uint32_t*));
//uint32_t ApplianceMemmoryHandler::total_requests = 0;
//long unsigned int ApplianceMemmoryHandler::loop_time = 0;
//long unsigned int ApplianceMemmoryHandler::uptime = 0;

//static void ApplianceMemmoryHandler::allocSetActuatorCommand(uint8_t device_key) {
//	ApplianceMemmoryHandler::command_set_actuator = new SetActuatorCommand(device_key);
//}

ApplianceMemmoryHandler::ApplianceMemmoryHandler() {
	lock = false;

	//temporary pointer address
	//It must be setted in the main Setup() on be one
//	container_4BCP = &_4BCPContainer();
//	localBuffers = &LocalBuffers();
//	machine_state = &MachineState();
//	responses = &Responses();
//	commands_handler = &CommandsHandler();
//	msgpack_handler = &MsgPackHandler();
}

void ApplianceMemmoryHandler::newLoop() {
	ApplianceMemmoryHandler::localBuffers->float2char_buffer1[0] = '\0';
	ApplianceMemmoryHandler::localBuffers->float2char_buffer2[0] = '\0';
	ApplianceMemmoryHandler::localBuffers->string_cpy_buffer[0] = '\0';
	ApplianceMemmoryHandler::commands_handler->reset();
	ApplianceMemmoryHandler::msgpack_handler->reset();
//	ApplianceMemmoryHandler::uptime = millis() /1000 /60;

//	ApplianceMemmoryHandler::machine_state->clean();
	lock = false;
}

