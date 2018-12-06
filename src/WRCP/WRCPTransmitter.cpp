#include <iostream>
#include <strings.h>
#include <cstring>
#include <wiringPi.h>
#include "WRCPTransmitter.hpp"
#include "../MiniModem/MinimodemResolver.hpp"

void WRCPTransmitter::run() {
	unsigned char data[BUFFER_SIZE_TRANSMITTER];
	MiniModem *modem = MinimodemResolver::resolve();
	while (true) {
		if (!this->outcoming_packets->hasMessage()) {
			delay(300);
			continue;
		}

		WRCP packet = this->outcoming_packets->pull();
		bzero(data, BUFFER_SIZE_TRANSMITTER);
		unsigned char *wrcp_data = packet.getData();
		memcpy(data + 10, wrcp_data, sizeof(_wrcp_packet));
		modem->writeData(data, BUFFER_SIZE_TRANSMITTER);
	}
}
