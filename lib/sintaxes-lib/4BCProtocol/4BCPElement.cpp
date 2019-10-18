/*
 * 4BCPElement.cpp
 *
 *  Created on: 18 de out de 2019
 *      Author: mcloc
 */

#include <4BCProtocol/4BCPElement.h>
#include <Arduino.h>
#include <defines/sintaxes-framework-defines.h>

static _4BCPMapElement *_4BCPElement::element4BCP_1 = (_4BCPMapElement*) malloc(sizeof(_4BCPMapElement));
static _4BCPMapElement *_4BCPElement::element4BCP_2 = (_4BCPMapElement*) malloc(sizeof(_4BCPMapElement));
static _4BCPMapElement *_4BCPElement::element4BCP_3 = (_4BCPMapElement*) malloc(sizeof(_4BCPMapElement));
static _4BCPMapElement *_4BCPElement::element4BCP_4 = (_4BCPMapElement*) malloc(sizeof(_4BCPMapElement));
static _4BCPMapElement *_4BCPElement::element4BCP_5 = (_4BCPMapElement*) malloc(sizeof(_4BCPMapElement));
static _4BCPMapElement *_4BCPElement::element4BCP_6 = (_4BCPMapElement*) malloc(sizeof(_4BCPMapElement));
static _4BCPMapElement *_4BCPElement::element4BCP_7 = (_4BCPMapElement*) malloc(sizeof(_4BCPMapElement));
static _4BCPMapElement *_4BCPElement::element4BCP_8 = (_4BCPMapElement*) malloc(sizeof(_4BCPMapElement));
static _4BCPMapElement *_4BCPElement::element4BCP_9 = (_4BCPMapElement*) malloc(sizeof(_4BCPMapElement));
static _4BCPMap _4BCPElement::map4BCP;


static _4BCPElementValue *_4BCPElement::value_1 = (_4BCPElementValue*) malloc(sizeof(_4BCPElementValue));
static _4BCPElementValue *_4BCPElement::value_2 = (_4BCPElementValue*) malloc(sizeof(_4BCPElementValue));
static _4BCPElementValue *_4BCPElement::value_3 = (_4BCPElementValue*) malloc(sizeof(_4BCPElementValue));
static _4BCPElementValue *_4BCPElement::value_4 = (_4BCPElementValue*) malloc(sizeof(_4BCPElementValue));
static _4BCPElementValue *_4BCPElement::value_5 = (_4BCPElementValue*) malloc(sizeof(_4BCPElementValue));
static _4BCPElementValue *_4BCPElement::value_6 = (_4BCPElementValue*) malloc(sizeof(_4BCPElementValue));

_4BCPElement::_4BCPElement() {

}

