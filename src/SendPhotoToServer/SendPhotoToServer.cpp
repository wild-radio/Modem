//
// Created by clemente on 13/11/18.
//

#include "SendPhotoToServer.hpp"
#include <curl/curl.h>
#include "sstream"
#include <string>
#include <iostream>
#include <fstream>
#include "../Base64/base64.hpp"

void SendPhotoToServer::sendPhoto(std::string filename, int id, int cam_id, int timestamp, std::string suffix) {
	CURL *curl;
	CURLcode res;
	
	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();
	if(! curl) {
		curl_global_cleanup();
		return;
	}

	std::stringstream file_contents = FileUtils::getFileContents(filename);

	std::stringstream path;
	std::stringstream body;
	std::string camera_type = (cam_id == 0) ? "principal" : "alternativa";
	path << POST_URL << "cameras/integracao/WR000" << id << "/" << camera_type << "/" << suffix;
	auto string_contents = file_contents.str();
	body << "{\"dataHoraCaptura\": " << timestamp << ", \"conteudo\": \"" << base64_encode((const unsigned char*)string_contents.c_str(), string_contents.size()) <<"\"}";

	CURL *hnd = curl_easy_init();

	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(hnd, CURLOPT_URL, path.str().c_str());

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
	auto body_as_string = body.str();
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, body_as_string.c_str());

	CURLcode ret = curl_easy_perform(hnd);
}

