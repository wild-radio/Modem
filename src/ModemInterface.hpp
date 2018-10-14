#ifndef MODEM_MODEMINTERFACE_HPP
#define MODEM_MODEMINTERFACE_HPP

#include <string>

class ModemInterface {
public:
	virtual void write(const char *data, int size) = 0;
	virtual int read(char *data, int size) = 0;
	virtual void setReadOptions(std::string options) = 0;
};

#endif //MODEM_MODEMINTERFACE_HPP
