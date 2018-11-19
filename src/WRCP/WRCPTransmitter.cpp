#include <iostream>
#include <strings.h>
#include <cstring>
#include <wiringPi.h>
#include "WRCPTransmitter.hpp"
#include "../ModemInterface.hpp"
#include "../ModemResolver.hpp"

void WRCPTransmitter::run() {
	ModemInterface *modem = ModemResolver::resolve();
	int number = 0;
	while (true) {
		unsigned char data[20];
		if (!this->outcoming_packets->hasMessage()) {
			delay(300);
			continue;
		}
		number++;

		WRCP packet = this->outcoming_packets->pull();
		bzero(data, 20);
		unsigned char *wrcp_data = packet.getData(number);
		memcpy(data + 5, wrcp_data, (size_t)WRCP_PACKET_SIZE);
		modem->writeData(wrcp_data, 20);

		if (number == 255)
			number = 0;
	}
}
