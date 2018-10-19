#include <iostream>
#include "WRCPController.hpp"
#include "WRCPReceiver.hpp"
#include "WRCPTransmitter.hpp"
#include "WRCPTimeoutException.hpp"
#include "WRCPPermissionException.hpp"
#include <chrono>

void WRCPController::startReceiver() {
	auto receiver = new WRCPReceiver(&this->incoming_packets, &this->outcoming_packets);
	this->receiver_thread = new std::thread(&WRCPReceiver::run, receiver);
}

void WRCPController::startTransmitter() {
	auto transmitter = new WRCPTransmitter(&this->outcoming_packets);
	this->transmitter_thread = new std::thread(&WRCPTransmitter::run, transmitter);
}

void WRCPController::mainLoop() {
	if (this->isSlave()) {
		this->sendInformPresence();
	}

	//this->sendAngleChange(1, 45, 45, 0);
//	this->sendCameraOptions(1, 5, WRCP_ACTIVATE_SENSOR, 0);
//	this->sendRequestPhoto(1, 0);
//	this->sendRequestSendingRights();
//	this->sendPhoto(12315234, 0);

	while (true) {
		if (!this->incoming_packets.hasMessage())
			continue;

		WRCP packet = this->incoming_packets.pull();
		this->processPacket(packet);

	}
}

bool WRCPController::isSlave() {
	return this->id != 0;
}

bool WRCPController::handleACKAndNACK(const WRCP &packet, int trys, int timeout) {
	WRCP r_packet;
	bool success = false;
	int r_timeout = timeout;
	for (int i = 0; i < trys; i++) {
		if (timeout == -1)
			r_timeout = rand() % 5 + 2;
		try {
			success = resendWhileNotAck(r_packet, r_timeout);
			break;
		} catch (WRCPTimeoutException exception) {
			success = false;
			std::cout << "Trying to send packet" << std::endl;
			this->outcoming_packets.post(packet);
		}
	}

	return success;
}

bool WRCPController::resendWhileNotAck(WRCP &r_packet, int r_timeout) {
	for (int i = 0; i < 15; i++) {
		r_packet = getReturn(r_timeout);

		if (!r_packet.isValidChecksum())
			continue;

		if (!r_packet.isACK())
			continue;

		return true;
	}

	return false;
}

WRCP WRCPController::getReturn(float timeout) {
	auto start = std::chrono::steady_clock::now();

	while (true) {
		if (this->incoming_packets.hasMessage())
			break;

		/*auto current = std::chrono::steady_clock::now();
		std::chrono::duration<float> diff = current - start;
		if (diff.count() >= timeout)
			throw WRCPTimeoutException(1, "Timeout!");*/
	}

	return this->incoming_packets.pull();
}

void WRCPController::processPacket(WRCP packet) {
	if (!packet.isValidChecksum()) {
		std::cout << "Received packet with invalid checksum. Sending nack" << std::endl;
		this->sendNACK(packet);
		return;
	}

	if (!packet.isForMe(this->id)) {
		std::cout << "Received packet for another person. Do nothing!" << std::endl;
	}

	if (this->id_with_transmission_rights !=0 && packet.getSender() != this->id_with_transmission_rights) {
		this->incoming_packets.post(packet);
		return;
	}

	if (packet.isPhoto()) {
		this->id_with_transmission_rights = 0;
		this->request_photo = false;
		std::cout << packet.getSender() << " will send a photo with the timestamp "
		          << packet.getTimestamp() << " from the camera "
		          << packet.getCameraId() << std::endl;
		this->sendACK(packet);
	}

	if (this->request_photo) {
		this->incoming_packets.post(packet);
		return;
	}

	if (packet.isInformPresenceAction()) {
		std::cout << (int)packet.getSender() << " is informing their presence to us!" << std::endl;
		this->other_ids[packet.getSender()] = true;
		this->sendACK(packet);
		return;
	}

	if (packet.isRequestSendingRights()) {
		this->id_with_transmission_rights = packet.getSender();
		std::cout << (int)packet.getSender() << " has the right to comunicate with us!" << std::endl;
		this->sendACK(packet);
		return;
	}

	if (packet.isAngleChange()) {
		std::cout <<  "Master requested an angle change in the horizontal of "
		          << (int)packet.getHorizontalAngle() << " degrees and in the vertical of "
		          << (int)packet.getVerticalAngle() << " degres for the camera "
		          << (int)packet.getCameraId() << std::endl;
		this->sendACK(packet);
		return;
	}

	if (packet.isCameraOptions()) {
		std::cout <<  "Master requested an camera options change, timer for capture of "
		          << (int)packet.getTimerForCapture() << " and use sensor is "
		          << (int)packet.getUseSensor() << " for the camera "
		          << (int)packet.getCameraId() << std::endl;
		this->sendACK(packet);
		return;
	}

	if (packet.isRequestPhoto()) {
		//TODO: Get photo.
		std::cout << "Mater requested a photo from the camera " << (int)packet.getCameraId() << std::endl;
		this->sendPhoto(123124, packet.getCameraId());
		return;
	}

	std::cout << "Packet received with an invalid action!" << std::endl;
}

