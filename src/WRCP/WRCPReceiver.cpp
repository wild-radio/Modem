#include <iostream>
#include "WRCPReceiver.hpp"

void WRCPReceiver::run() {
	this->receiving = false;
	bool received_w = false;
	this->buffer = new unsigned char[WRCP_PACKET_SIZE];
	unsigned char byte;
	this->clearReceivedList();

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
	this->incoming_queue->post(packet);
	this->moveList();
	this->addToOurList(packet);

	this->receiving = false;
	this->clearBuffer();
}

bool WRCPReceiver::isInOurList(WRCP packet) {
	auto pair = getPair(packet);
	for (auto &l_pair : this->list) {
		if (*l_pair == *pair) {
			delete pair;
			return true;
		}
	}

	delete pair;
	return false;
}

void WRCPReceiver::sendACK(WRCP packet) {
	WRCP ack_packet;
	ack_packet.createACK(packet);

	this->outcoming_queue->post(ack_packet);
}

void WRCPReceiver::addToOurList(WRCP packet) {
	auto pair = getPair(packet);
	for (int i = 0; i < MAX_RECEIVED_PACKETS_BUFFER; i++) {
		if (list[i] == nullptr) {
			list[i] = pair;
			return;
		}
	}
}

std::pair<int, int> * WRCPReceiver::getPair(WRCP &packet) const {
	int sender_id = packet.getSender();
	int message_number = packet.getMessageNumber();
	return new std::pair<int, int>(sender_id, message_number);
}

void WRCPReceiver::clearReceivedList() {
	for (int i = 0; i < MAX_RECEIVED_PACKETS_BUFFER; i++)
		list[i] = nullptr;
}

void WRCPReceiver::moveList() {
	auto array_limit = MAX_RECEIVED_PACKETS_BUFFER - 1;
	if (list[array_limit] != nullptr)
		delete list[array_limit];
	for(int i = array_limit; i > 1; i++) {
		list[i - 1] = list[i];
	}
}
