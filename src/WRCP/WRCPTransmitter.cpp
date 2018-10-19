#include <iostream>
#include "WRCPTransmitter.hpp"
#include "../ModemInterface.hpp"
#include "../ModemResolver.hpp"

void WRCPTransmitter::run() {
	ModemInterface *modem = ModemResolver::resolve();
	while (true) {
		if (!this->outcoming_packets->hasMessage())
			return;

		//TODO: Maybe add a delay time here.

		WRCP packet = this->outcoming_packets->pull();
		modem->writeData(packet.getData(), WRCP_PACKET_SIZE);
	}
}
