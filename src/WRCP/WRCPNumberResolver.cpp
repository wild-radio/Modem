//
// Created by clemente on 21/11/18.
//

#include "WRCPNumberResolver.hpp"

WRCPNumber WRCPNumberResolver::number;

WRCPNumber * WRCPNumberResolver::resolve() {
	return &number;
}
