//
// Created by clemente on 13/11/18.
//

#ifndef MODEM_INOTIFY_H
#define MODEM_INOTIFY_H

#include "../ThreadInterface.hpp"
#include "Notification.hpp"
#include "../MessageQueue/MessageQueue.hpp"

typedef struct camera_config
{

};


class Inotify : public ThreadInterface {
public:
	explicit Inotify(MessageQueue<Notification> *notifications_queue) {
		this->debouncing = NULL;
		this->notifications_queue = notifications_queue;
		//this->notifications_queue->post(Notification(NotificationType::REQUEST_CAPTURE, 1));
	}
	void run() override;

private:
	MessageQueue<Notification> *notifications_queue;
	int *debouncing;

	char *readFile(char *file_name);

	char *getSharedMemoryName(char *full_path);

	void storeConfig(char *full_path, camera_config new_config);

	camera_config getConfig(char *full_path);

	void diffConfigs(camera_config new_config, char *full_path);

	void debounceDiffConfigs(camera_config new_config, char *full_path);

	camera_config loadConfigFile(char *file_name);

	int evnt_mon(char *argv);

	int getCameraId(char *string);
};

#endif //MODEM_INOTIFY_H
