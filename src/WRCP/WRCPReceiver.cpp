#include <iostream>
#include "WRCPReceiver.hpp"

void WRCPReceiver::run() {
	this->receiving = false;
	bool received_w = false;
	this->buffer = new unsigned char[WRCP_PACKET_SIZE];
	unsigned char byte;

	while (true) {
		int len = modem.readData(&byte, 1);
		if (len == 0)
			continue;
		if (byte == 'W') {
			received_w = true;
		}

		if (byte == 'R' && received_w) {
			this->receiving = true;
			received_w = false;
			clearBuffer();
			this->addToBuffer('W');
			this->addToBuffer('R');
			continue;
		}

		if (this->receiving) {
			this->addToBuffer(byte);
		}
	}
}

void WRCPReceiver::clearBuffer() {
	this->buffer_pos = 0;
	/*if ()
	delete this->buffer;
	this->buffer = new unsigned char[WRCP_PACKET_SIZE];*/
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
	this->addToOurList(packet);
	this->incoming_queue->post(packet);

	this->receiving = false;
	this->clearBuffer();
}

bool WRCPReceiver::isInOurList(WRCP packet) {
	auto pair = std::pair<const int, const int >((const int)packet.getSender(), (const int)packet.getMessageNumber());
	for (auto &cursor_pair: list_message_numbers) {
		if (cursor_pair == pair)
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
	auto pair = std::pair<const int, const int >((const int)packet.getSender(), (const int)packet.getMessageNumber());
	list_message_numbers.push_back(pair);
	//if (list_message_numbers.size() == MAX_RECEIVED_PACKETS_BUFFER)
}
