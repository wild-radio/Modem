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
		this->read_stream = nullptr;
	}
	void writeData(const unsigned char *data, int size) override;
	int readData(unsigned char *data, int size) override;

	void setOptions(std::string options) override {
		this->alsa_option = options;
	}

private:
	PTT *ptt = PTTResolver::resolve();
	std::FILE *read_stream;
	std::string alsa_option;
	std::string TX_COMMAND = "minimodem -R 44100 --tx 120 ";
	std::string RX_COMMAND = "minimodem -R 44100 --rx 120 -c 1.5";
};


#endif //MODEM_MINIMODEM_HPP