void WRCPController::sendACK(WRCP packet) {
	WRCP ack_packet;
	ack_packet.createACK(packet);

	this->outcoming_packets.post(ack_packet);
}

void WRCPController::sendNACK(WRCP packet) {
	WRCP nack_packet;
	nack_packet.createNACK(packet, this->id);

	this->outcoming_packets.post(nack_packet);
}

void WRCPController::sendInformPresence() {
	WRCP presence_packet;
	presence_packet.createSlaveInformPresence(this->id);
	this->outcoming_packets.post(presence_packet);

	bool success = handleACKAndNACK(presence_packet, 5, 6);
	if (!success) {
		std::cout << "Presence timeout!" << std::endl;
		return;
	}

	std::cout << "Presence informed!" << std::endl;
}

void WRCPController::sendRequestSendingRights() {
	WRCP request_packet;
	request_packet.createRequestSendingRights(this->id);
	
	this->outcoming_packets.post(request_packet);

	bool success = handleACKAndNACK(request_packet, 5, -1);
	if (!success) {
		std::cout << "Failed requesting permission to comunicate!" << std::endl;
		return;
	}

	std::cout << "Got permission to comunicate!" << std::endl;
}

void WRCPController::sendPhoto(int32_t timestamp, int8_t camera_id) {
	WRCP photo_packet;
	photo_packet.createPhoto(this->id, timestamp, camera_id);

	this->outcoming_packets.post(photo_packet);

	bool success = handleACKAndNACK(photo_packet, 3, 2);
	if (!success) {
		std::cout << "Failed trying to transmit photo info!" << std::endl;
		return;
	}

	std::cout << "Transmitted photo info!" << std::endl;
}

void WRCPController::sendAngleChange(int8_t receiver_id, int8_t angle_h, int8_t angle_v, int8_t camera_id) {
	if (this->isSlave())
		throw WRCPPermissionException(1, "Slave can't send this packet!");

	WRCP angle_packet;
	angle_packet.createAngleChange(receiver_id, angle_h, angle_v, camera_id);

	this->outcoming_packets.post(angle_packet);

	bool success = handleACKAndNACK(angle_packet, 3, 2);
	if (!success) {
		std::cout << "Failed trying to change camera angle!" << std::endl;
		return;
	}

	std::cout << "Camera angle changed!" << std::endl;
}

void WRCPController::sendCameraOptions(int8_t receiver_id, int8_t timer_for_capture, int8_t use_sensor, int8_t camera_id) {
	if (this->isSlave())
		throw WRCPPermissionException(1, "Slave can't send this packet!");

	WRCP options_packet;
	options_packet.createCameraOptions(receiver_id, timer_for_capture, use_sensor, camera_id);

	this->outcoming_packets.post(options_packet);

	bool success = handleACKAndNACK(options_packet, 3, 2);
	if (!success) {
		std::cout << "Failed trying to change camera options!" << std::endl;
		return;
	}

	std::cout << "Camera options changed!" << std::endl;
}

void WRCPController::sendRequestPhoto(int8_t receiver_id, int8_t camera_id) {
	if (this->isSlave())
		throw WRCPPermissionException(1, "Slave can't send this packet!");

	WRCP request_photo_packet;
	request_photo_packet.createRequestPhoto(receiver_id, camera_id);

	this->outcoming_packets.post(request_photo_packet);

	bool success = handleACKAndNACK(request_photo_packet, 3, 5);
	if (!success) {
		std::cout << "Failed requesting photo!" << std::endl;
		return;
	}

	std::cout << "Photo requested!" << std::endl;
	this->request_photo = true;
	this->id_with_transmission_rights = receiver_id;
}

