#ifndef __SINTAX_RESPONSES_H_
#define __SINTAX_RESPONSES_H_
#include <UIPEthernet.h>
#include <Arduino.h>

class Responses;

class Responses{
    public:
        Responses(LocalBuffers *_local_buffers);
        void setClient(EthernetClient *client);
        void writeModuleDataResponse();
        void writeModule200DataHeaders();
        void writeModule500DataHeaders();
        void writeError_MAX_SIZE_REQUEST();
        void writeProcess32bitwordERROR();
        void writeMsgPackError(uint8_t _byte);
        void writeSTXError();
        void writeByte(uint8_t byte);
        void write32bitByte(unsigned long byte);
        void writeRaw(char *byte);

        void writeDEBUG_INT(unsigned long byte);
        void writeDEBUG_CHAR(__FlashStringHelper *byte);
//        void writeDEBUG_CHAR(unsigned long byte);
        int	error_MAX_SIZE_REQUEST_SIZE();



    
    private:
        LocalBuffers *localBuffers;
        EthernetClient *client;
        
        
};


#endif
