#include <Arduino.h>
#include <Commands.h>
#include <Responses.h>
#include <LocalBuffers.h>
#include <Responses.h>
#include <StandardCplusplus.h>
#include <module_string.h>
//#include <sintaxes-lib.h>
/**
 * command to execute / in execution
 */
 uint32_t Commands::command_executing;

/**
 * uint32_t 8 bytes arguments for commands to be called
 */
 uint32_t Commands::command_argument1;
 uint32_t Commands::command_argument2;
 uint32_t Commands::command_argument3;
 uint32_t Commands::command_argument4;
 uint32_t Commands::command_argument5;
 uint32_t Commands::command_argument6;
 uint32_t Commands::command_argument7;
 uint32_t Commands::command_argument8;

Commands::Commands(LocalBuffers *_localBuffers, Responses *_response){
	localBuffers = _localBuffers;
	response = _response;
}

bool Commands::get_data(){
	char *sensor1_data, *sensor2_data;
	// DTH21#1 ouput
	sensor1_data = getSensor1();
	// DTH21#2 ouput
	sensor2_data = getSensor2();

	response->sendFullStatusData(sensor1_data, sensor2_data);

	return true;

}

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

bool Commands::execute(){
	//TODO 	GET THE ASSEMBLED COMMAND ON COMMANDS.buffers AND CAL THE COMMAND FUNCTION
	//		on the function execution send json with assignature of 4bytes command executing and
	//		all it's args
}
