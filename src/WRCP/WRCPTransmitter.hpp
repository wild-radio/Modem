#ifndef MODEM_WRCPTRANSMITTER_HPP
#define MODEM_WRCPTRANSMITTER_HPP


#include "../ThreadInterface.hpp"

class WRCPTransmitter : public ThreadInterface {
public:
	void run();
};


#endif //MODEM_WRCPTRANSMITTER_HPP
