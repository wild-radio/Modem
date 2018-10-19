#include <zconf.h>
#include <cstring>
#include <iostream>
#include <sys/poll.h>
#include "FileModem.hpp"

std::string FileModem::out_file;
std::string FileModem::in_file;
int FileModem::fd = 0;
int FileModem::wd = 0;

unsigned char FileModem::rx_buffer[255];
int FileModem::rx_buffer_cursor = 0;
int FileModem::rx_buffer_size= 0 ;

void FileModem::writeData(const unsigned char *data, int size) {
	FILE *file = fopen(FileModem::out_file.c_str(), "ab");
	fwrite(data, sizeof(unsigned char), (size_t) size, file);
	fclose(file);
}

int FileModem::readData(unsigned char *data, int size) {
	while (FileModem::rx_buffer_size - (FileModem::rx_buffer_cursor -1) <= size) {
		//waitEvent();

//		FileModem::rx_buffer_cursor = 0;
//		FileModem::rx_buffer_size = 0;

		FILE *file = fopen(FileModem::in_file.c_str(), "rb");

		int read_size = 0;

		fseek(file, FileModem::rx_buffer_size, SEEK_SET);

		while ((read_size = fread(FileModem::rx_buffer + FileModem::rx_buffer_size, sizeof(unsigned char), 1, file))) {
			FileModem::rx_buffer_size += read_size;
		}
		fclose(file);
	}

	memcpy(data, FileModem::rx_buffer + FileModem::rx_buffer_cursor, (size_t)size);
	FileModem::rx_buffer_cursor += size;

	return size;
}

void FileModem::waitEvent() {
	int poll_n;
	struct pollfd fds;
	fds.fd = FileModem::fd;
	fds.events = POLLIN;
	while (1) {
			poll_n = poll(&fds, 1, -1);
			if (poll_n > 0) {
				if (fds.revents & POLLIN) {
					return handle_events(FileModem::fd);
				}
			}
		}
}

void FileModem::prepareNotify() {
	FileModem::fd = inotify_init();
	FileModem::wd = inotify_add_watch(FileModem::fd, FileModem::in_file.c_str(), IN_MODIFY);
}

void FileModem::handle_events(int fd) {
	/* Some systems cannot read integer variables if they are not
	 properly aligned. On other systems, incorrect alignment may
	 decrease performance. Hence, the buffer used for reading from
	 the inotify file descriptor should have the same alignment as
	 struct inotify_event. */

	char buf[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));
	const struct inotify_event *event;
	int i;
	ssize_t len;
	char *ptr;

	/* Loop while events can be read from inotify file descriptor. */

	for (;;) {

		/* Read some events. */

		len = read(fd, buf, sizeof buf);
		if (len == -1 && errno != EAGAIN) {
			perror("read");
			exit(EXIT_FAILURE);
		}

		/* If the nonblocking read() found no events to read, then
		 it returns -1 with errno set to EAGAIN. In that case,
		 we exit the loop. */

		if (len <= 0)
			break;

		/* Loop over all events in the buffer */

		for (ptr = buf; ptr < buf + len;
		     ptr += sizeof(struct inotify_event) + event->len) {

			event = (const struct inotify_event *) ptr;

			/* Print event type */

			if (event->mask & IN_OPEN)
				printf("IN_OPEN: ");
			if (event->mask & IN_CLOSE_NOWRITE)
				printf("IN_CLOSE_NOWRITE: ");
			if (event->mask & IN_CLOSE_WRITE)
				printf("IN_CLOSE_WRITE: ");
			if (event->mask & IN_MODIFY)
				return;

			/* Print the name of the watched directory */


			/* Print the name of the file */

			if (event->len)
				printf("%s", event->name);

			/* Print type of filesystem object */

			if (event->mask & IN_ISDIR)
				printf(" [directory]\n");
			else
				printf(" [file]\n");
		}
	}
}
