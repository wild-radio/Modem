//
// Created by clemente on 14/10/18.
//

#include "ModemResolver.hpp"
#include "MiniModem/MiniModem.hpp"
#include "test/TestModem.hpp"

ModemInterface* ModemResolver::instance = nullptr;

void ModemResolver::setOverride(ModemInterface *instance) {
	ModemResolver::instance = instance;
}

ModemInterface *ModemResolver::resolve() {
	if (ModemResolver::instance == nullptr)
		ModemResolver::instance = new MiniModem();

	return ModemResolver::instance;

}
