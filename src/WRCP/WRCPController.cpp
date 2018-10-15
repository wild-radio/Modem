#include <iostream>
#include "WRCPController.hpp"
#include "WRCPReceiver.hpp"
#include "WRCPTransmitter.hpp"

void WRCPController::startReceiver() {
	auto receiver = new WRCPReceiver;
	this->receiver_thread = new std::thread(&WRCPReceiver::run, receiver);
}

void WRCPController::startTransmitter() {
	auto transmitter = new WRCPTransmitter;
	this->transmitter_thread = new std::thread(&WRCPTransmitter::run, transmitter);
}

void WRCPController::mainLoop() {
	while (true) {
		std::cout << "Controller\n";
	}
}
