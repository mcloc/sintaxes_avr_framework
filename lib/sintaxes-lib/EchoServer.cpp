#include "EchoServer.h"
#include "Responses.h"
#include <SPI.h>






EchoServer::EchoServer(Commands *_commands,Responses *_response){
     commands = _commands;
     response = _response;
     (*response).setCommands(commands);
//     (*response).setEchoServer(this);
}


void EchoServer::setCommand(Commands *_commands){
    commands = _commands;
}

void EchoServer::setResponse(Responses *_response){
    response = _response;
}



