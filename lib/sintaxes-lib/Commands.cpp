#include <sintaxes-lib.h>

/**
 * command to execute / in execution
 */
static uint32_t Commands::command_executing;

/**
 * uint32_t 8 bytes arguments for commands to be called
 */
static uint32_t Commands::command_argument1;
static uint32_t Commands::command_argument2;
static uint32_t Commands::command_argument3;
static uint32_t Commands::command_argument4;
static uint32_t Commands::command_argument5;
static uint32_t Commands::command_argument6;
static uint32_t Commands::command_argument7;
static uint32_t Commands::command_argument8;

Commands::Commands(){};

void Commands::setDHT1(DHT *_dht1, uint8_t dht_pin, uint8_t type){
    dht1 = _dht1;
    //RESET THE DHT#1 SENSOR
	digitalWrite(dht_pin, LOW); // sets output to gnd
	pinMode(dht_pin, OUTPUT); // switches power to DHT on
	delay(1200); // delay necessary after power up for DHT to stabilize
	(*dht1).begin();
}
void Commands::setDHT2(DHT *_dht2,uint8_t dht_pin, uint8_t type){
    dht2 = _dht2;
    //RESET THE DHT#2 SENSOR
	digitalWrite(dht_pin, LOW); // sets output to gnd
	pinMode(dht_pin, OUTPUT); // switches power to DHT on
	delay(1200); // delay necessary after power up for DHT to stabilize
    (*dht2).begin();
}

char *  Commands::getSensor1(){
    float readed_value = (*dht1).readHumidity();
    dtostrf(readed_value, 5, 2, localBuffers->float2char_buffer1);
    readed_value = (*dht1).readTemperature();
    dtostrf(readed_value, 5, 2, localBuffers->float2char_buffer2);
    snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_sensor1), localBuffers->float2char_buffer1, localBuffers->float2char_buffer2);
    return LocalBuffers::string_cpy_buffer;
}

char *  Commands::getSensor2(){
    float readed_value = (*dht2).readHumidity();
    dtostrf(readed_value, 5, 2, localBuffers->float2char_buffer1);
    readed_value = (*dht2).readTemperature();
    dtostrf(readed_value, 5, 2, localBuffers->float2char_buffer2);
    snprintf_P(LocalBuffers::string_cpy_buffer, sizeof(LocalBuffers::string_cpy_buffer), (PGM_P)&(json_module_sensor2), localBuffers->float2char_buffer1, localBuffers->float2char_buffer2);
    return LocalBuffers::string_cpy_buffer;
}
