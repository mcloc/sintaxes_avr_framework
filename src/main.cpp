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



// **** ETHERNET SETTING ****
static EthernetServer server = EthernetServer(LISTENPORT);
const uint8_t mac[6] = { MACADDRESS };

//DEBUG DHCPH is commented for debug proposes uncomment it for production
IPAddress ip(192,168,1,16);


//static  SintaxesLib sintaxes_lib;
//static LocalBuffers localBuffers;
//static  MachineState machine_state = MachineState();
//static  Responses response(&localBuffers);
//static  CommandsHandler commands(&localBuffers, &response);
//static  MsgPackHandler msgpck(&response, &commands, &sintaxes_lib);
//static  DHT dht1 = DHT(DHT1PIN, DHTTYPE, 15);
//static  DHT dht2 = DHT(DHT2PIN, DHTTYPE, 15);
//static  _4BCPContainer container_4BCP;

static ApplianceMemmoryHandler memory_handler = ApplianceMemmoryHandler();


//INITIALIZATION OF DEVICES
//static ActuatorBase *dn20_1 = (ActuatorBase *)malloc(sizeof(ActuatorBase));
//static ActuatorBase *dn20_2 = (ActuatorBase *)malloc(sizeof(ActuatorBase));
//static ActuatorBase *dn20_3 = (ActuatorBase *)malloc(sizeof(ActuatorBase));





void setup() {


	pinMode(RED_LED, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(BUZZPIN, OUTPUT);
	memory_handler.sintaxes_lib->setLed(RED_LED, LOW);
	memory_handler.sintaxes_lib->setLed(LED_BUILTIN, LOW);
	memory_handler.sintaxes_lib->_BUZZPIN= BUZZPIN;
	memory_handler.sintaxes_lib->buzz(800, 500);


	//Set machine state
	memory_handler.machine_state->init();

	//Set all actuators MAX_ACTUATORS define in sintaxes-framwork.h

//	if(!(*machine_state_ptr)->addActuator(&dn20_2)){
//		while(true) {
//			sintaxes_lib.buzz(400, 500, 5);
//			delay(2000);
//		}
//	}
//
//	if(!(*machine_state_ptr)->addActuator(&dn20_3)){
//		while(true) {
//			sintaxes_lib.buzz(400, 500, 5);
//			delay(2000);
//		}


	memory_handler.sintaxes_lib->blink(RED_LED, 200, 3);
	// DHCP, will buzz for ever trying
//	while (Ethernet.begin(mac) == 0) {
//		sintaxes_lib.buzz( 8000, 400, 2);
//		delay(500);
//	}

	Ethernet.begin(mac,ip);

//
	server.begin();
	memory_handler.sintaxes_lib->buzz( 5000, 300, 4);
//	sintaxes_lib.blink(BOARD_LED, 200, 4);
}

void loop() {
	size_t size;
	while (EthernetClient client = server.available()) {
		sintaxes_lib.buzz( 8000, 200, 1);
		while ((size = client.available()) > 0) {
			response.setClient(&client);
			if(size > MAX_SIZE_ALLOWED_REQUEST){
				response.writeError_MAX_SIZE_REQUEST();
				break;
			}

			if(size == 0){
				response.writeError_MAL_FORMED_REQUEST();
				break;
			}

			//TODO: deal with headers and URL arguments, it must be raw msgpack 4BCP

			//MsgPackHandler: deserialize 4Bytes Command Protocol (4BCP) over the MessagePack Messages
			//[check 4BCP specs Documentation for more information]
			msgpck.init((Stream *) &client, size, &machine_state);
			//TODO: save previous state on SD Card, and LOG the request
			if(msgpck.processStream()){
				//TODO:save the new state on SD Card and log executions, and a break;
				//break;
			} else {
				//TODO:roolback machine state from SD Card
				break;
			}
//			client.write(LocalBuffers::client_request_buffer, size);
		}
		client.stop();
	}

//	LocalBuffers::reset()
	LocalBuffers::float2char_buffer1[0] = '\0';
	LocalBuffers::float2char_buffer2[0] = '\0';
	LocalBuffers::string_cpy_buffer[0] = '\0';
	delay(2);
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
