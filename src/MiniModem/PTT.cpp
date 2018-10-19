#include "PTT.hpp"

#ifdef __arm__

PTT::PTT() {
	wiringPiSetup();
	pinMode (GPIO_PIN, OUTPUT) ;
}

void PTT::push() {
	delay(1000);
	digitalWrite (GPIO_PIN, HIGH);
	delay(500);
}

void PTT::release() {
	delay(100);
	digitalWrite (GPIO_PIN, LOW);
}

#else

PTT::PTT() {}
void PTT::push() {}
void PTT::release() {}

#endif

