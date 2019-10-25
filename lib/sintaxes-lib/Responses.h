#pragma once

#ifndef __SINTAX_RESPONSES_H_
#define __SINTAX_RESPONSES_H_
#include <UIPEthernet.h>
#include <Arduino.h>
#include <LocalBuffers.h>

class Responses;

class Responses{
    public:
        Responses();
        void setClient(EthernetClient *client);
        void writeModuleDataResponse();
        void writeModule200DataHeaders();
        void writeModule500DataHeaders();

        void sendFullStatusData(char *sensor1_data, char *sensor2_data);
        void initJsonResponse();
        void closeJsonResponse();

        void writeError_MAL_FORMED_REQUEST();
        void writeError_MAX_SIZE_REQUEST();
        void writeError_on_INIT();
        void writeProcess32bitwordERROR();
        void writeReseting32bitwordERROR();
        void write4BCPWordNotMappedERROR();
        void write4BCPCommandExecutionERROR();
        void write4BCPCommandArgsMissing();
        void write4BCPUnknowCommand();
        void writeErrorMsgPack4BCPElementHasNoKey(uint8_t byte);
        void writeErrorMsgPack4BCPElementKeyProcessing();
        void writeErrorMsgPack4BCPExecuteFlagError();
        void writeErrorMsgPack4BCPHasNoCommandFlag();
        void write4BCPMalFormedRequest(uint8_t byte,uint8_t status);
        void write4BCPNestedElementsOutOfBound();

        void write4BCPUnknowError(uint8_t prev, uint8_t status, uint8_t next);
        void writeErrorMsgPackHasNotFinishedStatus();
        void writeErrorMsgPack4BCPZeroElementMap();
        void writeMsgPackError(unsigned long  _word);
        void writeMsgPackUnknowError();
        void writeMsgPackUnimplemented(uint8_t _byte);
        void writeMsgPack4BCPMethodUnimplemented(uint32_t _byte);
        void writeMsgPackUnknownType(uint8_t type);
        void writeMsgPackProcessingFlowError();
        void writeMsgPackProcessingFlowError(uint8_t status, uint8_t next, uint8_t prev);
        void writeMsgPackProcessingFlowStatus(uint8_t status, uint8_t next, uint8_t prev);
        void writeMsgPackProcessingMap(uint8_t status, uint8_t next, uint8_t prev);
        void writeErrorProcessingStream();
        void writeErrorMsgPackHasFinishedWithBytes();

        void writeSTXError();


        void writeByte(uint8_t byte);
        void write32bitByte(unsigned long word);
        void writeRaw(char *byte);
        void writeRaw(const __FlashStringHelper *);

        void writeDEBUG_INT(unsigned long word);
        void writeDEBUG_CHAR(__FlashStringHelper *byte);
//        void writeDEBUG_CHAR(unsigned long byte);
        int	error_MAX_SIZE_REQUEST_SIZE();
        void setReponseJsonInitiated();


    
    private:
        EthernetClient *client;
        
    	bool response_json_initiated = false;
    	char response_json_finish_objects[];

        
};


#endif
