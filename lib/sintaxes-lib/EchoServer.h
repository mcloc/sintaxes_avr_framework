#ifndef __ECHO_SERVER_H_
#define __ECHO_SERVER_H_

class Commands;
class Responses;
class EchoServer;

class EchoServer {
    public:
        EchoServer(Commands *_commands,Responses *_response);
        void setCommand(Commands *commands);
        void setResponse(Responses *response);
        bool getDHCPSetup();
    private:
        Commands *commands;
        Responses *response;
        
        

};

#endif
