#ifndef DECODER36_WRCPOTHERPACKETEXCEPTION_HPP
#define DECODER36_WRCPOTHERPACKETEXCEPTION_HPP

#include "../Exception.hpp"

class WRCPOtherPacketException : public Exception {
public:
	WRCPOtherPacketException(int code, const std::string &message) : Exception(code, message) {}
};


#endif //DECODER36_WRCPOTHERPACKETEXCEPTION_HPP
