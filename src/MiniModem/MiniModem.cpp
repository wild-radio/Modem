#include "MiniModem.hpp"
#include "StreamException.hpp"
#include "../ControlRecordAccess.hpp"


void MiniModem::writeData(const unsigned char *data, int size) {
	std::string command = TX_COMMAND;

	this->ptt = PTTResolver::resolve();
	this->write_stream = popen(command.c_str(), "w");

	if (write_stream == nullptr) {
		throw StreamException(1, "Failed to open the write_stream!");
	}

	this->ptt->push();
	fwrite(data, sizeof(char), (size_t) size, write_stream);
	pclose(write_stream);
	this->ptt->release();
}

int MiniModem::readData(unsigned char *data, int size) {
	return fread(data, sizeof(unsigned char), size, this->read_stream);
}

