#ifndef MODEM_FILEMODEM_HPP
#define MODEM_FILEMODEM_HPP


#include "../ModemInterface.hpp"
#include <sys/inotify.h>

class FileModem : public ModemInterface {
public:
	void writeData(const unsigned char *data, int size) override;
	int readData(unsigned char *data, int size) override;
	void setOptions(std::string options) override {};

	static void setOutFile(std::string out_file) {FileModem::out_file = out_file; }
	static void setInFile(std::string in_file) {FileModem::out_file = in_file; }
	static void prepareNotify();
private:
	static std::string out_file;
	static std::string in_file;
	static int fd;

	static unsigned char rx_buffer[255];
	static int rx_buffer_cursor;
	static int rx_buffer_size;
};


#endif //MODEM_FILEMODEM_HPP
