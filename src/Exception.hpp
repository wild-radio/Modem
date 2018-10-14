//
// Created by clemente on 14/10/18.
//

#ifndef MODEM_EXCEPTION_HPP
#define MODEM_EXCEPTION_HPP

#include <exception>
#include <string>

class Exception : public std::exception{
public:
	Exception(int code, std::string message) {
		this->code = code;
		this->message = message;
	}

	std::string getMessage() {
		return this->message;
	}

	int getCodes() {
		return this->code;
	}

private:
	int code;
	std::string message;

};


#endif //MODEM_EXCEPTION_HPP
