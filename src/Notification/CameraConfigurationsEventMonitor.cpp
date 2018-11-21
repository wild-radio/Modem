#include "CameraConfigurationsEventMonitor.hpp"
#include <sys/inotify.h>
#include <string.h>
#include <unistd.h>

#define EVENT_SIZE (sizeof(struct inotify_event))

CameraConfigurationsEventMonitor::CameraConfigurationsEventMonitor(std::string directory, std::string filename, MessageQueue<Notification> *notification_queue, int camera_id) {
	this->directory = directory;
	this->filename = filename;
	this->notification_queue = notification_queue;
	this->camera_id = camera_id;
	this->config = new CameraConfigurations;
	this->config->loadConfigurations(directory + "/" + filename);

	fd = inotify_init();
	if (fd < 0) {
		std::perror("Couldn't initialize Notification");
	}

	wd = inotify_add_watch(fd, directory.c_str(), IN_CREATE | IN_MODIFY);

	if (wd == -1) {
		std::perror(std::string("Couldn't add watch to " + directory).c_str());
	}
}

void CameraConfigurationsEventMonitor::run() {
	this->monitor();
}

void CameraConfigurationsEventMonitor::monitor() {
	int length;
	char buffer[(1024 * ((sizeof(struct inotify_event)) + 16))];
	while (1) {
		length = read(fd, buffer, (1024 * ((sizeof(struct inotify_event)) + 16)));

		if (length < 0) {
			std::perror("read");
		}

		processEvent(length, buffer);
	}
}

CameraConfigurationsEventMonitor::~CameraConfigurationsEventMonitor() {
	inotify_rm_watch(fd, wd);
	close(fd);
}

void CameraConfigurationsEventMonitor::processEvent(int length, const char *buffer) {
	int i = 0;
	int last_event_timestamp = 0;
	while (i < length) {
		inotify_event *event = (struct inotify_event *)&buffer[i];

		if (!event->len)
			break;

		i += EVENT_SIZE + event->len;

		if (this->getTimestamp() - last_event_timestamp < 2)
			continue;

		if (wasMyConfigModified(event)) {
			this->generateNotification();
			last_event_timestamp = this->getTimestamp();
		}
	}
}


int CameraConfigurationsEventMonitor::getTimestamp() const {
	time_t timestamp = time(nullptr);
	return (int)timestamp;
}

bool CameraConfigurationsEventMonitor::wasMyConfigModified(const inotify_event *event) const {
	return event->mask & IN_MODIFY && !(event->mask & IN_ISDIR) && filename == event->name;
}

void CameraConfigurationsEventMonitor::generateNotification() {
	sleep(1);
	auto new_config = new CameraConfigurations;
	new_config->loadConfigurations(directory + "/" + filename);

	if (wasCameraOptionsModified(new_config))
		notifyModifiedOptions(new_config);

	if (this->config->horizontal != new_config->horizontal || this->config->vertical != new_config->vertical)
		notifyAngleChanged(new_config);

	if (new_config->request_photo) {
		notifyPhotoRequested();
	}

	delete (this->config);
	this->config = new_config;
}

bool CameraConfigurationsEventMonitor::wasCameraOptionsModified(const CameraConfigurations *new_config) const {
	return config->timer != new_config->timer || config->sensor != new_config->sensor || config->active != new_config->active;
}

void CameraConfigurationsEventMonitor::notifyPhotoRequested() const {
	notification_queue->post(Notification(NotificationType::REQUEST_CAPTURE, camera_id));
}

void CameraConfigurationsEventMonitor::notifyAngleChanged(CameraConfigurations *new_config) const {
	notification_queue->post(Notification(NotificationType::NEW_ANGLE, new_config->horizontal, new_config->vertical, camera_id));
}

void CameraConfigurationsEventMonitor::notifyModifiedOptions(CameraConfigurations *new_config) {
	notification_queue->post(
			Notification(NotificationType::MODIFY_OPTIONS, (bool) new_config->timer, (bool) new_config->sensor, (bool)new_config->active,
			             camera_id));
}
