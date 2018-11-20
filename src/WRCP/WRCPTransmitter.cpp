#include <iostream>
#include <strings.h>
#include <cstring>
#include <wiringPi.h>
#include "WRCPTransmitter.hpp"

void WRCPTransmitter::run() {

	int number = 0;
	unsigned char data[20];
	while (true) {
		if (!this->outcoming_packets->hasMessage()) {
			delay(300);
			continue;
		}

		WRCP packet = this->outcoming_packets->pull();
		bzero(data, 20);
		unsigned char *wrcp_data = packet.getData();
		memcpy(data + 5, wrcp_data, (size_t)WRCP_PACKET_SIZE);
		modem.writeData(wrcp_data, 20);

		if (number == 255)
			number = 0;
	}
}
