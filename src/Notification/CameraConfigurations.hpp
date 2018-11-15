#ifndef MODEM_CAMERACONFIGURATIONS_HPP
#define MODEM_CAMERACONFIGURATIONS_HPP

#include <string>

class CameraConfigurations {
public:
	int active;
	int timer;
	int sensor;
	int horizontal;
	int vertical;
	int request_photo;

	void loadConfigurations(std::string filename);
private:
};


#endif //MODEM_CAMERACONFIGURATIONS_HPP
