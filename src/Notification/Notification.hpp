#ifndef MODEM_NOTIFICATION_HPP
#define MODEM_NOTIFICATION_HPP

enum NotificationType{
	NEW_ANGLE,
	ACTIVATE,
	INACTIVATE,
	MODIFY_OPTIONS,
	REQUEST_CAPTURE
};

class Notification {
public:
	NotificationType type;
	int angle_h;
	int angle_v;
	bool timer;
	bool sensor;
	int camera_id;

	Notification(NotificationType type, int angle_h, int angle_v, int camera_id) : type(type), angle_h(angle_h), angle_v(angle_v), camera_id(camera_id){}
	explicit Notification(NotificationType type, int camera_id) : type(type), camera_id(camera_id){}
	Notification(NotificationType type, bool timer, bool sensor, int camera_id) : type(type), timer(timer),
	                                                                              sensor(sensor),
	                                                                              camera_id(camera_id) {}
};


#endif //MODEM_NOTIFICATION_HPP
