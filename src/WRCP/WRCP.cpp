#include <cstring>
#include "WRCP.hpp"

#include <string>
#include <iostream>

int8_t WRCP::packet_number = 0;
std::mutex WRCP::m_number;

WRCP::WRCP(unsigned char *buffer) {
	memcpy(&this->packet, buffer, WRCP_PACKET_SIZE);
}

void WRCP::print() {
	std::string as_msg = std::string((char*)&this->packet);
	std::cout << as_msg << std::endl;
}

unsigned char *WRCP::getData() {
	return (unsigned char*) &this->packet;
}

WRCP::~WRCP() {
}

void WRCP::createSlaveInformPresence(int8_t sender_id) {
	addProtocolIdentifier();
	this->packet.sender_id = sender_id;
	this->packet.receiver_id = 0;
	this->packet.action = this->getInformPresenceAction();
	this->packet.message_number = this->getPacketNumber();
	this->packet.checksum = this->calculateChecksum();
}

void WRCP::addProtocolIdentifier() {
	packet.protocol_identifier[0] = 'W';
	packet.protocol_identifier[1] = 'R';
	packet.protocol_identifier[2] = 'C';
	packet.protocol_identifier[3] = 'P';
}

int8_t WRCP::getRequestPhotoAction() {
	return 0x08;
}

int8_t WRCP::getCameraOptionsAction() {
	return 0x07;
}

int8_t WRCP::getAngleChangeAction() {
	return 0x06;
}

int8_t WRCP::getPhotoAction() {
	return 0x05;
}

int8_t WRCP::getRequestSendingRightsAction() {
	return 0x04;
}

int8_t WRCP::getInformPresenceAction() {
	return 0x03;
}

int8_t WRCP::getNACKAction() {
	return 0x02;
}

int8_t WRCP::getACKAction() {
	return 0x01;
}

int8_t WRCP::getPacketNumber() {
	std::lock_guard<std::mutex> lock(m_number);
	uint8_t number = WRCP::packet_number++;
	if (number == 255)
		WRCP::packet_number = 0;
	return number;
}

int16_t WRCP::calculateChecksum() {
	int16_t parts[7];
	int16_t checksum = 0;
	getPartsForChecksum(parts);

	for (int16_t &part : parts) {
		checksum += part;
	}

	return checksum;
}

void WRCP::getPartsForChecksum(int16_t *parts) {
	//Necessary so we dont count the checksum field
	int jump_checksum = 0;
	for (int i = 0; i < 6; i++) {
		if (i == 4)
			jump_checksum = 1;
		memcpy(parts + i, ((char16_t*)&this->packet) + i + jump_checksum, 2);
	}
	//Last part needs to be shifted
	parts[6] = ((int16_t)this->packet.data[4]) << 8;
}

bool WRCP::isValidChecksum() {
	int16_t checksum = this->calculateChecksum();
	return checksum == this->packet.checksum;
}

void WRCP::createNACK(WRCP packet_to_respond, int8_t sender_id) {
	this->addProtocolIdentifier();
	this->clearData();
	this->packet.sender_id = sender_id;
	this->packet.receiver_id = packet_to_respond.packet.sender_id;
	this->packet.message_number = packet_to_respond.getMessageNumber();
	this->packet.action = this->getNACKAction();
	this->packet.checksum = this->calculateChecksum();
}

bool WRCP::isForMe(int8_t id) {
	return packet.receiver_id == id;
}

void WRCP::createACK(WRCP packet_to_respond) {
	this->addProtocolIdentifier();
	this->packet.sender_id = packet_to_respond.packet.receiver_id;
	this->packet.receiver_id = packet_to_respond.packet.sender_id;
	this->packet.message_number = packet_to_respond.getMessageNumber();
	this->packet.action = this->getACKAction();
	this->packet.checksum = this->calculateChecksum();
}

void WRCP::createRequestSendingRights(int8_t sender_id) {
	this->addProtocolIdentifier();
	this->packet.sender_id = sender_id;
	this->packet.receiver_id = 0;
	this->packet.message_number = this->getPacketNumber();
	this->packet.action = this->getRequestSendingRightsAction();
	this->packet.checksum = this->calculateChecksum();
}

void WRCP::createPhoto(int8_t sender_id, int32_t timestamp, int8_t camera_id) {
	this->addProtocolIdentifier();
	this->packet.sender_id = sender_id;
	this->packet.receiver_id = 0;
	this->packet.message_number = this->getPacketNumber();
	this->packet.action = this->getPhotoAction();
	memcpy(this->packet.data, &timestamp, 4);
	this->packet.data[4] = camera_id;
	this->packet.checksum = this->calculateChecksum();
}

void WRCP::createAngleChange(int8_t receiver_id, int8_t angle_h, int8_t angle_v, int8_t camera_id) {
	this->addProtocolIdentifier();
	this->packet.sender_id = 0;
	this->packet.receiver_id = receiver_id;
	this->packet.message_number = this->getPacketNumber();
	this->packet.action = this->getAngleChangeAction();
	this->packet.data[0] = angle_h;
	this->packet.data[1] = angle_v;
	this->packet.data[4] = camera_id;
	this->packet.checksum = this->calculateChecksum();
}

void WRCP::createCameraOptions(int8_t receiver_id, int8_t timer_for_capture, int8_t use_sensor, int8_t enable, int8_t camera_id) {
	this->addProtocolIdentifier();
	this->packet.sender_id = 0;
	this->packet.receiver_id = receiver_id;
	this->packet.message_number = this->getPacketNumber();
	this->packet.action = this->getCameraOptionsAction();
	this->packet.data[0] = timer_for_capture;
	this->packet.data[1] = use_sensor;
	this->packet.data[2] = enable;
	this->packet.data[4] = camera_id;
	this->packet.checksum = this->calculateChecksum();
}

void WRCP::createRequestPhoto(int8_t receiver_id, int8_t camera_id) {
	this->addProtocolIdentifier();
	this->packet.sender_id = 0;
	this->packet.receiver_id = receiver_id;
	this->packet.message_number = this->getPacketNumber();
	this->packet.action = this->getRequestPhotoAction();
	this->packet.data[4] = camera_id;
	this->packet.checksum = this->calculateChecksum();
}

int8_t WRCP::getCameraId() {
	return this->packet.data[4];
}

int32_t WRCP::getTimestamp() {
	int32_t timestamp;
	memcpy(&timestamp, this->packet.data, 4);
	return timestamp;
}

int8_t WRCP::getHorizontalAngle() {
	return this->packet.data[0];
}

int8_t WRCP::getVerticalAngle() {
	return this->packet.data[1];
}

int8_t WRCP::getTimerForCapture() {
	return this->packet.data[0];
}

int8_t WRCP::getUseSensor() {
	return this->packet.data[1];
}

int8_t WRCP::getCameraEnable() {
	return this->packet.data[2];
}

bool WRCP::operator==(WRCP &packet) const {
	bool is_same_sender = this->packet.sender_id == packet.packet.sender_id;
	bool is_same_number = this->packet.message_number == packet.packet.message_number ;

	return is_same_sender && is_same_number;
}

void WRCP::clearData() {
	this->packet.data[0] = 0;
	this->packet.data[1] = 0;
	this->packet.data[2] = 0;
	this->packet.data[3] = 0;
}

_wrcp_packet WRCP::getPacket() {
	return this->packet;
}

const int8_t WRCP::getMessageNumber() {
	return packet.message_number;
}
