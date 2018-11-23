#include "MinimodemResolver.hpp"

MiniModem MinimodemResolver::modem;

MiniModem *MinimodemResolver::resolve() {
	return &modem;
}
