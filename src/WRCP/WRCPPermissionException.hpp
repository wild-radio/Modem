#ifndef MODEM_WRCPPERMISSIONEXCEPTION_HPP
#define MODEM_WRCPPERMISSIONEXCEPTION_HPP

#include "../Exception.hpp"

class WRCPPermissionException : public Exception {
public:
	WRCPPermissionException(int code, const std::string &message) : Exception(code, message) {}
};

#endif //MODEM_WRCPPERMISSIONEXCEPTION_HPP
