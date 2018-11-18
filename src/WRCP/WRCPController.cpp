#include <iostream>
#include "WRCPController.hpp"
#include "WRCPReceiver.hpp"
#include "WRCPTransmitter.hpp"
#include "WRCPTimeoutException.hpp"
#include "WRCPPermissionException.hpp"
#include "../SSTV/Robot36.hpp"
#include "../SendPhotoToServer/SendPhotoToServer.hpp"
#include "../Notification/CameraConfigurationsEventMonitor.hpp"
#include "../Notification/PhotosEventMonitor.hpp"
#include <sstream>
#include <sys/mman.h>
#include <fcntl.h>
#include <zconf.h>
#include <ctime>

void WRCPController::startReceiver() {
	auto receiver = new WRCPReceiver(&this->incoming_packets, &this->outcoming_packets);
	this->receiver_thread = new std::thread(&WRCPReceiver::run, receiver);
}

void WRCPController::startTransmitter() {
	auto transmitter = new WRCPTransmitter(&this->outcoming_packets);
	this->transmitter_thread = new std::thread(&WRCPTransmitter::run, transmitter);
}

void WRCPController::startMasterNotifications() {
	auto monitor_main = new CameraConfigurationsEventMonitor(
			"/home/pi/.wildradio/config/WR001",
			"principal",
			&this->incoming_notifications,
			MAIN_CAMERA_ID
	);
	auto monitor_secondary = new CameraConfigurationsEventMonitor(
			"/home/pi/.wildradio/config/WR001",
			"alternativa",
			&this->incoming_notifications,
			SECONDARY_CAMERA_ID
	);
	this->main_monitor_thread = new std::thread(&CameraConfigurationsEventMonitor::run, monitor_main);
	this->secondary_monitor_thread = new std::thread(&CameraConfigurationsEventMonitor::run, monitor_secondary);
}

void WRCPController::startSlaveNotifications() {
	this->main_config.loadConfigurations("/home/pi/.wildradio/config/principal.txt");
	this->secondary_config.loadConfigurations("/home/pi/.wildradio/config/alternativa.txt");

	auto monitor_main = new PhotosEventMonitor(
			main_photo_path,
			&this->incoming_notifications,
			MAIN_CAMERA_ID
	);
	auto secondary_main = new PhotosEventMonitor(
			secondary_photo_path,
			&this->incoming_notifications,
			SECONDARY_CAMERA_ID
	);

	this->main_photo_monitor_thread = new std::thread(&PhotosEventMonitor::run, monitor_main);
	this->secondary_photo_monitor_thread = new std::thread(&PhotosEventMonitor::run, secondary_main);
}

void WRCPController::mainLoop() {
	if (this->isSlave()) {
		//TODO Remove comentary
		this->sendInformPresence();
	}
	this->id_with_transmission_rights = 0;

	while (true) {
		if (this->incoming_packets.hasMessage())
			this->handlePacket();

		if (this->incoming_notifications.hasMessage()) {
			this->handleNotifications();
		}
	}
}

void WRCPController::handlePacket() {
	WRCP packet = incoming_packets.pull();
	processPacket(packet);
}

void WRCPController::handleNotifications() {
	Notification notification = this->incoming_notifications.pull();
	processMasterNotification(notification);
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
			r_timeout = rand() % 5 + 4;
		try {
			success = resendWhileNotAck(r_packet, r_timeout);
			break;
		} catch (WRCPTimeoutException exception) {
			success = false;
			std::cout << "Trying to resend packet" << std::endl;
			this->outcoming_packets.post(packet);
		}
	}

	return success;
}

