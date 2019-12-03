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



//######  T E S T I N G   T H E   N E W    BMP280 S E N S O R S
#include <Adafruit_BME280.h>
#include <Wire.h>


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
static DN20 dn20_2 = DN20(MODULE_ACTUATOR_DN20_1_2, BLUE_LED);
static DN20 dn20_3 = DN20(MODULE_ACTUATOR_DN20_1_3, GREEN_LED);
//static DN20 dn20_4 = DN20(MODULE_ACTUATOR_DN20_1_4, RED_LED);

//Just to initializate, we will use the pointer only no the object
//static ApplianceMemmoryHandler memory_handler = ApplianceMemmoryHandler();

///////////////////////////////D E B U G   T H E   BMP280 ///////////////////////////////////

#define SEALEVELPRESSURE_HPA (1013.25)
static Adafruit_BME280 BME280; // I2C
unsigned long delayTime;
//////////////////////////////////////////////////////////////////////////////////////////////



static uint32_t total_requests = 0;

void setup() {
	///////////////////////////////D E B U G   T H E   BMP280 ///////////////////////////////////
	Serial.begin(115200);
	    Serial.println(F("BME280 test"));

	    if (! BME280.begin(0x77)) {
	        Serial.println("Could not find a valid BMP280 sensor, check wiring!");
	        while (1);
	    }

	    // humidity sensing
	        Serial.println("-- Humidity Sensing Scenario --");
//	        Serial.println("forced mode, 1x temperature / 1x humidity / 0x pressure oversampling");
	        Serial.println("normal mode, 16x pressure / 2x temperature / 1x humidity oversampling,");
	        Serial.println("= pressure off, filter off");
	        BME280.setSampling(Adafruit_BME280::MODE_NORMAL,
                    Adafruit_BME280::SAMPLING_X16, // temperature
                    Adafruit_BME280::SAMPLING_X16, // pressure
                    Adafruit_BME280::SAMPLING_X16, // humidity
					Adafruit_BME280::FILTER_X16,
					Adafruit_BME280::STANDBY_MS_0_5);

	        // suggested rate is 1Hz (1s)

	           Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
	           Serial.print("SensorID was: 0x");
	           Serial.println(BME280.sensorID(),16);
	           Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
	           Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
	           Serial.print("        ID of 0x60 represents a BME 280.\n");
	           Serial.print("        ID of 0x61 represents a BME 680.\n");

	       Serial.println("-- Default Test --");

	    //BMP280
//	    Serial.println("-- Default Test --");
//	    Serial.println("normal mode, 16x oversampling for all, filter off,");
//	    Serial.println("0.5ms standby period");
//	    delayTime = 5000;
//
//
//	    /* Default settings from datasheet. */
//	    BMP280.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
//	                      Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
//	                      Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
//	                      Adafruit_BMP280::FILTER_X16,      /* Filtering. */
//	                      Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */




	//////////////////////////////////////////////////////////////////////////////////////////////

	//RESET THE DHT#1 SENSOR
	digitalWrite(DHT1PIN, LOW); // sets output to gnd
	pinMode(DHT1PIN, OUTPUT); // switches power to DHT on
	delay(1200); // delay necessary after power up for DHT to stabilize
	dht1.begin();

	//RESET THE DHT#1 SENSOR
	digitalWrite(DHT2PIN, LOW); // sets output to gnd
	pinMode(DHT2PIN, OUTPUT); // switches power to DHT on
	delay(1200); // delay necessary after power up for DHT to stabilize
	dht2.begin();




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
	pinMode(BLUE_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(BUZZPIN, OUTPUT);
//	memory_handler.sintaxes_lib->setLed(RED_LED, LOW);
//	memory_handler.sintaxes_lib->setLed(LED_BUILTIN, LOW);
//	memory_handler.sintaxes_lib->_BUZZPIN= BUZZPIN;
//	memory_handler.sintaxes_lib->buzz(800, 500);
	sintaxes_lib.setLed(RED_LED, LOW);
	sintaxes_lib.setLed(BLUE_LED, LOW);
	sintaxes_lib.setLed(GREEN_LED, LOW);
	sintaxes_lib.setLed(LED_BUILTIN, LOW);
	sintaxes_lib._BUZZPIN= BUZZPIN;
	sintaxes_lib.buzz(800, 500);



	//Set machine state
	machine_state.init();
	machine_state.addActuator(&ApplianceMemmoryHandler::dn20_1);
	machine_state.addActuator(&ApplianceMemmoryHandler::dn20_2);
	machine_state.addActuator(&ApplianceMemmoryHandler::dn20_3);


	sintaxes_lib.blink(RED_LED, 200, 1);
	sintaxes_lib.blink(BLUE_LED, 200, 1);
	sintaxes_lib.blink(GREEN_LED, 200, 1);
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
	 Serial.print("Temperature = ");
	    Serial.print(BME280.readTemperature());
	    Serial.println(" *C");

	    Serial.print("Pressure = ");

	    Serial.print(BME280.readPressure() / 100.0F);
	    Serial.println(" hPa");

//	    Serial.print("Approx. Altitude = ");
//	    Serial.print(BME280.readAltitude(SEALEVELPRESSURE_HPA));
//	    Serial.println(" m");

	    Serial.print("Humidity = ");
	    Serial.print(BME280.readHumidity());
	    Serial.println(" %");

	    Serial.println();
    // For more details on the following scenarious, see chapter
      // 3.5 "Recommended modes of operation" in the datasheet

  /*
      // weather monitoring
      Serial.println("-- Weather Station Scenario --");
      Serial.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
      Serial.println("filter off");
      bme.setSampling(Adafruit_BME280::MODE_FORCED,
                      Adafruit_BME280::SAMPLING_X1, // temperature
                      Adafruit_BME280::SAMPLING_X1, // pressure
                      Adafruit_BME280::SAMPLING_X1, // humidity
                      Adafruit_BME280::FILTER_OFF   );

      // suggested rate is 1/60Hz (1m)
      delayTime = 60000; // in milliseconds
  */

  /*
      // humidity sensing
      Serial.println("-- Humidity Sensing Scenario --");
      Serial.println("forced mode, 1x temperature / 1x humidity / 0x pressure oversampling");
      Serial.println("= pressure off, filter off");
      bme.setSampling(Adafruit_BME280::MODE_FORCED,
                      Adafruit_BME280::SAMPLING_X1,   // temperature
                      Adafruit_BME280::SAMPLING_NONE, // pressure
                      Adafruit_BME280::SAMPLING_X1,   // humidity
                      Adafruit_BME280::FILTER_OFF );

      // suggested rate is 1Hz (1s)
      delayTime = 1000;  // in milliseconds
  */

  /*
      // indoor navigation
      Serial.println("-- Indoor Navigation Scenario --");
      Serial.println("normal mode, 16x pressure / 2x temperature / 1x humidity oversampling,");
      Serial.println("0.5ms standby period, filter 16x");
      bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                      Adafruit_BME280::SAMPLING_X2,  // temperature
                      Adafruit_BME280::SAMPLING_X16, // pressure
                      Adafruit_BME280::SAMPLING_X1,  // humidity
                      Adafruit_BME280::FILTER_X16,
                      Adafruit_BME280::STANDBY_MS_0_5 );

      // suggested rate is 25Hz
      // 1 + (2 * T_ovs) + (2 * P_ovs + 0.5) + (2 * H_ovs + 0.5)
      // T_ovs = 2
      // P_ovs = 16
      // H_ovs = 1
      // = 40ms (25Hz)
      // with standby time that should really be 24.16913... Hz
      delayTime = 41;
      */

      /*
      // gaming
      Serial.println("-- Gaming Scenario --");
      Serial.println("normal mode, 4x pressure / 1x temperature / 0x humidity oversampling,");
      Serial.println("= humidity off, 0.5ms standby period, filter 16x");
      bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                      Adafruit_BME280::SAMPLING_X1,   // temperature
                      Adafruit_BME280::SAMPLING_X4,   // pressure
                      Adafruit_BME280::SAMPLING_NONE, // humidity
                      Adafruit_BME280::FILTER_X16,
                      Adafruit_BME280::STANDBY_MS_0_5 );

      // Suggested rate is 83Hz
      // 1 + (2 * T_ovs) + (2 * P_ovs + 0.5)
      // T_ovs = 1
      // P_ovs = 4
      // = 11.5ms + 0.5ms standby
      delayTime = 12;
  */

	Serial.println();
	delay(1000);

	return;




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
