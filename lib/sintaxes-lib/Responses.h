#ifndef __RESPONSES_H_
#define __RESPONSES_H_
#include "Commands.h"
#include "EchoServer.h"
#include <EtherCard.h>

//#include "module_string.h"

class Responses;

class Responses{
    public:
        Responses(Commands *_commands, BufferFiller *_bfill);
        void setEchoServer(EchoServer *_echoServer);
        void setCommands(Commands *_commands);
        void writeModuleDataResponse();
        void writeModule200DataHeaders();
    
    private:
        EchoServer *echoServer;
        Commands *commands;
        BufferFiller *bfill;
        
        
};


#endif
