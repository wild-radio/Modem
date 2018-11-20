#ifndef MODEM_WRCPRECEIVER_HPP
#define MODEM_WRCPRECEIVER_HPP

static const int MAX_RECEIVED_PACKETS_BUFFER = 15;

#include <vector>
#include "../ThreadInterface.hpp"
#include "WRCP.hpp"
#include "../MessageQueue/MessageQueue.hpp"
#include "../MiniModem/MiniModem.hpp"

class WRCPReceiver : public ThreadInterface {
public:
	WRCPReceiver(MessageQueue<WRCP> *incoming_queue, MessageQueue<WRCP> *outcoming_queue) {
		this->buffer_pos = 0;
		this->buffer = nullptr;
		this->receiving = false;
		this->incoming_queue = incoming_queue;
		this->outcoming_queue = outcoming_queue;
	}

	void run() override;

private:
	int buffer_pos;
	unsigned char *buffer;
	bool receiving;
	MessageQueue<WRCP> *incoming_queue;
	MessageQueue<WRCP> *outcoming_queue;
	std::pair<int, int> *list[MAX_RECEIVED_PACKETS_BUFFER];
	MiniModem modem;

	void clearBuffer();
	void addToBuffer(unsigned char byte);
	void sendACK(WRCP packet);
	bool isInOurList(WRCP packet);
	void addToOurList(WRCP packet);

	std::pair<int, int> * getPair(WRCP &packet) const;

	void clearReceivedList();

	void moveList();
};


#endif //MODEM_WRCPRECEIVER_HPP
