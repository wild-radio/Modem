#ifndef MODEM_MINIMODEM_HPP
#define MODEM_MINIMODEM_HPP

#include <cstdio>
#include <string>
#include "../ModemInterface.hpp"
#include "PTT.hpp"
#include "PTTResolver.hpp"

class MiniModem : public ModemInterface {
public:
	MiniModem() {
		this->read_stream = popen((RX_COMMAND + " 2>/dev/null").c_str(), "r");;
		this->write_stream = nullptr;
	}
	void writeData(const unsigned char *data, int size) override;
	int readData(unsigned char *data, int size) override;

	void setOptions(std::string options) override {
		this->alsa_option = options;
	}

private:
	PTT *ptt;
	std::FILE *read_stream;
	std::FILE *write_stream;
	std::string alsa_option;
	std::string TX_COMMAND = "minimodem -R 44100 --tx 300";
	std::string RX_COMMAND = "minimodem -R 44100 --rx 300 -c 1";
};


#endif //MODEM_MINIMODEM_HPP
