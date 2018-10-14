//
// Created by clemente on 14/10/18.
//

#ifndef MODEM_MODEMRESOLVER_HPP
#define MODEM_MODEMRESOLVER_HPP


#include "ModemInterface.hpp"

class ModemResolver {
public:
	static ModemInterface *resolve();

private:
	static ModemInterface *instance;
};


#endif //MODEM_MODEMRESOLVER_HPP
