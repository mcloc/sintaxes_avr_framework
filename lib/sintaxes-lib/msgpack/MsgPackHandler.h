#ifndef __SINTAXES_MSGPACK_HANDLER_H_
#define __SINTAXES_MSGPACK_HANDLER_H_

#include <4BCProtocol/4BCPContainer.h>
#include <sintaxes-lib.h>
#include <Stream.h>
#include <Arduino.h>
#include <commands/CommandsHandler.h>
#include <defines/msgpack_defines.h>
#include <defines/sintaxes-framework-defines.h>
#include <MachineState.h>

#define MSGPACK4BCPProcessFlow_SIZE 	9
#define MSGPACK4BCPProcessFlow2_SIZE 	7

class MsgPackHandler;

class MsgPackHandler {
public:
	MsgPackHandler();
//	MsgPackHandler(Responses *_response, CommandsHandler *commands, SintaxesLib *sintax_lib);
	bool init(Stream * _stream, int size);
	bool processStream();
	void reset();

private:
	Stream *stream;
//	Responses *response;
//	CommandsHandler *commands_handler;
//	SintaxesLib *sintaxesLib;
//	MachineState *machine_state;

	uint8_t status;
	uint8_t prev_status;
	uint8_t next_status;
//	static uint8_t process_state_stack[]; //Pointer to array in the SD Card, LOG PROCESS, and LOG in Commands execution footprint

	unsigned int error_code;

	//TCP request buffer
	int buffer_lenght;
	int buffer_position;
	int buffer_bytes_remaining;
	char buffer_processsing_byte;

	uint8_t last_byte; // may be we'll use to know if we are inseide array, but i think other controll should be done for that

	//32bit word of the 4Bytes Command Protocol
	unsigned long _32bitword; // this buffer is utilized directly from inside methods, beaware to don't overwrite it
	uint8_t _32bitword_remaining = 4; // 4 8 bit bytes to achieve 32bits unsignedLong
	unsigned long _32bitword_array[4]; //4th index is the NULL terminator


	//msgpack values data type > 8bit
//	uint16_t _16bitword;



	unsigned int response_headers_code;
	bool response_headers_already_sent = false;


	//Acording to definition MAX_MSGPACK_NESTED_ELEMENTS
	uint8_t element4BCP_number = 0;
	uint8_t nested_element4BCP = 0;
	uint8_t total_element4BCP = 0;
	uint8_t total_elementValue4BCP = 0;
	uint8_t elements_remaining = MAX_MSGPACK_4BCP_ELEMENTS;



	//FIXME: PROGMEM is ignored due FIXED: PROGMEM is on cpp file
	 static const uint8_t MSGPACK4BCPProcessFlow[MSGPACK4BCPProcessFlow_SIZE];
	 static const uint8_t MSGPACK4BCPProcessFlow2[MSGPACK4BCPProcessFlow2_SIZE];
//	 static const uint8_t MSGPACK4BCPProcessFlow3[];
//	 static const uint8_t MSGPACKTYPES[MSGPACK4BCPProcessFlow2_SIZE];



	uint8_t whatNext();
	uint8_t next();
	bool processArray(uint8_t _byte, uint8_t array_size);
	bool assembleMap(uint8_t _byte, uint8_t map_elements_size);
	bool processMap();
	bool processCommandHeader(uint8_t _word);
	bool checkModulesMap();
	uint8_t getNextType();
	bool setElementValue(_4BCPMapElement **element);


	bool check4BCPProcesFlow(const uint8_t *flow_array_ptr, uint8_t array_size);
	bool checkFlow();

	bool assemble_uint32_Byte(uint8_t _byte);
	bool assemble_uint16_Byte(uint8_t _byte);
	bool reset_32bit_processing();
	uint8_t actualBufferByte();




	bool setStatus(uint8_t _status);
	unsigned int isArray(uint8_t _byte);
	unsigned int isMap(uint8_t _byte);
	unsigned long isMapped();

	_4BCPMapElement **processStatusNextElement();
	bool processStatusMSGPACK_STATE_BEGIN();


	_4BCPMapElement **setElementPointer(bool add_vlue = false);
	_4BCPElementValue **setStructElementValue();

	/**
	 * we wont use pointer to another pointer (at least minimal when talking to other classes due to scope)
	 * just for standards [&](){ cout << F(x)} ...
	 */
	//unsigned long get32bitByte();
	//Tiny and Tight that's the deal. This code will be refactored dozen times until it's tinier and tighter as possible. We wont have much problem with coupling code case this is a firmware
	// We just need the procedures divided by true scope and human maintenance agile. This framework template is intended to to be s initial base for creation of unique and well defined end.
	//It just gives a certain way to achieve your goals based on best practice and agile human interference...
	//This is a creation of a new Design Pattern. Agile, relative tiny and very tight, rough, but with agile human interface
	//This is a long road (20191022 16:14 -3 UTC), I (for now) will mark all end points of the goal achievement.
	//Our main platform will be 8 bits Atmel's AVR. Eclipse a tool spread on the market with platformIO as project administration, dependency control and etc
	//Besides msgpack-rpc, I don't think RPC should be as Designed to be on embedded applications. There's a lack in RPC with real little RAM memory (2KB, 8KB) for processing strings.
	//MsgPack was taken for base communication and data transmitter.
	//But we are developing an above layer on msgpack, the 4BytesCommandProtocol(4BCP) which is a uint32_t hex map of commands and devices uuid, so we will only use 4bytes to transmit
	//up to 4.294.967.295 packets of 4byte information, symbols. See more on 4BCP Documentation.
	//And yeah we will use :
	//	- static class members;
	//	- and startup initialization for important data.
	//
	// This new pattern for embedded application will paradox with some other patterns. In the grace of robust, efficient and agile development for embedded.

};




#endif
