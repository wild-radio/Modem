#ifndef MODEM_WRCPCONTROLLER_HPP
#define MODEM_WRCPCONTROLLER_HPP


static const int MAIN_CAMERA_ID = 0;

static const int SECONDARY_CAMERA_ID = 1;

static const char *const main_photo_path = "/home/pi/.wildradio/pictures/main";

static const char *const secondary_photo_path = "/home/pi/.wildradio/pictures/secondary";

static const int DEFAULT_TIMEOUT = 8;

#include <thread>
#include "../MessageQueue/MessageQueue.hpp"
#include "WRCP.hpp"
#include "../Notification/Notification.hpp"
#include "../Notification/CameraConfigurations.hpp"
#include "../SSTV/Robot36Encoder.hpp"
#include "../SendPhotoToServer/SendPhotoToServer.hpp"
#include "../SSTV/Robot36Decoder.hpp"

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
	void startMasterNotifications();
	void startSlaveNotifications();
	void mainLoop();

private:
	int fd;
	int8_t id;
	int8_t id_with_transmission_rights;
	bool request_photo;
	bool other_ids[255];
	MessageQueue<WRCP> incoming_packets;
	MessageQueue<WRCP> outcoming_packets;
	MessageQueue<Notification> incoming_notifications;
	std::thread *main_monitor_thread;
	std::thread *secondary_monitor_thread;
	std::thread *receiver_thread;
	std::thread *transmitter_thread;
	std::thread *main_photo_monitor_thread;
	std::thread *secondary_photo_monitor_thread;
	Robot36Decoder sstv_decoder;
	SendPhotoToServer sender;
	Robot36Encoder sstv_encoder;

	CameraConfigurations main_config;
	CameraConfigurations secondary_config;

	bool isSlave();

	void processPacket(WRCP packet);

	WRCP getReturn(float timeout);

	bool handleACKAndNACK(WRCP &packet, int trys, int timeout);
	bool resendWhileNotAck(WRCP &r_packet, int r_timeout);

	void sendACK(WRCP packet);
	void sendNACK(WRCP packet);
	void sendInformPresence();
	bool sendRequestSendingRights();
	void sendPhoto(int32_t timestamp, int8_t camera_id, std::string photo_path = "");
	void sendAngleChange(int8_t receiver_id, int8_t angle_h, int8_t angle_v, int8_t camera_id);
	void sendCameraOptions(int8_t receiver_id, int8_t timer_for_capture, int8_t use_sensor, int8_t enable, int8_t camera_id);
	void sendRequestPhoto(int8_t receiver_id, int8_t camera_id);


	void handlePacket();
	void handleNotifications();
	void processMasterNotification(Notification notification);
	void handlePhotoReciever(WRCP &packet);
	void sendPhotoToServer(WRCP &packet);
	void convertPPMToPNG(std::string ppm_filename, std::string png_filename);
	void updateAngle(WRCP packet);
	CameraConfigurations *getConfigurations(int camera_id);

	void updateOptions(WRCP packet);

	void requestPhoto(WRCP packet);

	std::string getPhotoPath(WRCP &packet) const;
	std::string getPhotoPathFromTimestampAndCameraId(int32_t timestamp, int8_t camera_id);
	int getTimestamp() const;

	bool isEnoughWaitingTimeForTransmission(int last_recived_timestamp) const;
};


#endif //MODEM_WRCPCONTROLLER_HPP
