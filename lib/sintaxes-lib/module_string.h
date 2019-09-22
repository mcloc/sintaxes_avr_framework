#ifndef __MODULE_STRINGS_H_
#define __MODULE_STRINGS_H_

#include "avr/pgmspace.h"

/**
 * ATTENTION: each cons char variable must be included in the file module_strings_extern.h
 * which is located inside sintaxes-lib package
 * 
 */


    static const char header_response_200[] PROGMEM = "HTTP/1.1 200 OK";
    const char header_response_500[] PROGMEM = "HTTP/1.1 500 Internal Server Error";
    const char header_content_type_json[] PROGMEM = "Content-Type: application/json;charset=utf-8";
    const char header_server[] PROGMEM = "Server: AVR-Sintaxes-module-1";
    const char header_connection[] PROGMEM = "Connection: close";

    const char json_module_braces_open[] PROGMEM = "{";
    const char json_module_braces_close[] PROGMEM = "}";
    const char json_module_brackets_open[] PROGMEM = "[";
    const char json_module_brackets_close[] PROGMEM = "]";
    const char json_module_comma_separator[] PROGMEM = ",";
    const char json_module_new_line[] PROGMEM = "\n";
    const char json_module_double_quote[] PROGMEM = "\"";
    const char json_module_name[] PROGMEM = "\"module_name\":\"climatization_board#1\"";
    const char json_module_status[] PROGMEM = "\"status\":\"OK\"";
    const char json_module_uptime[] PROGMEM = "\"uptime\":132";
    const char json_module_error[] PROGMEM  = "{\"error_code\":%d,\"error_msg\":\"%S\"}";
    const char json_module_data_key[] PROGMEM  = "\"data\":";
    const char json_module_sensors_key[] PROGMEM  = "\"sensors\":";
    const char json_module_actuators_key[] PROGMEM  = "\"actuators\":";
    const char json_module_sensor1[] PROGMEM  = "{\"uuid\":\"DHT21#1\",\"value\":[{\"humidity\":%s},{\"temperature\":%s},{\"heat_index\":\"xxxxx\"}]}";
    const char json_module_sensor2[] PROGMEM  = "{\"uuid\":\"DHT21#2\",\"value\":[{\"humidity\":%s},{\"temperature\":%s},{\"heat_index\":\"xxxxx\"}]}";
    const char json_module_actuator1[] PROGMEM  = "{\"uuid\":\"solenoid#1\",\"value\":{\"active\":103,\"activated_time\":111}}";

    const char REQUEST_MAX_LENGHT_ERROR_STR[] PROGMEM = "\"max request lenght achieved.\"";
    const char MAL_FORMATED_MSGPCK_ERROR_STR[] PROGMEM = "\"mal formed msgpack or 4byteCommandProtocol\"";
    const char REQUEST_MISSING_STX_ERROR_STR[] PROGMEM = "\"mal formed request, must begin with STX command char and end with ETX command char\"";

#endif
