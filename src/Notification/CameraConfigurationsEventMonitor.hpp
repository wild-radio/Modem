#ifndef MODEM_EVENTMONITOR_HPP
#define MODEM_EVENTMONITOR_HPP


#include "CameraConfigurations.hpp"
#include "../ThreadInterface.hpp"
#include "Notification.hpp"
#include "../MessageQueue/MessageQueue.hpp"

#include <string>
#include <iostream>
#include <sys/inotify.h>

class CameraConfigurationsEventMonitor : public ThreadInterface {
public:
	CameraConfigurationsEventMonitor(std::string directory, std::string filename, MessageQueue<Notification> *notification_queue, int camera_id);
	~CameraConfigurationsEventMonitor();
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
	int processEvent(int length, const char *buffer, int last_event_timestamp);
	bool wasMyConfigModified(const inotify_event *event) const;
	void generateNotification();

	void notifyCamera(const CameraConfigurations *new_config) const;
	void notifyPhotoRequested() const;
	void notifyAngleChanged(CameraConfigurations *new_config) const;
	void notifyModifiedOptions(CameraConfigurations *new_config);

	bool wasCameraOptionsModified(const CameraConfigurations *new_config) const;

	int getTimestamp() const;

	int executeEventIfItIsNotTooSoon(int last_event_timestamp);
};


#endif //MODEM_EVENTMONITOR_HPP
