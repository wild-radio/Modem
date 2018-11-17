#ifndef MODEM_PTT_HPP
#define MODEM_PTT_HPP

#include <wiringPi.h>


class PTT {
public:
	PTT();
	~PTT();
	void push();
	void release();
private:
	const int GPIO_PIN = 15;
};


#endif //MODEM_PTT_HPP
