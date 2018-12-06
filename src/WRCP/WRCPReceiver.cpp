#include <iostream>
#include <strings.h>
#include "WRCPReceiver.hpp"
#include "../MiniModem/MinimodemResolver.hpp"

void WRCPReceiver::run() {
	this->receiving = false;
	bool received_w = false;
	bool received_r = false;
	this->buffer = new unsigned char[WRCP_PACKET_SIZE];
	unsigned char byte[15];
	this->clearBuffer();
	MiniModem *modem = MinimodemResolver::resolve();
	while (true) {
		int len = modem->readData(byte, 1);
		for (int i = 0; i < len; i++ ) {
			/*if (len == 0)
				continue;*/
			if (byte[i] == 'W') {
				received_w = true;
				continue;
			}

			if (byte[i] == 'R' && received_w) {
				received_r = true;
				continue;
			}

			if (byte[i] == 'C' && received_r) {
				this->receiving = true;
				received_w = false;
				received_r = false;
				clearBuffer();
				this->addToBuffer('W');
				this->addToBuffer('R');
				this->addToBuffer('C');
				continue;
			}

			if (this->receiving) {
				this->addToBuffer(byte[i]);
				continue;
			}

			if (received_w) {
				received_w = false;
			}
			if (received_r) {
				received_r = false;
			}
		}
	}
}

void WRCPReceiver::clearBuffer() {
	this->buffer_pos = 0;
	bzero(this->buffer, WRCP_PACKET_SIZE);
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
	this->receiving = false;

	WRCP packet = WRCP(this->buffer);
	this->clearBuffer();
	auto valid = packet.isValidChecksum();
	if (!valid) {
		std::cout << "Checksum error!" << std::endl;
		return;
	}

	auto isNotAckNorNack = !packet.isACK() && !packet.isNACK();
	int message_number = packet.getMessageNumber();
	std::cout << "Message number:" << message_number << std::endl;
	if (isNotAckNorNack && this->isInOurList(message_number)) {
		this->sendACK(packet);
		return;
	}
	if (isNotAckNorNack)
		this->addToOurList(message_number);
	this->incoming_queue->post(packet);
}

bool WRCPReceiver::isInOurList(int message_number) {
	/*for (int i = 0; i < list_message_numbers.size(); i++) {
		if (list_message_numbers[i] == message_number)
			return true;
	}
	return false;*/
	if (last_message_number == message_number)
		return true;
	return false;
}

void WRCPReceiver::sendACK(WRCP packet) {
	WRCP ack_packet;
	ack_packet.createACK(packet);
	std::cout << "Already processed! Sending ACK.." << std::endl;
	this->outcoming_queue->post(ack_packet);
}

void WRCPReceiver::addToOurList(int message_number) {
	/*list_message_numbers.insert(list_message_numbers.begin(), message_number);
	//list_message_numbers.push_back(message_number);
	if (list_message_numbers.size() > MAX_RECEIVED_PACKETS_BUFFER)
		list_message_numbers.pop_back();*/
	last_message_number = message_number;
}
