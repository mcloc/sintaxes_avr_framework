#pragma once

#ifndef __MODULE_COMMANDS_H_
#define __MODULE_COMMANDS_H_

#include <DHT.h>
#include <LocalBuffers.h>
#include <Responses.h>
#include <MachineState.h>

//class Commands;


class CommandsHandler {

public: 
    
    /**
     * MACINE ACTUAL STATE packed with messagePack
     */
    //static char module_actual_state[200]; MAde on the fly
//    static char module_old_state[200]; // made before execution commands for rollback

    /**
     * command to execute / in execution
     */
     static uint32_t command_executing;

    /**
     * uint32_t 8 bytes arguments for commands to be called
     */
//     uint32_t command_argument1;
//     uint32_t command_argument2;
//     uint32_t command_argument3;
//     uint32_t command_argument4;
//     uint32_t command_argument5;
//     uint32_t command_argument6;
//     uint32_t command_argument7;
//     uint32_t command_argument8;

    CommandsHandler();
    CommandsHandler(LocalBuffers *localBuffers, Responses *_response);

    void setMachineState(MachineState ** _machine_state);
    bool get_data();
    void initSensors();
    bool set_actuator(bool state, uint32_t duration);


    DHT *dht1;
    DHT *dht2;
    void setDHT1(DHT *dht1, uint8_t dht_pin, uint8_t type);
    void setDHT2(DHT *dht2, uint8_t dht_pin, uint8_t type);
//    void setActuator(uint32_t actuator_id, bool state, uint16_t duration);
//    void setActuator(SintaxesActuator[]);


    char *  getSensor1();
    char *  getSensor2();
    bool execute();


private:
    LocalBuffers *localBuffers;
    Responses *response;
    DHT getDHT1();
    DHT getDHT2();
    MachineState **machineState;
    

};


#endif
