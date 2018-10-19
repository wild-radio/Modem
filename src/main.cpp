#include <iostream>
#include "ModemInterface.hpp"
#include "ModemResolver.hpp"
#include "WRCP/WRCPController.hpp"
#include "test/TestModem.hpp"
#include "test/FileModem.hpp"

int main(int argc, char **argv) {
	FileModem::setInFile("inf.txt");
	FileModem::prepareNotify();
	ModemResolver::setOverride(new FileModem);

	ModemInterface *modem = ModemResolver::resolve();
	WRCPController controller(0);
	unsigned char data;
	modem->readData(&data, 1);
	//TestModem::txToRx();
	//TestModem::insertIntoBuffer((unsigned char *) "WRCP12121212121", 15);

	controller.startReceiver();
	controller.startTransmitter();
	controller.mainLoop();

}