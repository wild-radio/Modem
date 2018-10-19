#include <iostream>
#include "WRCPReceiver.hpp"
#include "../ModemResolver.hpp"

void WRCPReceiver::run() {
	this->receiving = false;
	while (true) {
		ModemInterface *modem = ModemResolver::resolve();
		unsigned char byte;
		int len = modem->readData(&byte, 1);
		/*if (len == 0)
			continue;*/
		std::cout << byte << std::endl;
		if (byte == 'W') {
			this->receiving = true;
			this->clearBuffer();
		}

		if (this->receiving) {
			this->addToBuffer(byte);
		}
	}
}

void WRCPReceiver::clearBuffer() {
	this->buffer_pos = 0;
	delete this->buffer;
	this->buffer = new unsigned char[WRCP_PACKET_SIZE];
}

void WRCPReceiver::addToBuffer(unsigned char byte) {
	this->buffer[this->buffer_pos] = byte;
	this->buffer_pos++;

	if (this->buffer_pos != WRCP_PACKET_SIZE) {
		return;
	}

	WRCP packet = WRCP(this->buffer);
	if (this->isInOurList(packet)) {
		this->sendACK(packet);
		return;
	}
	this->incoming_queue->post(packet);
	this->addToOurList(packet);

	this->receiving = false;
	this->clearBuffer();
}

bool WRCPReceiver::isInOurList(WRCP packet) {
	for (auto &r_packet : this->received_packets) {
		if (r_packet == packet)
			return true;
	}

	return false;
}

void WRCPReceiver::sendACK(WRCP packet) {
	WRCP ack_packet;
	ack_packet.createACK(packet);

	this->outcoming_queue->post(ack_packet);
}

void WRCPReceiver::addToOurList(WRCP packet) {
	this->received_packets.push_back(packet);
	
	if (this->received_packets.size() >= MAX_RECEIVED_PACKETS_BUFFER)
		this->received_packets.pop_back();
}