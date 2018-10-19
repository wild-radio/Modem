#ifndef MODEM_WRCPCONTROLLER_HPP
#define MODEM_WRCPCONTROLLER_HPP


#include <thread>
#include "../MessageQueue/MessageQueue.hpp"
#include "WRCP.hpp"

class WRCPController {
public:
	explicit WRCPController(int8_t id) {
		this->id = id;
		this->id_with_transmission_rights = 0;
		this->request_photo = false;

		for (int i = 0; i < 255; i++) {
			this->other_ids[i] = false;
		}
	}

	void startReceiver();
	void startTransmitter();
	void mainLoop();

private:
	int8_t id;
	int8_t id_with_transmission_rights;
	bool request_photo;
	bool other_ids[255];
	MessageQueue<WRCP> incoming_packets;
	MessageQueue<WRCP> outcoming_packets;
	std::thread *receiver_thread;
	std::thread *transmitter_thread;

	bool isSlave();

	void processPacket(WRCP packet);

	WRCP getReturn(float timeout);

	bool handleACKAndNACK(const WRCP &packet, int trys, int timeout);
	bool resendWhileNotAck(WRCP &r_packet, int r_timeout);

	void sendACK(WRCP packet);
	void sendNACK(WRCP packet);
	void sendInformPresence();
	void sendRequestSendingRights();
	void sendPhoto(int32_t timestamp, int8_t camera_id);
	void sendAngleChange(int8_t receiver_id, int8_t angle_h, int8_t angle_v, int8_t camera_id);

	void sendCameraOptions(int8_t receiver_id, int8_t timer_for_capture, int8_t use_sensor, int8_t camera_id);

	void sendRequestPhoto(int8_t receiver_id, int8_t camera_id);
};


#endif //MODEM_WRCPCONTROLLER_HPP
