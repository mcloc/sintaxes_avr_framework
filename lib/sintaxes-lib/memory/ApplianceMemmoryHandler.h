/*
 * ApplianceMemmoryHandler.h
 *
 *  Created on: 25 de out de 2019
 *      Author: mcloc
 */

#ifndef LIB_SINTAXES_LIB_MEMORY_APPLIANCEMEMMORYHANDLER_H_
#define LIB_SINTAXES_LIB_MEMORY_APPLIANCEMEMMORYHANDLER_H_

#include <sintaxes-lib.h>
#include <LocalBuffers.h>
#include <MachineState.h>
#include <Responses.h>
#include <commands/CommandsHandler.h>
#include <msgpack/MsgPackHandler.h>
#include <4BCProtocol/4BCPContainer.h>
#include <DHT.h>
#include <Adafruit_BME280.h>
#include <commands/SetActuatorCommand.h>


class ApplianceMemmoryHandler {
public:
	ApplianceMemmoryHandler();
	static  SintaxesLib *sintaxes_lib;
	static LocalBuffers *localBuffers;
	static  MachineState *machine_state;
	static  Responses *responses;
	static  CommandsHandler *commands_handler;
	static  MsgPackHandler *msgpack_handler;
	static  _4BCPContainer *container_4BCP;

	//Appliance specific allocations
	static  DHT *dht1;
	static  DHT *dht2;
	static  Adafruit_BME280 *BME280_1;
	static  Adafruit_BME280 *BME280_2;

	static ActuatorBase *dn20_1;
	static ActuatorBase *dn20_2;
	static ActuatorBase *dn20_3;

//	static uint32_t total_requests;
//	static long unsigned int loop_time;
//	static long unsigned int uptime;

//	static SetActuatorCommand *command_set_actuator;

//	static void allocSetActuatorCommand(uint8_t device_key);
	bool request_to_write();
	void unlock();
	static void newLoop();
private:
	static bool lock;


};

#endif /* LIB_SINTAXES_LIB_MEMORY_APPLIANCEMEMMORYHANDLER_H_ */
