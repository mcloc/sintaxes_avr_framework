/**
 * includes of the framework are all in <sintax-lieb> which is included in main.h
 * main.h also includes local project specific includes like commands.h devices.h etc
 * INITIAL PROJECT SPECIFIC #defines STAYS UNDER project-defines.h file
 * do not worray with the framework with this file, it's all yours, the framework #defines
 * is under his own headers. Change this project-defines.h as you want.
 */
#include "main.h"
#include <sintaxes-lib.h>
#include <LocalBuffers.h>
#include <Commands.h>
#include <MsgPackHandler.h>

// **** ETHERNET SETTING ****
EthernetServer server = EthernetServer(LISTENPORT);
const uint8_t mac[6] = { MACADDRESS };

static SintaxesLib sintaxes_lib;
static LocalBuffers localBuffers;
static Responses response(&localBuffers);
static Commands commands(&localBuffers, &response);
static MsgPackHandler msgpck(&response, &commands, &sintaxes_lib);
static DHT dht1 = DHT(DHT1PIN, DHTTYPE, 15);
static DHT dht2 = DHT(DHT2PIN, DHTTYPE, 15);



void setup() {
	pinMode(BUZZPIN, OUTPUT);
	sintaxes_lib._BUZZPIN = BUZZPIN;
	sintaxes_lib.buzz(800, 500);

	//Set all commands devices objects that will be need to execute commands
	//like Reading DHT, and other Arduino Objects
	commands.setDHT1(&dht1, DHT1PIN, DHTTYPE);
	commands.setDHT2(&dht2, DHT1PIN, DHTTYPE);

	// DHCP, will buzz for ever trying
	while (Ethernet.begin(mac) == 0) {
		sintaxes_lib.buzz( 8000, 400, 2);
		delay(500);
	}
	server.begin();
	sintaxes_lib.buzz( 5000, 300, 4);
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
			msgpck.init((Stream *) &client, size);
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
