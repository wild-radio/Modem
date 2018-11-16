#ifndef MODEM_PHOTOSEVENTMONITOR_HPP
#define MODEM_PHOTOSEVENTMONITOR_HPP


#include "../ThreadInterface.hpp"
#include "Notification.hpp"
#include "../MessageQueue/MessageQueue.hpp"
#include <sys/inotify.h>

class PhotosEventMonitor : public ThreadInterface {
public:
	PhotosEventMonitor(std::string directory, MessageQueue<Notification> *notification_queue, int camera_id);
	~PhotosEventMonitor();
	void run() override;
private:
	MessageQueue<Notification> *notification_queue;
	int wd;
	int fd;
	int camera_id;
	std::string directory;

	void processEvent(int length, char buffer[40960]);

	bool wasAPhotoCreated(inotify_event *event);

	void generateNotification(inotify_event *pEvent);
};


#endif //MODEM_PHOTOSEVENTMONITOR_HPP
