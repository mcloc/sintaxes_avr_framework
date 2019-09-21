#ifndef __MODULE_COMMANDS_H_
#define __MODULE_COMMANDS_H_

#include <DHT.h>
#include <LocalBuffers.h>


class Commands;


class Commands {

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
    static uint32_t command_argument1;
    static uint32_t command_argument2;
    static uint32_t command_argument3;
    static uint32_t command_argument4;
    static uint32_t command_argument5;
    static uint32_t command_argument6;
    static uint32_t command_argument7;
    static uint32_t command_argument8;

    Commands(LocalBuffers *localBuffers);
    char *  getSensor1();
    char *  getSensor2();
    void initSensors();
    void setDHT1(DHT *dht1, uint8_t dht_pin, uint8_t type);
    void setDHT2(DHT *dht2, uint8_t dht_pin, uint8_t type);
    LocalBuffers *localBuffers;
    DHT *dht1;
    DHT *dht2;


private:

    DHT getDHT1();
    DHT getDHT2();
    
};


#endif
