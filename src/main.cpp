/**
 *
 * deepest HW initiation, main firmware loop, must be as memory efficient as hell -- D E E P --
 *
 * Here is the place for 4CBP data package struct, - no no ...-;
 *
 * includes of the framework are all in <sintax-lib> which is included in main.h
 * main.h also includes local project specific includes like commands.h devices.h etc
 * INITIAL PROJECT SPECIFIC #defines STAYS UNDER project-defines.h file
 * do not worry with the framework with in this file, it's all yours, the framework #defines
 * is under his own headers. Change project-defines.h as you want.
 */
#include "main.h"
#include <sintaxes-lib.h>
#include <LocalBuffers.h>
#include <MachineState.h>
#include <devices/types/DN20.h>
#include <MsgPackHandler.h>
#include <4BCProtocol/4BCPContainer.h>
#include <commands/CommandsHandler.h>
#include <memory/ApplianceMemmoryHandler.h>
#include <stdlib.h>


// **** ETHERNET SETTING ****
static EthernetServer server = EthernetServer(LISTENPORT);
static EthernetClient *etherClient = (EthernetClient*)malloc(sizeof(EthernetClient));
const uint8_t mac[6] = { MACADDRESS };

//DEBUG DHCPH is commented for debug proposes uncomment it for production
IPAddress ip(192,168,1,16);




static _4BCPContainer container_4BCP = _4BCPContainer();
static LocalBuffers localBuffers = LocalBuffers();
static MachineState machine_state = MachineState();
static Responses responses = Responses();
static CommandsHandler commands_handler = CommandsHandler();
static MsgPackHandler msgpack_handler = MsgPackHandler();
static SintaxesLib sintaxes_lib = SintaxesLib();

static DHT dht1 = DHT(DHT1PIN, DHTTYPE, 15);
static DHT dht2 = DHT(DHT2PIN, DHTTYPE, 15);


//TODO: assemble another 2 leds for the 3 actuators
static DN20 dn20_1 = DN20(MODULE_ACTUATOR_DN20_1_1, RED_LED);
static DN20 dn20_2 = DN20(MODULE_ACTUATOR_DN20_1_2, RED_LED);
static DN20 dn20_3 = DN20(MODULE_ACTUATOR_DN20_1_3, RED_LED);
static DN20 dn20_4 = DN20(MODULE_ACTUATOR_DN20_1_4, RED_LED);

//Just to initializate, we will use the pointer only no the object
//static ApplianceMemmoryHandler memory_handler = ApplianceMemmoryHandler();


static uint32_t total_requests = 0;

void setup() {
	ApplianceMemmoryHandler::container_4BCP = &container_4BCP;
	ApplianceMemmoryHandler::localBuffers = &localBuffers;
	ApplianceMemmoryHandler::machine_state = &machine_state;
	ApplianceMemmoryHandler::responses = &responses;
	ApplianceMemmoryHandler::commands_handler = &commands_handler;
	ApplianceMemmoryHandler::msgpack_handler = &msgpack_handler;
	ApplianceMemmoryHandler::sintaxes_lib = &sintaxes_lib;
	ApplianceMemmoryHandler::dht1 = &dht1;
	ApplianceMemmoryHandler::dht2 = &dht2;
	ApplianceMemmoryHandler::dn20_1 = &dn20_1;
	ApplianceMemmoryHandler::dn20_2 = &dn20_2;
	ApplianceMemmoryHandler::dn20_3 = &dn20_3;


	pinMode(RED_LED, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(BUZZPIN, OUTPUT);
//	memory_handler.sintaxes_lib->setLed(RED_LED, LOW);
//	memory_handler.sintaxes_lib->setLed(LED_BUILTIN, LOW);
//	memory_handler.sintaxes_lib->_BUZZPIN= BUZZPIN;
//	memory_handler.sintaxes_lib->buzz(800, 500);
	sintaxes_lib.setLed(RED_LED, LOW);
	sintaxes_lib.setLed(LED_BUILTIN, LOW);
	sintaxes_lib._BUZZPIN= BUZZPIN;
	sintaxes_lib.buzz(800, 500);



	//Set machine state
	machine_state.init();
	machine_state.addActuator(&ApplianceMemmoryHandler::dn20_1);
	machine_state.addActuator(&ApplianceMemmoryHandler::dn20_2);
	machine_state.addActuator(&ApplianceMemmoryHandler::dn20_3);


	sintaxes_lib.blink(RED_LED, 200, 3);
	// DHCP, will buzz for ever trying
//	while (Ethernet.begin(mac) == 0) {
//		sintaxes_lib.buzz( 8000, 400, 2);
//		delay(500);
//	}

	Ethernet.begin(mac,ip);

//
	server.begin();
	sintaxes_lib.buzz( 5000, 300, 4);
//	sintaxes_lib.blink(BOARD_LED, 200, 4);
}

void loop() {
	size_t size;
	while (EthernetClient client = server.available()) {
		etherClient = &client;
		total_requests++;
		uint32_t start = millis();
		sintaxes_lib.buzz( 8000, 80, 1);
		while ((size = etherClient->available()) > 0) {
//			responses.setClient(etherClient);
			Responses::client = etherClient;
			if(size > MAX_SIZE_ALLOWED_REQUEST){
				responses.writeError_MAX_SIZE_REQUEST();
				sintaxes_lib.buzz( 500, 200, 3);
				break;
			}

			if(size == 0){
				responses.writeError_MAL_FORMED_REQUEST();
				sintaxes_lib.buzz( 500, 200, 3);
				break;
			}

			//TODO: deal with headers and URL arguments, it must be raw msgpack 4BCP

			//MsgPackHandler: deserialize 4Bytes Command Protocol (4BCP) over the MessagePack Messages
			//[check 4BCP specs Documentation for more information]
			if(!msgpack_handler.init((Stream *) etherClient, size)){
				responses.writeError_on_INIT();
				sintaxes_lib.buzz( 500, 200, 3);
				//TODO:roolback machine state from SD Card
				break;
			}

			//TODO: save previous state on SD Card, and LOG the request
			if(msgpack_handler.processStream()){
				ApplianceMemmoryHandler::responses->writeTotalRequests(total_requests, (millis() - start), (millis()/1000/60));
//				ApplianceMemmoryHandler::responses->closeJsonResponse();
//				sintaxes_lib.buzz( 3000, 200, 1);
				//TODO:save the new state on SD Card and log executions, and a break;
				//break;
			} else {
				responses.writeErrorProcessingStream();
				sintaxes_lib.buzz( 500, 200, 3);
				//TODO:roolback machine state from SD Card
				break;

			}
//			client.write(LocalBuffers::client_request_buffer, size);
		}

		etherClient->flush();
		etherClient->stop();
		ApplianceMemmoryHandler::newLoop();
	}
	delay(20);
}




//char * snfloat(char * buffer, uint8_t len, uint8_t decimals, float value) {
//
//    bool negative = value < 0;
//
//    uint32_t mul = 1;
//    for (uint8_t i=0; i<decimals; i++) mul *= 10;
//
//    value = abs(value);
//    uint32_t value_int = int(value);
//    uint32_t value_dec = int((value - value_int) * mul);
//
//    char format[20];
//    snprintf(format, sizeof(format), "%s%%lu.%%0%ulu", negative ? "-" : "", decimals);
//    snprintf(buffer, len, format, value_int, value_dec);
//
//    return buffer;
//
//}
