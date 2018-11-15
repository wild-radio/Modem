//
// Created by clemente on 15/11/18.
//

#ifndef MODEM_FILEUTILS_HPP
#define MODEM_FILEUTILS_HPP


#include<string>
#include <vector>

class FileUtils {
public:
	static std::stringstream getFileContents(const std::__cxx11::string &filename);
	static std::vector<std::string> getFileLines(const std::string &filename);
private:
	static std::stringstream readFile(std::ifstream &file);
	static std::vector<std::string> readFileLines(std::ifstream &file);

	static void checkFileOpened(const std::string &filename, std::ifstream &file);
};


#endif //MODEM_FILEUTILS_HPP
