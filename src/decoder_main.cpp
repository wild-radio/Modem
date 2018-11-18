#include <iostream>
#include <string>
#include "WRCP/WRCPController.hpp"
#include "SSTV/Robot36Encoder.hpp"
#include "SSTV/Robot36Decoder.hpp"

void convertPPMToPNG(std::string ppm_filename, std::string png_filename);

int main(int argc, char **argv) {
	int timestamp = std::stoi(argv[1]);
	int camera_id = std::stoi(argv[2]);
	std::string path = argv[3];

	std::stringstream photo_filename;
	photo_filename << timestamp << ".ppm";

	Robot36Decoder decoder;
	decoder.decoder(photo_filename.str());

	std::stringstream png_filename;
	png_filename << timestamp << ".png";
	convertPPMToPNG(photo_filename.str(), png_filename.str());

	SendPhotoToServer sender;
	sender.sendPhoto(png_filename.str(), 1, camera_id, timestamp, path);
}

void convertPPMToPNG(std::string ppm_filename, std::string png_filename) {
	std::system(std::string(std::string("convert ") + ppm_filename + std::string(" ") + png_filename).c_str());
}