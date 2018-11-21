#include <iostream>
#include <strings.h>
#include <cstring>
#include <wiringPi.h>
#include "WRCPTransmitter.hpp"

void WRCPTransmitter::run() {
	unsigned char data[BUFFER_SIZE_TRANSMITTER];
	while (true) {
		if (!this->outcoming_packets->hasMessage()) {
			delay(300);
			continue;
		}

		WRCP packet = this->outcoming_packets->pull();
		bzero(data, BUFFER_SIZE_TRANSMITTER);
		unsigned char *wrcp_data = packet.getData();
		memcpy(data + 5, wrcp_data, (size_t)WRCP_PACKET_SIZE);
		modem.writeData(data, BUFFER_SIZE_TRANSMITTER);
	}
}
