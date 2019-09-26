#include <sintaxes-lib.h>

SintaxesLib::SintaxesLib(){

}




void SintaxesLib::buzz(int freq, int _delay, uint8_t times = 1){
	for(uint8_t i=0;i<times;i++){
		tone(BUZZPIN, freq); // Send 1KHz sound signal...
		delay(_delay);        // ...for 1 sec
		noTone(BUZZPIN);     // Stop sound...
		delay(_delay);
	}
}
