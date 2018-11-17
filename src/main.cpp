#include <iostream>
#include <string>
#include "ModemInterface.hpp"
#include "ModemResolver.hpp"
#include "WRCP/WRCPController.hpp"
#include "Notification/CameraConfigurationsEventMonitor.hpp"
#include "SSTV/Robot36Encoder.hpp"
#include "SSTV/Robot36Decoder.hpp"

int main(int argc, char **argv) {
//	ModemResolver::setOverride(new FileModem);

	ModemInterface *modem = ModemResolver::resolve();
	WRCPController controller(std::stoi(std::string(argv[1])));

	std::string cmd = argv[1];
	if (cmd == "0") {
		auto encoder = new Robot36Encoder();
		encoder->transmit("photo.ppm");
		return 0;
	}

	auto decoder = new Robot36Decoder();
	decoder->decoder("teste.ppm");

	/*controller.startReceiver();
	controller.startTransmitter();
	controller.startMasterNotifications();*/
	//controller.startSlaveNotifications();
	//controller.mainLoop();
}