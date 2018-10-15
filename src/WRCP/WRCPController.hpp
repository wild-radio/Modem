#ifndef MODEM_WRCPCONTROLLER_HPP
#define MODEM_WRCPCONTROLLER_HPP


#include <thread>

class WRCPController {
public:
	void startReceiver();
	void startTransmitter();
	void mainLoop();

private:
	std::thread *receiver_thread;
	std::thread *transmitter_thread;
};


#endif //MODEM_WRCPCONTROLLER_HPP
