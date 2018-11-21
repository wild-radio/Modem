//
// Created by clemente on 21/11/18.
//

#ifndef DECODER36_WRCPNUMBERRESOLVER_HPP
#define DECODER36_WRCPNUMBERRESOLVER_HPP


#include "WRCPNumber.hpp"

class WRCPNumberResolver {
public:
	static WRCPNumber * resolve();
private:
	static WRCPNumber number;
};


#endif //DECODER36_WRCPNUMBERRESOLVER_HPP
