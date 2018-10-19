#include <iostream>
#include <strings.h>
#include <cstring>
#include "WRCPTransmitter.hpp"
#include "../ModemInterface.hpp"
#include "../ModemResolver.hpp"

void WRCPTransmitter::run() {
	ModemInterface *modem = ModemResolver::resolve();
	while (true) {
		if (!this->outcoming_packets->hasMessage())
			continue;

		//TODO: Maybe add a delay time here.

		WRCP packet = this->outcoming_packets->pull();
		unsigned char data[30];
		bzero(data, 30);
		unsigned char *wrcp_data = packet.getData();
		memcpy(data + 13, wrcp_data, (size_t)WRCP_PACKET_SIZE);
		modem->writeData(data, 30);
		std::cout << "Packet sent" << std::endl;
	}
}
