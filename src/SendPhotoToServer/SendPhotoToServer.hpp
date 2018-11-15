#ifndef MODEM_REST_HPP
#define MODEM_REST_HPP

static const char *const POST_URL = "http://127.0.0.1:8080/";

#include<string>
#include "../Utils/FileUtils.hpp"

class SendPhotoToServer : public FileUtils {
public:

	void sendPhoto(std::string filename, int id, int cam_id, int timestamp, std::string suffix = "fotos");

};


#endif //MODEM_REST_HPP
