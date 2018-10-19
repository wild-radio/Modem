#include <zconf.h>
#include <cstring>
#include <iostream>
#include "FileModem.hpp"

#define EVENT_SIZE  ( sizeof (struct inotify_event) ) /*size of one event*/
#define BUF_LEN     ( 1 * ( EVENT_SIZE + 7 ))

std::string FileModem::out_file;
std::string FileModem::in_file;
int FileModem::fd = 0;

unsigned char FileModem::rx_buffer[255];
int FileModem::rx_buffer_cursor = 0;
int FileModem::rx_buffer_size= 0 ;

void FileModem::writeData(const unsigned char *data, int size) {
	FILE *file = fopen(FileModem::out_file.c_str(), "wb");
	fwrite(data, sizeof(unsigned char), (size_t)size, file);
	fclose(file);
}

int FileModem::readData(unsigned char *data, int size) {
	char buffer[BUF_LEN];

	if (FileModem::rx_buffer_size - (FileModem::rx_buffer_cursor -1) <= size) {
		read(FileModem::fd, buffer, BUF_LEN);
		FILE *file = fopen(FileModem::in_file.c_str(), "rb");
		std::cout << "eie" << std::endl;

		while (!fread(FileModem::rx_buffer, sizeof(unsigned char), 1, file)) {
			FileModem::rx_buffer_size += size;
		}
		fclose(file);
	}

	memcpy(data, FileModem::rx_buffer + FileModem::rx_buffer_cursor, (size_t)size);
	FileModem::rx_buffer_cursor += size;

	return 0;
}

void FileModem::prepareNotify() {
	FileModem::fd = inotify_init();
	inotify_add_watch(FileModem::fd, FileModem::in_file.c_str(), IN_MODIFY);
}

