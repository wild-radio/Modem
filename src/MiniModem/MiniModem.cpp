#include "MiniModem.hpp"
#include "StreamException.hpp"
#include "../ControlRecordAccess.hpp"


void MiniModem::writeData(const unsigned char *data, int size) {
	if (write_stream == nullptr) {
		std::string command = TX_COMMAND;
		std::FILE *stream = popen(command.c_str(), "w");
		this->ptt = PTTResolver::resolve();
	}

	if (write_stream == nullptr) {
		throw StreamException(1, "Failed to open the write_stream!");
	}

	this->ptt->push();
	fwrite(data, sizeof(char), (size_t) size, write_stream);
	pclose(write_stream);
	this->ptt->release();
}

int MiniModem::readData(unsigned char *data, int size) {
	if (read_stream == nullptr) {
		read_stream = popen((RX_COMMAND + " 2>/dev/null").c_str(), "r");
	}

	if (this->read_stream == nullptr) {
		throw StreamException(1, "Failed to open the write_stream!");
	}

	return fread(data, sizeof(unsigned char), size, this->read_stream);
}

