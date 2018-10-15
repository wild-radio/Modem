#include "MiniModem.hpp"
#include "StreamException.hpp"

void MiniModem::write(const char *data, int size) {
	std::string command = std::string(TX_COMMAND) + this->alsa_option;
	std::FILE *stream = popen(command.c_str(), "w");

	if (stream == nullptr) {
		throw StreamException(1, "Failed to open the stream!");
	}

	this->ptt.push();
	fwrite(data, sizeof(char), (size_t) size, stream);
	fclose(stream);
	this->ptt.release();
}

int MiniModem::read(char *data, int size) {
	std::string command = std::string(RX_COMMAND) + this->alsa_option + " 2>/dev/null";

	if (this->read_stream == nullptr) {
		this->read_stream = popen(command.c_str(), "r");
	}

	if (this->read_stream == nullptr) {
		throw StreamException(1, "Failed to open the stream!");
	}

	int read_size = (int) fread(data, sizeof(char), size, this->read_stream);
	return read_size;
}

