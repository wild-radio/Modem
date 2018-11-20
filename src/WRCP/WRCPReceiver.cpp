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
	auto isNotAckNorNanck = !packet.isACK() && !packet.isNACK();
	int message_number = packet.getMessageNumber();
	if (isNotAckNorNanck && this->isInOurList(message_number)) {
		this->sendACK(packet);
		return;
	}
	if (isNotAckNorNanck)
		this->addToOurList(message_number);
	this->incoming_queue->post(packet);

	this->receiving = false;
	this->clearBuffer();
}

bool WRCPReceiver::isInOurList(int message_number) {

	for (int i = 0; i < list_message_numbers.size(); i++) {
		if (list_message_numbers[i] == message_number)
			return true;
	}
	return false;
}

void WRCPReceiver::sendACK(WRCP packet) {
	WRCP ack_packet;
	ack_packet.createACK(packet);
	std::cout << "Already processed! Sending ACK.." << std::endl;
	this->outcoming_queue->post(ack_packet);
}

void WRCPReceiver::addToOurList(int message_number) {
	std::cout << message_number << std::endl;
	list_message_numbers.push_back(message_number);
	if (list_message_numbers.size() > MAX_RECEIVED_PACKETS_BUFFER)
		list_message_numbers.pop_back();
}
