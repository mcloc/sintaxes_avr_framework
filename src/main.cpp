#include "main.h"
#include <MessagePack.h>
//#include <sintaxes-lib.h>
//#include <char_array_buffer.hpp>
//#include <msgpack.hpp>
//#include <msgpck.h>
//#include "commands_map.h"
//#include "devices.h"
//#include <EchoServer.h>
//#include <Responses.h>
//#include <StandardCplusplus.h>
//#include <boost_1_51_0.h>
//#include <boost/shared_array.hpp>
//#include <istream>
//#include <DHT.h>
//#include <EtherCard.h>

#define STX 0x02
#define ETX 0x03
#define MAX_SIZE_ALLOWED_REQUEST 180 // 180 bytes allowed in post get incomming requests
#define GREEN_LED 6
#define DHT1PIN 7
#define DHT2PIN 8
//#define DHTTYPE DHT21   // DHT 11
#define BUZZPIN A0

// **** ETHERNET SETTING ****
static const uint8_t CS_PIN = 10; //Put here what pin you are using for your ENC28J60's chip select
static byte mac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };
static byte Ethernet::buffer[500]; // TCP/IP send and receive buffer
static BufferFiller bfill;

static Commands commands;
static Responses response(&commands, &bfill);
static EchoServer TCPServer(&commands, &response);
static DHT dht1 = DHT(DHT1PIN, DHTTYPE, 15);
static DHT dht2 = DHT(DHT2PIN, DHTTYPE, 15);
// Create a message pack object, defaults to 100 bytes of memory
//#define ONLY_DECLARATIONS
MessagePack *message = new MessagePack();
//StaticJsonDocument<500> doc;

//static char server_post_buffer[MAX_SIZE_ALLOWED_REQUEST];

char * snfloat(char * buffer, uint8_t len, uint8_t decimals, float value) {

    bool negative = value < 0;

    uint32_t mul = 1;
    for (uint8_t i=0; i<decimals; i++) mul *= 10;

    value = abs(value);
    uint32_t value_int = int(value);
    uint32_t value_dec = int((value - value_int) * mul);

    char format[20];
    snprintf(format, sizeof(format), "%s%%lu.%%0%ulu", negative ? "-" : "", decimals);
    snprintf(buffer, len, format, value_int, value_dec);

    return buffer;

}



void setup() {
	pinMode(BUZZPIN, OUTPUT);
	buzz(BUZZPIN, 800, 500);
	commands.setDHT1(&dht1, DHT1PIN, DHTTYPE);
	commands.setDHT2(&dht2, DHT1PIN, DHTTYPE);
	response.setEchoServer(&TCPServer);

	while (ether.begin(sizeof Ethernet::buffer, mac, SS) == 0) {
		buzz(BUZZPIN, 200, 1000, 1);
		delay(1500);
	}

	while (!ether.dhcpSetup()) {
		buzz(BUZZPIN, 1200, 500, 2);
		delay(1000);
	}

	buzz(BUZZPIN, 5000, 300, 4);

}

