#ifndef MODEM_MODEMINTERFACE_HPP
#define MODEM_MODEMINTERFACE_HPP

#include <string>

class ModemInterface {
public:
	virtual void writeData(const unsigned char *data, int size) = 0;
	virtual int readData(unsigned char *data, int size) = 0;
	virtual void setOptions(std::string options) = 0;
};

#endif //MODEM_MODEMINTERFACE_HPP
