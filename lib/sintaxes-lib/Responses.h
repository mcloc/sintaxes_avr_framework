#pragma once

#ifndef __SINTAX_RESPONSES_H_
#define __SINTAX_RESPONSES_H_
#include <UIPEthernet.h>
#include <Arduino.h>
#include <LocalBuffers.h>

class Responses;

class Responses{
    public:
        Responses(LocalBuffers *_local_buffers);
        void setClient(EthernetClient *client);
        void writeModuleDataResponse();
        void writeModule200DataHeaders();
        void writeModule500DataHeaders();

        void sendFullStatusData(char *sensor1_data, char *sensor2_data);
        void initJsonResponse();
        void closeJsonResponse();

        void writeError_MAL_FORMED_REQUEST();
        void writeError_MAX_SIZE_REQUEST();
        void writeProcess32bitwordERROR();
        void writeReseting32bitwordERROR();
        void write4BCPWordNotMappedERROR();
        void write4BCPUnknowCommand();
        void writeErrorMsgPack4BCPExecuteFlagError();
        void writeErrorMsgPackHasNotFinishedStatus();
        void writeMsgPackError(unsigned long  _word);
        void writeMsgPackUnknowError();
        void writeMsgPackProcessingFlowError();
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



    
    private:
        LocalBuffers *localBuffers;
        EthernetClient *client;
        
    	bool response_json_initiated = false;
    	char response_json_finish_objects[];

        
};


#endif
