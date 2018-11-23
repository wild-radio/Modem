#ifndef MODEM_WRCPRECEIVER_HPP
#define MODEM_WRCPRECEIVER_HPP

static const int MAX_RECEIVED_PACKETS_BUFFER = 4;

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
		//list_pointer = 0;
	}

	void run() override;

private:
	int buffer_pos;
	unsigned char *buffer;
	bool receiving;
	MessageQueue<WRCP> *incoming_queue;
	MessageQueue<WRCP> *outcoming_queue;
	std::vector<int> list_message_numbers;
	int last_message_number;

	void clearBuffer();
	void addToBuffer(unsigned char byte);
	void sendACK(WRCP packet);
	bool isInOurList(int message_number);
	void addToOurList(int message_number);
};


#endif //MODEM_WRCPRECEIVER_HPP
