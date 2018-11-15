//
// Created by clemente on 15/11/18.
//

#include <sstream>
#include "CameraConfigurations.hpp"
#include "../Utils/FileUtils.hpp"

void CameraConfigurations::loadConfigurations(std::string filename) {
	auto file_lines = FileUtils::getFileLines(filename);

	this->active = std::stoi(file_lines[0]);
	this->timer = std::stoi(file_lines[1]);
	this->sensor = std::stoi(file_lines[2]);
	this->horizontal = std::stoi(file_lines[3]);
	this->vertical = std::stoi(file_lines[4]);
	this->request_photo = std::stoi(file_lines[5]);
}
