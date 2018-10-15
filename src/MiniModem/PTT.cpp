#include "PTT.hpp"

#ifdef __arm__

PTT::PTT() {
	wiringPiSetup();
	pinMode (GPIO_PIN, OUTPUT) ;
}

void PTT::push() {
	digitalWrite (GPIO_PIN, HIGH);
	delay(150);
}

void PTT::release() {
	digitalWrite (GPIO_PIN, LOW);
	delay(200);
}

#else

PTT::PTT() {}
void PTT::push() {}
void PTT::release() {}

#endif

