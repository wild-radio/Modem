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
	this->incoming_queue->post(packet);
	this->addToOurList(packet);

	this->receiving = false;
	this->clearBuffer();
}

bool WRCPReceiver::isInOurList(WRCP packet) {
	auto b_packet =  packet.getPacket();
	for (auto &a_packet : this->received_packets) {
		if (isEqual(a_packet, b_packet))
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
	this->received_packets.push_back(packet.getPacket());
	
	if (this->received_packets.size() >= MAX_RECEIVED_PACKETS_BUFFER)
		this->received_packets.pop_back();
}

bool WRCPReceiver::isEqual(_wrcp_packet &packet_a, _wrcp_packet &packet_b) {
	bool is_same_sender = packet_a.sender_id == packet_b.sender_id;
	bool is_same_number = packet_a.message_number == packet_b.sender_id;
	return is_same_number && is_same_sender;
}
