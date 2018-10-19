#ifndef MODEM_TESTMODEM_HPP
#define MODEM_TESTMODEM_HPP


#include <mutex>
#include "../ModemInterface.hpp"

class TestModem : public ModemInterface {
public:
	void writeData(const unsigned char *data, int size) override;
	int readData(unsigned char *data, int size) override;
	void setOptions(std::string options) override {};

	static void insertIntoBuffer(unsigned char *data, int size);
	static void txToRx();

private:
	static unsigned char rx_buffer[255];
	static int rx_buffer_cursor;
	static int rx_buffer_size;

	static unsigned char tx_buffer[255];
	static int tx_buffer_cursor;
	static int tx_buffer_size;

	static bool tx_to_rx;

	static std::mutex m_mutex;
};


#endif //MODEM_TESTMODEM_HPP
