//
// Created by clemente on 21/11/18.
//

#include "WRCPNumber.hpp"

int8_t WRCPNumber::getNumber() {
	std::lock_guard<std::mutex> lock(mutex);
	++this->number;
	if (this->number == 255)
		this->number = 0;
	return this->number;
}
