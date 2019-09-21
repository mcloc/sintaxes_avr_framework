#ifndef __SINTAXES_H_
#define __SINTAXES_H_

#include <Arduino.h>
typedef const __FlashStringHelper* FSH;

//#define BOOST_NO_EXCEPTIONS 1
//#define BOOST_EXCEPTION_DISABLE 1
//#define BOOST_SP_DISABLE_THREADS 1

//#include "module_strings_extern.h"
#include <EchoServer.h>
#include <Responses.h>
#include <Commands.h>
#include <DHT.h>
#include <EtherCard.h>


void buzz(uint8_t BUZPIN, int freq, int _delay, uint8_t times = 1);

#endif
