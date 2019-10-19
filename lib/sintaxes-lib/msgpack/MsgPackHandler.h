#ifndef __SINTAXES_MSGPACK_HANDLER_H_
#define __SINTAXES_MSGPACK_HANDLER_H_

#include <sintaxes-lib.h>
#include <Stream.h>
#include <Commands.h>
#include <Arduino.h>
#include <defines/msgpack_defines.h>
#include <defines/sintaxes-framework-defines.h>
#include <MachineState.h>
#include <4BCProtocol/4BCPElement.h>

#define MSGPACK4BCPProcessFlow_SIZE 	9
#define MSGPACK4BCPProcessFlow2_SIZE 	7

class MsgPackHandler;

class MsgPackHandler {
public:
	MsgPackHandler();
	MsgPackHandler(Responses *_response, Commands *commands, SintaxesLib *sintax_lib);
	bool init(Stream * _stream, int size, MachineState * machine_state);
	bool processStream();

private:
	Stream *stream;
	Responses *response;
	Commands *commands;
	SintaxesLib *sintaxesLib;
	MachineState *machine_state;

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
//
//	static _4BCPMapElement *element4BCP_1;
//	static _4BCPMapElement *element4BCP_2;
//	static _4BCPMapElement *element4BCP_3;
//	static _4BCPMapElement *element4BCP_4;
//	static _4BCPMapElement *element4BCP_5;
//	static _4BCPMapElement *element4BCP_6;
//	static _4BCPMapElement *element4BCP_7;
//	static _4BCPMapElement *element4BCP_8;
//	static _4BCPMapElement *element4BCP_9;
//
//	static _4BCPElementValue *value_1;
//	static _4BCPElementValue *value_2;
//	static _4BCPElementValue *value_3;
//	static _4BCPElementValue *value_4;
//	static _4BCPElementValue *value_5;
//	static _4BCPElementValue *value_6;




	//4Bytes Command Protocol buffers
//	unsigned long ext_command;
//	unsigned long ext_command_args1;
//	unsigned long ext_command_args2;
//	unsigned long ext_command_args3;
//	unsigned long ext_command_args4;



	//FIXME: PROGMEM is ignored due FIXED: PROGMEM is on cpp file
	 static const uint8_t MSGPACK4BCPProcessFlow[MSGPACK4BCPProcessFlow_SIZE];
	 static const uint8_t MSGPACK4BCPProcessFlow2[MSGPACK4BCPProcessFlow2_SIZE];
//	 static const uint8_t MSGPACK4BCPProcessFlow3[];
//	 static const uint8_t MSGPACKTYPES[MSGPACK4BCPProcessFlow2_SIZE];

//
//	uint8_t length = sizeof(some_array) / sizeof(some_array[0]);
//
//	for (uint8_t i = 0; i < length; i++) {
//		Process(pgm_read_byte(&some_array[i]));
//	}


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
	uint8_t MsgPackHandler::actualBufferByte();




	bool setStatus(uint8_t _status);
	unsigned int isArray(uint8_t _byte);
	unsigned int isMap(uint8_t _byte);
	unsigned long isMapped();

	_4BCPMapElement **processStatusMSGPACK_STATE_COMMAND_SET();
	bool processStatusMSGPACK_STATE_BEGIN();


	_4BCPMapElement **setElementPointer(bool add_vlue = false);
	_4BCPElementValue **setStructElementValue();

	/**
	 * we wont use pointer to another pointer (at least minimal when talking to other classes due to scope)
	 * just for standards [&](){ cout << F(x)} ...
	 */
	//unsigned long get32bitByte();


};


//static _4BCPMapElement *element4BCP_1 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//	element4BCP_2 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//	element4BCP_3 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//	element4BCP_4 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//	element4BCP_5 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//	element4BCP_6 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//	element4BCP_7 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//	element4BCP_8 = (_4BCPMapElement *)malloc(sizeof(_4BCPMapElement));
//
//
//	value_1 = (_4BCPElementValue *)malloc(sizeof(_4BCPElementValue));
//	value_2 = (_4BCPElementValue *)malloc(sizeof(_4BCPElementValue));
//	value_3 = (_4BCPElementValue *)malloc(sizeof(_4BCPElementValue));
//	value_4 = (_4BCPElementValue *)malloc(sizeof(_4BCPElementValue));
//	value_5 = (_4BCPElementValue *)malloc(sizeof(_4BCPElementValue));
//	value_6 = (_4BCPElementValue *)malloc(sizeof(_4BCPElementValue));




#endif