void loop() {
	bool headers_sent = false;
	word len = ether.packetReceive();
	word pos = ether.packetLoop(len);

	if (pos) {  // check if valid tcp data is received
		buzz(BUZZPIN, 8000, 200, 1);
		bfill = ether.tcpOffset();
		/* print the deserialized object. */
//		if (!headers_sent) {
//			response.writeModule200DataHeaders();
//			headers_sent = true;
//		}

//		char *data = (char*) Ethernet::buffer + pos;
//		/* msgpack::sbuffer is a simple buffer implementation. */
//		msgpack_sbuffer sbuf;
//		msgpack_sbuffer_init(&sbuf);
//		msgpack_zone mempool;
//		msgpack_zone_init(&mempool, len);
//		msgpack_object deserialized;
//		msgpack_unpack((char*) Ethernet::buffer, len, NULL, &mempool, &deserialized);

//		deserialized.type

		/* buf is allocated by unpacker. */
//		msgpack_unpacker *unp = msgpack_unpacker_new(150);
//		msgpack_unpacked result;
//		msgpack_unpack_return ret;
//		size_t recv_len;
//		int recv_count = 0;
//		int i = 0;
//		msgpack_unpacked_init(&result);
//		while (true) {
//			recv_len = len;
//			if (recv_len == 0)
//				break; // (reached end of input)
//			ret = msgpack_unpacker_next(unp, &result);
//			while (ret == MSGPACK_UNPACK_SUCCESS) {
//				msgpack_object obj = result.data;
//
//				/* Use obj. */
//				bfill.print("Object no:");
//				bfill.println(++i);
////				msgpack_object_print(stdout, obj);
//				msgpack_object_print_buffer(responseBuffer, strlen(responseBuffer), obj);
//				/* If you want to allocate something on the zone, you can use zone. */
//				/* msgpack_zone* zone = result.zone; */
//				/* The lifetime of the obj and the zone,  */
//				bfill.print("Object data:");
//				bfill.println(responseBuffer);
//				responseBuffer[0] = '\0';
//
//				ret = msgpack_unpacker_next(unp, &result);
//			}
//			if (ret == MSGPACK_UNPACK_PARSE_ERROR) {
//				printf("The data in the buf is invalid format.\n");
//				msgpack_unpacked_destroy(&result);
//				return;
//			}
//		}
//		msgpack_unpacked_destroy(&result);
//		msgpack_unpacker_free(unp);
//		DeserializationError error = deserializeMsgPack(doc, Ethernet::buffer);
//		if (error) {
//			bfill.print(F("deserializeMsgPack() failed: "));
//			bfill.println(error.c_str());
//		    return;
//		  }
//		ArduinoJson6120_000001::VariantRef x;
//		x = doc.getElement(0);
//		ArduinoJson6120_000001::VariantRef y;
//		y = x.getElement(0);
//		bfill.println(y._data);
//		x = doc.getMember(MODULE_COMMMAND_FLAG);
//		x.g
//		bfill.println(len);

//		if(doc.containsKey(MODULE_COMMMAND_FLAG))
//			bfill.print(F("contains MODULE_COMMMAND_FLAG key: "));

//		for (uint8_t i = 0; i < len; i++) {
//			bfill.print(Ethernet::buffer[i]);
//		}

//		bfill.println();

		message->unpack(Ethernet::buffer, len);

		// Number of elements
		unsigned char count = message->count();
		bfill.print(F("Number of values: "));
		bfill.println(count);

		// Walk elements
		char buffer[32];
		for (unsigned char i = 0; i < count; i++) {
			unsigned char type = message->type(i);
			if (MESSAGE_PACK_BOOLEAN == type) {
				snprintf(buffer, sizeof(buffer), "Value #%d is a BOOLEAN: %d\n",
						i, message->getBoolean(i) ? 1 : 0);
			} else if (MESSAGE_PACK_SHORT == type) {
				snprintf(buffer, sizeof(buffer), "Value #%d is a SHORT: %u\n",
						i, message->getShort(i));
			} else if (MESSAGE_PACK_INTEGER == type) {
				snprintf(buffer, sizeof(buffer),
						"Value #%d is an INTEGER: %u\n", i,
						message->getInteger(i));
			} else if (MESSAGE_PACK_LONG == type) {
				snprintf(buffer, sizeof(buffer), "Value #%d is a LONG: %lu\n",
						i, message->getLong(i));
			} else if (MESSAGE_PACK_FLOAT == type) {
				char number[10];
				snfloat(number, sizeof(number), 2, message->getFloat(i));
				snprintf(buffer, sizeof(buffer), "Value #%d is a FLOAT: %s\n",
						i, number);
			} else if (MESSAGE_PACK_TEXT == type) {
				unsigned char size = message->getTextLength(i);
				char text[size + 1];
				snprintf(buffer, sizeof(buffer), "Value #%d is a TEXT: %s\n", i,
						message->getText(i, text));
			} else {
				snprintf(buffer, sizeof(buffer), "Value #%d is UNKNOWN\n", i);
			}
			bfill.println(type);
		}

//		boost::shared_array<char> data((char*) &Ethernet::buffer);
//		char_array_buffer buf(data.get(), len);     // no copy here!!!
//		std::istream is(&buf);                      // now we can read
//
//		bool res = true;
//		uint32_t map_size;
//		uint32_t r_size;
//		res &= msgpck_map_next((Stream *)&is);
//		if (!res) {
//			ether.httpServerReply(bfill.position());
//			continue;
//		}
//		res &= msgpck_read_map_size(&is, &map_size);
//		if (!res) {
//			ether.httpServerReply(bfill.position());
//			continue;
//		}
//		res &= (map_size == 2);
//		res &= msgpck_read_string(&is, buf, 3, &r_size);
//		if (!res) {
//			ether.httpServerReply(bfill.position());
//			continue;
//		}

//		for(buf.)

//		bfill.print(F("deserialize:type "));
//		bfill.println(deserialized.type);
//		bfill.print(F("deserialize:via.array.size"));
//		bfill.println(deserialized.via.array.size);
//		msgpack_zone_destroy(&mempool);
//		msgpack_sbuffer_destroy(&sbuf);

		ether.httpServerReply(bfill.position());
	}

	delay(2);
}

