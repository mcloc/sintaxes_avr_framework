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
        void writeError_MAX_SIZE_REQUEST();
        void writeProcess32bitwordERROR();
        void writeMsgPackError(unsigned long  _word);
        void writeMsgPackUnknowError();
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
        
        
};


#endif
