#include <fstream>
#include <iostream>
#include <cstdio>
#include "sstream"
#include "../SendPhotoToServer/SendPhotoToServer.hpp"
#include "FileUtils.hpp"

std::stringstream FileUtils::getFileContents(const std::string &filename) {
	std::ifstream file (filename, std::ios_base::binary);
	checkFileOpened(filename, file);

	auto file_contents = readFile(file);

	file.close();
	return file_contents;
}

std::vector<std::string> FileUtils::getFileLines(const std::string &filename) {
	std::ifstream file (filename);
	checkFileOpened(filename, file);

	auto file_lines = readFileLines(file);

	file.close();
	return file_lines;
}

void FileUtils::checkFileOpened(const std::string &filename, std::ifstream &file) {
	if (!file.is_open()) {
		perror(std::__cxx11::string("Error opening the file " + filename).c_str());
	}
}

std::stringstream FileUtils::readFile(std::ifstream &file) {
	std::stringstream file_contents;
	char buffer;

	while (file.get(buffer)) {
		file_contents << buffer;
	}

	return file_contents;
}

std::vector<std::string> FileUtils::readFileLines(std::ifstream &file) {
	std::vector<std::string> lines;
	std::string line;

	while (std::getline(file, line)) {
		lines.push_back(line);
	}

	return lines;
}

void FileUtils::saveFileContents(std::string filename, std::string contents) {
	std::ofstream file;
	file.open(filename);
	file << contents;
	file.close();
}
