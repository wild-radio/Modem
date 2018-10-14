//
// Created by clemente on 14/10/18.
//

#include "ModemResolver.hpp"
#include "MiniModem/MiniModem.hpp"

ModemInterface* ModemResolver::instance = nullptr;

ModemInterface *ModemResolver::resolve() {
	if (ModemResolver::instance == nullptr)
		ModemResolver::instance = new MiniModem();

	return ModemResolver::instance;
}
