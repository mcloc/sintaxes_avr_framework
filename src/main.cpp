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

// **** ETHERNET SETTING ****
EthernetServer server = EthernetServer(LISTENPORT);
uint8_t mac[6] = { MACADDRESS };

static LocalBuffers localBuffers;
static Commands commands;
static Responses response(&commands);
MsgPackHandler msgpck(&response);
static DHT dht1 = DHT(DHT1PIN, DHTTYPE, 15);
static DHT dht2 = DHT(DHT2PIN, DHTTYPE, 15);



void setup() {
	pinMode(BUZZPIN, OUTPUT);
	buzz(BUZZPIN, 800, 500);
	commands.setDHT1(&dht1, DHT1PIN, DHTTYPE);
	commands.setDHT2(&dht2, DHT1PIN, DHTTYPE);

// DHCP, will buzz for ever trying
	while (Ethernet.begin(mac) == 0) {
		buzz(BUZZPIN, 8000, 400, 2);
		delay(500);
	}
	server.begin();
	buzz(BUZZPIN, 5000, 300, 4);
}

void loop() {

	size_t size;
	while (EthernetClient client = server.available()) {
		buzz(BUZZPIN, 8000, 200, 1);
		while ((size = client.available()) > 0) {
			response.setClient(&client);
			if(size > MAX_SIZE_ALLOWED_REQUEST){
				response.writeError_MAX_SIZE_REQUEST();
				break;
			}
//			uint8_t *msg = (uint8_t*) malloc(size);9
//			size = client.readBytes(LocalBuffers::client_request_buffer, size);

			msgpck.init((Stream *) &client, size);
			msgpck.processStream();


//			client.write(LocalBuffers::client_request_buffer, size);
		}
		client.stop();
	}
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
