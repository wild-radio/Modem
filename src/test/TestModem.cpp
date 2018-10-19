#include "TestModem.hpp"
#include <cstring>

unsigned char TestModem::rx_buffer[255];
int TestModem::rx_buffer_cursor = 0;
int TestModem::rx_buffer_size = 0;

unsigned char TestModem::tx_buffer[255];
int TestModem::tx_buffer_cursor = 0;
int TestModem::tx_buffer_size = 0;
bool TestModem::tx_to_rx = false;

std::mutex TestModem::m_mutex;

void TestModem::insertIntoBuffer(unsigned char *data, int size) {
	TestModem::m_mutex.lock();
	memcpy(TestModem::rx_buffer + TestModem::rx_buffer_cursor, data, (size_t)size);
	TestModem::rx_buffer_size += size;
	TestModem::m_mutex.unlock();
}

int TestModem::readData(unsigned char *data, int size) {
	bool bytes_to_read = false;
	while (!bytes_to_read) {
		TestModem::m_mutex.lock();
		if (TestModem::rx_buffer_size - (TestModem::rx_buffer_cursor -1) > size)
			bytes_to_read = true;
		TestModem::m_mutex.unlock();
	}

	TestModem::m_mutex.lock();
	memcpy(data, TestModem::rx_buffer + TestModem::rx_buffer_cursor, (size_t)size);
	TestModem::rx_buffer_cursor += size;
	TestModem::m_mutex.unlock();

	return 0;
}

void TestModem::writeData(const unsigned char *data, int size) {
	TestModem::m_mutex.lock();
	if (TestModem::tx_to_rx) {
		memcpy(TestModem::rx_buffer + TestModem::rx_buffer_cursor, data, (size_t)size);
		TestModem::rx_buffer_size += size;
		TestModem::m_mutex.unlock();
		return;
	}
	memcpy(TestModem::tx_buffer + TestModem::tx_buffer_cursor, data, (size_t)size);
	TestModem::tx_buffer_size += size;
	TestModem::m_mutex.unlock();
}

void TestModem::txToRx() {
	TestModem::tx_to_rx = true;
}
