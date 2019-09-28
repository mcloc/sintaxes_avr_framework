#include <sintaxes-lib.h>

SintaxesLib::SintaxesLib(){

}




void SintaxesLib::buzz(int freq, int _delay, uint8_t times = 1){
	for(uint8_t i=0;i<times;i++){
		tone(_BUZZPIN, freq); // Send 1KHz sound signal...
		setLed(LED_BUILTIN, HIGH);
		delay(_delay);        // ...for 1 sec
		noTone(_BUZZPIN);     // Stop sound...
		setLed(LED_BUILTIN, LOW);
		delay(_delay);

	}
}

void SintaxesLib::setLed(uint8_t pin_led, uint8_t level){
	digitalWrite(pin_led, level);
}

void SintaxesLib::blink(uint8_t pin_led, uint8_t _delay, uint8_t times = 1){
	for(uint8_t i=0;i<times;i++){
		setLed(pin_led, HIGH);
		delay(_delay);        // ...for 1 sec
		setLed(pin_led, LOW);
		delay(_delay);
	}
}
