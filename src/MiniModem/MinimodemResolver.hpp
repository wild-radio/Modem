//
// Created by clemente on 22/11/18.
//

#ifndef DECODER36_MINIMODEMRESOLVER_HPP
#define DECODER36_MINIMODEMRESOLVER_HPP


#include "MiniModem.hpp"

class MinimodemResolver {
public:
	static MiniModem *resolve();
private:
	static MiniModem modem;
};


#endif //DECODER36_MINIMODEMRESOLVER_HPP
