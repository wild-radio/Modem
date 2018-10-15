#ifndef MODEM_WRCP_HPP
#define MODEM_WRCP_HPP

#include <cstdint>

struct _wrcp_packet {
	char protocol_identifier[4];
	int8_t sender_id;
	int8_t receiver_id;
	int8_t action;
	int8_t message_number;
	int16_t checksum;
	char data[5];
};

class WRCP {

};


#endif //MODEM_WRCP_HPP
