#ifndef MODEM_MINIMODEM_HPP
#define MODEM_MINIMODEM_HPP

static const char *const TX_COMMAND = "minimodem -R 44100 --tx 1200 ";

static const char *const RX_COMMAND = "minimodem -R 44100 --rx 1200 -l 1.5 ";

#include <cstdio>
#include <string>
#include "../ModemInterface.hpp"

class MiniModem : public ModemInterface {
public:
	void write(const char *data, int size) override;
	int read(char *data, int size) override;

	void setOptions(std::string options){
		this->alsa_option = options;
	}

private:
	std::FILE *read_stream;
	std::string alsa_option;
};


#endif //MODEM_MINIMODEM_HPP
