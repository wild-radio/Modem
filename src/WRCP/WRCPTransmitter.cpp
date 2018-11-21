#include <iostream>
#include <strings.h>
#include <cstring>
#include <wiringPi.h>
#include "WRCPTransmitter.hpp"

void WRCPTransmitter::run() {
	unsigned char data[30];
	while (true) {
		if (!this->outcoming_packets->hasMessage()) {
			delay(300);
			continue;
		}

		WRCP packet = this->outcoming_packets->pull();
		//bzero(data, 30);
		unsigned char *wrcp_data = packet.getData();
		//memcpy(data + 10, wrcp_data, (size_t)WRCP_PACKET_SIZE);
		modem.writeData(wrcp_data, WRCP_PACKET_SIZE);
	}
}
