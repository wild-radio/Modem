#ifndef MODEM_CONTROLRECORDACCESS_HPP
#define MODEM_CONTROLRECORDACCESS_HPP

#include <mutex>

class ControlRecordAccess {
public:
	static std::mutex record_mutex;
};


#endif //MODEM_CONTROLRECORDACCESS_HPP
