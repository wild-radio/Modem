//
// Created by clemente on 14/10/18.
//

#ifndef MODEM_STREAMEXCEPTION_HPP
#define MODEM_STREAMEXCEPTION_HPP

#include "../Exception.hpp"

class StreamException : public Exception {
public:
	StreamException(int code, const std::string &message) : Exception(code, message) {}
};

#endif //MODEM_STREAMEXCEPTION_HPP
