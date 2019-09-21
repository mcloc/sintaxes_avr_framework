#ifndef __MODULE_COMMANDS_H_
#define __MODULE_COMMANDS_H_

#include <DHT.h>


class Commands;


class Commands {

public: 
    
    /**
     * This is used by dtostrf() to convert float numbers into string
     * TODO: check if the string in PROGMEM accept %f so we dont need to use
     * we have 2 buffer for cases on which we do sprintf_P(str, buffer1, buffer2), so it's for when we have 2 sprintf_P() arguments
     * on the PROGMEM String variable;
     *
     * Remember this is for use bouth buffer in the same sprintf_P() statement, you will need to reuse it for others  write(FSH()
     * stament. Fof example, multiple sensors goes one by one so you only need 2 buffers at the same time. One for temperature, other
     * for humidity.
     *
     * FIXME: split sensors message segment into smal pieaces. As more  write(FSH() we do, as more little memory we'll need. One spit
     * at a time to client.
     *
     */
    static char float2char_buffer1[6];
    static char float2char_buffer2[6];

    /**
     * This is the buffer used on all sprintf_P for the PROGMEM String,
     * increase as the String is longer than this value or split into several other PROGMEM variables
     * this should keep low then 180 bytes
     */
    static char string_cpy_buffer[120];

    /**
     * MACINE ACTUAL STATE packed with messagePack
     */
    //static char module_actual_state[200]; MAde on the fly
    static char module_old_state[200]; // made before execution commands for rollback

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

    Commands();
    char *  getSensor1();
    char *  getSensor2();
    void initSensors();
    void setDHT1(DHT *dht1, uint8_t dht_pin, uint8_t type);
    void setDHT2(DHT *dht2, uint8_t dht_pin, uint8_t type);
    DHT *dht1;
    DHT *dht2;

private:

    DHT getDHT1();
    DHT getDHT2();
    
};


#endif