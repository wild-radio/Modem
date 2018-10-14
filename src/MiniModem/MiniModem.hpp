#ifndef MODEM_MINIMODEM_HPP
#define MODEM_MINIMODEM_HPP

static const char *const TX_COMMAND = "minimodem -R 44100 --tx 1200 ";

static const char *const RX_COMMAND = "minimodem -R 44100 --rx 1200 -l 1.5 ";

static const int GPIO_PIN = 15;

#include <cstdio>
#include <string>
#include <wiringPi.h>
#include "../ModemInterface.hpp"

class MiniModem : public ModemInterface {
public:
	MiniModem() {
		wiringPiSetup();
		pinMode (GPIO_PIN, OUTPUT) ;
	}
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
