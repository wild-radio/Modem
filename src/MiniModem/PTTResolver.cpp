#include "PTTResolver.hpp"

PTT PTTResolver::ptt;

PTT *PTTResolver::resolve() {
	return &PTTResolver::ptt;
}
