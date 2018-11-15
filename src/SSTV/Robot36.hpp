#ifndef MODEM_ROBOT36_HPP
#define MODEM_ROBOT36_HPP


#include "Robot36Decoder.hpp"
#include "Robot36Encoder.hpp"

class Robot36 {
public:
	void asyncDecode(std::string filename);
	void encode(std::string filename);
private:
	Robot36Decoder decoder;
	Robot36Encoder encoder;
};


#endif //MODEM_ROBOT36_HPP
