#ifndef __SINTAX_RESPONSES_H_
#define __SINTAX_RESPONSES_H_
#include "Commands.h"
#include <UIPEthernet.h>
//#include "EchoServer.h"

//#include "module_string.h"

class Responses;

class Responses{
    public:
        Responses(Commands *_commands);
        void setCommands(Commands *_commands);
        void setClient(EthernetClient *client);
        void writeModuleDataResponse();
        void writeModule200DataHeaders();
        void error_MAX_SIZE_REQUEST();
        int	error_MAX_SIZE_REQUEST_SIZE();


    
    private:
        Commands *commands;
        EthernetClient *client;
        
        
};


#endif
