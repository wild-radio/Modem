#ifndef DECODER36_WRCPNUMBER_HPP
#define DECODER36_WRCPNUMBER_HPP


#include <mutex>

class WRCPNumber {
public:
	int8_t getNumber();
private:
	int8_t number;
	std::mutex mutex;
};


#endif //DECODER36_WRCPNUMBER_HPP
