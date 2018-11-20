#ifndef MODEM_WRCPTRANSMITTER_HPP
#define MODEM_WRCPTRANSMITTER_HPP


#include "../ThreadInterface.hpp"
#include "WRCP.hpp"
#include "../MessageQueue/MessageQueue.hpp"
#include "../MiniModem/MiniModem.hpp"

class WRCPTransmitter : public ThreadInterface {
public:
	explicit WRCPTransmitter(MessageQueue<WRCP> *outcoming_packets) {
		this->outcoming_packets = outcoming_packets;
	}
	void run() override;

private:
	MessageQueue<WRCP> *outcoming_packets;
	MiniModem modem;
};


#endif //MODEM_WRCPTRANSMITTER_HPP
