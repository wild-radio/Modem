#include <iostream>
#include <string>
#include "ModemInterface.hpp"
#include "ModemResolver.hpp"
#include "WRCP/WRCPController.hpp"
#include "test/TestModem.hpp"
#include "test/FileModem.hpp"
#include "Notification/CameraConfigurationsEventMonitor.hpp"

int main(int argc, char **argv) {
//	ModemResolver::setOverride(new FileModem);

	ModemInterface *modem = ModemResolver::resolve();
	WRCPController controller(std::stoi(std::string(argv[1])));
	//TestModem::txToRx();
	//TestModem::insertIntoBuffer((unsigned char *) "WRCP12121212121", 15);
	/*controller.startReceiver();
	controller.startTransmitter();
	controller.startMasterNotifications();*/
	controller.startSlaveNotifications();
	controller.mainLoop();

}