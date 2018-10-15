#ifndef MODEM_WRCPRECEIVER_HPP
#define MODEM_WRCPRECEIVER_HPP


#include "../ThreadInterface.hpp"

class WRCPReceiver : public ThreadInterface {
public:
	void run();
};


#endif //MODEM_WRCPRECEIVER_HPP
