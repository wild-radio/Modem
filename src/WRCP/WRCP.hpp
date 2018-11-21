#ifndef MODEM_WRCP_HPP
#define MODEM_WRCP_HPP

static const int WRCP_PACKET_SIZE = 15;
static const int WRCP_ACTIVATE_SENSOR = 0x00;
static const int WRCP_DEACTIVATE_SENSOR = 0x01;

#include <cstdint>
#include <mutex>

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

public:
	explicit WRCP(unsigned char *buffer);
	WRCP(){};
	~WRCP();

	unsigned char *getData();

	void print();

	bool isValidChecksum();
	bool isForMe(int8_t id);

	bool isACK() { return  packet.action == this->getACKAction(); }
	bool isNACK() { return  packet.action == this->getNACKAction(); }
	bool isInformPresenceAction() { return packet.action == this->getInformPresenceAction(); }
	bool isRequestSendingRights() { return packet.action == this->getRequestSendingRightsAction(); }
	bool isPhoto() { return packet.action == this->getPhotoAction(); }
	bool isAngleChange() { return packet.action == this->getAngleChangeAction(); }
	bool isCameraOptions() {return packet.action == this->getCameraOptionsAction(); }
	bool isRequestPhoto() { return packet.action == this->getRequestPhotoAction(); }

	void createNACK(WRCP packet_to_respond, int8_t sender_id);
	void createACK(WRCP packet_to_respond);
	void createSlaveInformPresence(int8_t sender_id);
	void createRequestSendingRights(int8_t sender_id);
	void createPhoto(int8_t sender_id, int32_t timestamp, int8_t camera_id);

	int8_t getSender() { return this->packet.sender_id; }
	int8_t getCameraId();
	int32_t getTimestamp();
	void createAngleChange(int8_t receiver_id, int8_t angle_h, int8_t angle_v, int8_t camera_id);
	int8_t getVerticalAngle();
	int8_t getHorizontalAngle();
	void createCameraOptions(int8_t receiver_id, int8_t timer_for_capture, int8_t use_sensor, int8_t camera_id);
	int8_t getUseSensor();
	int8_t getTimerForCapture();
	const int8_t getMessageNumber();
	void createRequestPhoto(int8_t receiver_id, int8_t camera_id);
	_wrcp_packet getPacket();

	bool operator==(WRCP& packet)const;

private:
	static int8_t packet_number;
	static std::mutex m_number;
	_wrcp_packet packet;

	void addProtocolIdentifier();

	int8_t getPacketNumber();
	int8_t getInformPresenceAction();

	int16_t calculateChecksum();
	void getPartsForChecksum(int16_t *parts);

	int8_t getACKAction();
	int8_t getNACKAction();
	int8_t getRequestSendingRightsAction();
	int8_t getPhotoAction();

	int8_t getAngleChangeAction();
	int8_t getCameraOptionsAction();
	int8_t getRequestPhotoAction();

	void clearData();
};


#endif //MODEM_WRCP_HPP
