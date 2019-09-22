#ifndef __SINTAXES_H_
#define __SINTAXES_H_

#include <Arduino.h>
typedef const __FlashStringHelper* FSH;
#include <sintaxes-framework-defines.h>
#include <StandardCplusplus.h>
#include <boost_1_51_0.h>
#include <UIPEthernet.h>
#include <LocalBuffers.h>
#include <Responses.h>
#include <Commands.h>
#include <msgpack.hpp>
#include <MsgPackHandler.h>
#include <DHT.h>


void buzz(uint8_t BUZPIN, int freq, int _delay, uint8_t times = 1);

#endif
