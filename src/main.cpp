#include <iostream>
#include <string>
#include <wiringPi.h>
#include "WRCP/WRCPController.hpp"
#include "SSTV/Robot36Encoder.hpp"
#include "SSTV/Robot36Decoder.hpp"

int main(int argc, char **argv) {
	std::string path = argv[1];
	Robot36Encoder encoder;
	encoder.encode(path);
	/*auto id = std::stoi(std::string(argv[1]));
	WRCPController controller(id);

	controller.startReceiver();
	controller.startTransmitter();
	if (id == 1) {
		controller.startSlaveNotifications();
		controller.mainLoop();
	}
	controller.startMasterNotifications();
	controller.mainLoop();*/
}