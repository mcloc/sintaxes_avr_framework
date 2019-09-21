#ifndef __SINTAXES_H_
#define __SINTAXES_H_

#include <Arduino.h>
typedef const __FlashStringHelper* FSH;
#include <UIPEthernet.h>
//#include "utility/logging.h"
//#include "module_strings_extern.h"
//#include <EchoServer.h>
#include <LocalBuffers.h>
#include <Responses.h>
#include <Commands.h>
#include <DHT.h>


void buzz(uint8_t BUZPIN, int freq, int _delay, uint8_t times = 1);

#endif
