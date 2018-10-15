#include <iostream>
#include "ModemInterface.hpp"
#include "ModemResolver.hpp"

int main(int argc, char **argv) {
	ModemInterface *modem = ModemResolver::resolve();

	std::string action = std::string(argv[1]);

	if (action == "tx") {
		std::string data = std::string(argv[2]);

		if (argc == 4) {
			std::string options = std::string(argv[3]);
			modem->setOptions(options);
		}
		modem->write(data.c_str(), (int) data.size());

		return 0;
	}

	if (action == "rx") {

		if (argc == 3) {
			std::string options = std::string(argv[3]);
			modem->setOptions(options);
		}

		while (true) {
			char buffer[200];
			int size = modem->read(buffer, 1);
			buffer[size] = 0;
			std::cout << std::string(buffer) << std::flush;

		}
	}
}