bool WRCPController::resendWhileNotAck(WRCP &r_packet, int r_timeout) {
	for (int i = 0; i < 5; i++) {
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

		auto current = std::chrono::steady_clock::now();
		std::chrono::duration<float> diff = current - start;
		if (diff.count() >= timeout)
			throw WRCPTimeoutException(1, "Timeout!");
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

	if (this->id == 0 && packet.getSender() != this->id_with_transmission_rights) {
		//this->incoming_packets.post(packet);
		return;
	}

	if (packet.isPhoto()) {
		handlePhotoReciever(packet);
		this->id_with_transmission_rights = 0;
		return;
	}



	if (packet.isAngleChange()) {
		std::cout <<  "Master requested an angle change in the horizontal of "
		          << (int)packet.getHorizontalAngle() << " degrees and in the vertical of "
		          << (int)packet.getVerticalAngle() << " degres for the camera "
		          << (int)packet.getCameraId() << std::endl;
		this->updateAngle(packet);
		this->sendACK(packet);
		return;
	}

	if (packet.isCameraOptions()) {
		std::cout <<  "Master requested an camera options change, timer for capture of "
		          << (int)packet.getTimerForCapture() << " and use sensor is "
		          << (int)packet.getUseSensor() << " for the camera "
		          << (int)packet.getCameraId() << std::endl;
		this->updateOptions(packet);
		this->sendACK(packet);
		return;
	}

	if (packet.isRequestPhoto()) {
		std::cout << "Master requested a photo from the camera " << (int)packet.getCameraId() << std::endl;
		this->requestPhoto(packet);
		this->sendACK(packet);
		sleep(3);
		std::string photo_path = getPhotoPath(packet);

		this->sendPhoto(123124, packet.getCameraId(), std::__cxx11::string());
		return;
	}

	std::cout << "Packet received with an invalid action!" << std::endl;
}

std::string WRCPController::getPhotoPath(WRCP &packet) const {
	std::__cxx11::string photo_path = (packet.getCameraId() == MAIN_CAMERA_ID) ?
	                                  std::__cxx11::string(main_photo_path) + "/confirmation_cam_1.ppm" :
	                                  std::__cxx11::string(secondary_photo_path) + "/confirmation_cam_2.ppm";
	return photo_path;
}

void WRCPController::handlePhotoReciever(WRCP &packet) {
	id_with_transmission_rights = 0;
	request_photo = false;
	std::cout << packet.getSender() << " will send a photo with the timestamp "
	          << packet.getTimestamp() << " from the camera "
	          << packet.getCameraId() << std::endl;
	sendACK(packet);

	sendPhotoToServer(packet);
}

void WRCPController::sendPhotoToServer(WRCP &packet) {
	Robot36Decoder decoder;
	SendPhotoToServer sender;
	auto ppm_filename = "temp_photo.ppm";
	decoder.decoder(ppm_filename);
	auto png_filename = "temp_photo.png";
	this->convertPPMToPNG(ppm_filename, png_filename);

	auto suffix = (request_photo) ? "configuracao/confirmacao/foto" : "fotos";
	sender.sendPhoto(png_filename, id, packet.getCameraId(), packet.getTimestamp(), suffix);
}

void WRCPController::sendACK(WRCP packet) {
	WRCP ack_packet;
	ack_packet.createACK(packet);
	sleep(1);
	this->outcoming_packets.post(ack_packet);
}

void WRCPController::sendNACK(WRCP packet) {
	WRCP nack_packet;
	nack_packet.createNACK(packet, this->id);
	sleep(1);
	this->outcoming_packets.post(nack_packet);
}

void WRCPController::sendInformPresence() {
	WRCP presence_packet;
	presence_packet.createSlaveInformPresence(this->id);
	this->outcoming_packets.post(presence_packet);

	bool success = handleACKAndNACK(presence_packet, 5, DEFAULT_TIMEOUT);
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

void WRCPController::sendPhoto(int32_t timestamp, int8_t camera_id, std::string photo_path) {
	WRCP photo_packet;
	photo_packet.createPhoto(this->id, timestamp, camera_id);

	this->outcoming_packets.post(photo_packet);

	bool success = handleACKAndNACK(photo_packet, 3, DEFAULT_TIMEOUT);

	if (!success) {
		std::cout << "Failed trying to transmit photo info!" << std::endl;
		return;
	}

	std::cout << "Transmitted photo info!" << std::endl;

	if (photo_path.empty()) {
		photo_path = this->getPhotoPathFromTimestampAndCameraId(timestamp, camera_id);
	}

	std::cout << photo_path << std::endl;

	sstv.encode(photo_path);

	std::cout << "Image transmitted!" << std::endl;
}

void WRCPController::sendAngleChange(int8_t receiver_id, int8_t angle_h, int8_t angle_v, int8_t camera_id) {
	if (this->isSlave())
		throw WRCPPermissionException(1, "Slave can't send this packet!");

	WRCP angle_packet;
	angle_packet.createAngleChange(receiver_id, angle_h, angle_v, camera_id);

	this->outcoming_packets.post(angle_packet);

	bool success = handleACKAndNACK(angle_packet, 3, DEFAULT_TIMEOUT);
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

	bool success = handleACKAndNACK(options_packet, 3, DEFAULT_TIMEOUT);
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

	bool success = handleACKAndNACK(request_photo_packet, 3, DEFAULT_TIMEOUT + 4);
	if (!success) {
		std::cout << "Failed requesting photo!" << std::endl;
		return;
	}

	std::cout << "Photo requested!" << std::endl;
	this->request_photo = true;
	this->id_with_transmission_rights = receiver_id;
}

void WRCPController::processMasterNotification(Notification notification) {
	if (notification.type == NotificationType::MODIFY_OPTIONS) {
		this->sendCameraOptions(!id, notification.timer, notification.sensor, notification.camera_id);
		return;
	}

	if (notification.type == NotificationType::NEW_ANGLE) {
		this->sendAngleChange(!id, notification.angle_h, notification.angle_v, notification.camera_id);
	}

	if (notification.type == NotificationType::REQUEST_CAPTURE) {
		this->sendRequestPhoto(!id, notification.camera_id);
	}

	if (notification.type == NotificationType::NEW_PHOTO) {
		this->sendPhoto(notification.timestamp, notification.camera_id, "");
	}
}

void WRCPController::convertPPMToPNG(std::string ppm_filename, std::string png_filename) {
	std::system(std::string(std::string("magick ") + ppm_filename + std::string(" ") + png_filename).c_str());
}

void WRCPController::updateAngle(WRCP packet) {
	auto *config = getConfigurations(packet.getCameraId());
	config->vertical = packet.getVerticalAngle();
	config->horizontal = packet.getHorizontalAngle();
	config->request_photo = 0;
	config->saveConfigurations();
}

void WRCPController::updateOptions(WRCP packet) {
	auto *config = getConfigurations(packet.getCameraId());
	config->sensor = packet.getUseSensor();
	config->timer = packet.getTimerForCapture();
	config->request_photo = 0;
	config->saveConfigurations();
}

void WRCPController::requestPhoto(WRCP packet) {
	auto *config = getConfigurations(packet.getCameraId());
	config->request_photo = 1;
	config->saveConfigurations();
}

CameraConfigurations *WRCPController::getConfigurations(int camera_id){
	if (camera_id == MAIN_CAMERA_ID)
		return &main_config;
	return &secondary_config;
}

std::string WRCPController::getPhotoPathFromTimestampAndCameraId(int32_t timestamp, int8_t camera_id) {
	std::stringstream timestamp_str;
	timestamp_str << "/" << timestamp << ".ppm";
	std::__cxx11::string photo_path = (camera_id == MAIN_CAMERA_ID) ?
	                                  std::__cxx11::string(main_photo_path):
	                                  std::__cxx11::string(secondary_photo_path);

	return photo_path + timestamp_str.str();
}

