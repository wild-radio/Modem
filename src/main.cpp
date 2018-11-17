#include <iostream>
#include <string>
#include "ModemInterface.hpp"
#include "ModemResolver.hpp"
#include "WRCP/WRCPController.hpp"
#include "Notification/CameraConfigurationsEventMonitor.hpp"
#include "SSTV/Robot36Encoder.hpp"
#include "SSTV/Robot36Decoder.hpp"

int main(int argc, char **argv) {

	ModemInterface *modem = ModemResolver::resolve();
	WRCPController controller(std::stoi(std::string(argv[1])));

	controller.startReceiver();
	controller.startTransmitter();
	//controller.startMasterNotifications();
	//controller.startSlaveNotifications();
	controller.mainLoop();
}