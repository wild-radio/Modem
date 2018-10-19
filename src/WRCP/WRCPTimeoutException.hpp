#ifndef MODEM_WRCPTIMEOUTEXCEPTION_HPP
#define MODEM_WRCPTIMEOUTEXCEPTION_HPP

#include "../Exception.hpp"

class WRCPTimeoutException : public Exception {
public:
	WRCPTimeoutException(int code, const std::string &message) : Exception(code, message) {}
};

#endif //MODEM_WRCPTIMEOUTEXCEPTION_HPP
