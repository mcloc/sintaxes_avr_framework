#ifndef __SINTAX_RESPONSES_H_
#define __SINTAX_RESPONSES_H_
#include "Commands.h"
#include <UIPEthernet.h>
//#include "EchoServer.h"

//#include "module_string.h"

class Responses;

class Responses{
    public:
        Responses(Commands *_commands, LocalBuffers *_local_buffers);
        Responses(Commands *_commands);
        void setCommands(Commands *_commands);
        void setClient(EthernetClient *client);
        void writeModuleDataResponse();
        void writeModule200DataHeaders();
        void writeModule500DataHeaders();
        void writeError_MAX_SIZE_REQUEST();
        void writeProcess32bitwordERROR();
        void writeSTXError();
        void writeByte(uint8_t byte);
        void write32bitByte(unsigned long byte);
        int	error_MAX_SIZE_REQUEST_SIZE();



    
    private:
        LocalBuffers *localBuffers;
        Commands *commands;
        EthernetClient *client;
        
        
};


#endif
