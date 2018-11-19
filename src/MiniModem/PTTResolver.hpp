#ifndef DECODER36_PTTRESOLVER_HPP
#define DECODER36_PTTRESOLVER_HPP


#include "PTT.hpp"

class PTTResolver {
public:
	static PTT* resolve();
private:
	static PTT ptt;
};


#endif //DECODER36_PTTRESOLVER_HPP
