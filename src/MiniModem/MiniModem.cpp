#include "MiniModem.hpp"
#include "StreamException.hpp"
#include <cstdio>

void MiniModem::write(const char *data, int size) {
	std::string command = std::string(TX_COMMAND) + this->alsa_option;
	std::FILE *stream = popen(command.c_str(), "w");

	if (stream == nullptr) {
		throw StreamException(1, "Failed to open the stream!");
	}

	digitalWrite (GPIO_PIN, HIGH);
	fwrite(data, sizeof(char), (size_t) size, stream);
	digitalWrite (GPIO_PIN, LOW);
	fclose(stream);
}

int MiniModem::read(char *data, int size) {
	std::string command = std::string(RX_COMMAND) + this->alsa_option;

	if (this->read_stream == nullptr) {
		this->read_stream = popen(command.c_str(), "r");
	}

	if (this->read_stream == nullptr) {
		throw StreamException(1, "Failed to open the stream!");
	}

	int read_size = (int) fread(data, sizeof(char), size, this->read_stream);
	fclose(this->read_stream);
	return read_size;
}

