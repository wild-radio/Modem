#include <iostream>
#include <string>
#include "ModemInterface.hpp"
#include "ModemResolver.hpp"
#include "WRCP/WRCPController.hpp"
#include "Notification/CameraConfigurationsEventMonitor.hpp"
#include "SSTV/Robot36Encoder.hpp"
#include "SSTV/Robot36Decoder.hpp"

int main(int argc, char **argv) {

	auto id = std::stoi(std::string(argv[1]));
	WRCPController controller(id);

	controller.startReceiver();
	controller.startTransmitter();
	if (id == 1) {
		controller.startSlaveNotifications();
		controller.mainLoop();
	}
	controller.startMasterNotifications();
	controller.mainLoop();
}