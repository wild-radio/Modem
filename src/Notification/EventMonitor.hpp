#ifndef MODEM_EVENTMONITOR_HPP
#define MODEM_EVENTMONITOR_HPP


#include "CameraConfigurations.hpp"
#include "../ThreadInterface.hpp"
#include "Notification.hpp"
#include "../MessageQueue/MessageQueue.hpp"

#include <string>
#include <iostream>
#include <sys/inotify.h>

class EventMonitor : public ThreadInterface {
public:
	EventMonitor(std::string directory, std::string filename, MessageQueue<Notification> *notification_queue, int camera_id);
	~EventMonitor();
	void run() override;

private:
	MessageQueue<Notification> *notification_queue;
	CameraConfigurations *config;
	int wd;
	int fd;
	int camera_id;
	std::string directory;
	std::string filename;

	void monitor();
	void processEvent(int length, const char *buffer);
	bool wasMyConfigModified(const inotify_event *event) const;
	void generateNotification();

	void notifyCamera(const CameraConfigurations *new_config) const;
	void notifyPhotoRequested() const;
	void notifyAngleChanged(CameraConfigurations *new_config) const;
	void notifyModifiedOptions(CameraConfigurations *new_config);
};


#endif //MODEM_EVENTMONITOR_HPP
