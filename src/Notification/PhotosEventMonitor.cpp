#include "PhotosEventMonitor.hpp"

#include <unistd.h>
#include <iostream>

#define EVENT_SIZE (sizeof(struct inotify_event))

PhotosEventMonitor::PhotosEventMonitor(std::string directory,  MessageQueue<Notification> *notification_queue, int camera_id) {
	this->directory = directory;
	this->notification_queue = notification_queue;
	this->camera_id = camera_id;

	fd = inotify_init();
	if (fd < 0) {
		std::perror("Couldn't initialize Notification");
	}

	wd = inotify_add_watch(fd, directory.c_str(), IN_CREATE | IN_MODIFY);

	if (wd == -1) {
		std::perror(std::string("Couldn't add watch to " + directory).c_str());
	}
}

PhotosEventMonitor::~PhotosEventMonitor() {
	inotify_rm_watch(fd, wd);
	close(fd);
}

void PhotosEventMonitor::run() {
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

void PhotosEventMonitor::processEvent(int length, char *buffer) {
	int i = 0;
	while (i < length) {
		inotify_event *event = (struct inotify_event *)&buffer[i];

		if (!event->len)
			break;

		if (wasAPhotoCreated(event))
			this->generateNotification(event);

		i += EVENT_SIZE + event->len;
	}
}

bool PhotosEventMonitor::wasAPhotoCreated(inotify_event *event) {
	return event->mask & IN_CREATE && !(event->mask & IN_ISDIR);
}

void PhotosEventMonitor::generateNotification(inotify_event *event) {
	std::string filename = event->name;
	auto dot = filename.find_last_of(".");
	auto timestamp_str = filename.substr(0, dot);
	auto timestamp = std::stoi(timestamp_str);
	notification_queue->post(Notification(NotificationType::NEW_PHOTO, timestamp, camera_id));
}
