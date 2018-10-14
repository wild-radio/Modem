#include <iostream>
#include "ModemInterface.hpp"
#include "ModemResolver.hpp"

int main(int argc, char **argv) {
	ModemInterface *modem = ModemResolver::resolve();

	if (argc < 3) {
		std::cout << "Not enough paramteres!\n";
		return 1;
	}

	std::string action = std::string(argv[1]);

	if (action == "tx") {
		std::string data = std::string(argv[2]);
		modem->write(data.c_str(), (int) data.size());

		return 0;
	}

	if (action == "rx") {
		char buffer[200];
		std::string options = std::string(argv[2]);
		modem->read(buffer, 10);

		std::cout << std::string(buffer) << std::endl;
	}
}