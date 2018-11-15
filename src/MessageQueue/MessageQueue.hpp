#ifndef MODEM_MESSAGEQUEUE_HPP
#define MODEM_MESSAGEQUEUE_HPP


#include <mutex>
#include <queue>

template <class T>
class MessageQueue {
public:
	bool hasMessage();
	void post(T msg);
	T pull();

private:
	std::mutex m_mutex;
	std::queue<T> m_queue;
};

template <class T>
T MessageQueue<T>::pull() {
	this->m_mutex.lock();

	T msg = this->m_queue.front();
	this->m_queue.pop();

	this->m_mutex.unlock();

	return msg;
}

template<class T>
void MessageQueue<T>::post(T msg) {
	this->m_mutex.lock();
	this->m_queue.push(msg);
	this->m_mutex.unlock();
}

template<class T>
bool MessageQueue<T>::hasMessage() {
	this->m_mutex.lock();
	bool has_message = !this->m_queue.empty();
	this->m_mutex.unlock();

	return has_message;
}

#endif //MODEM_MESSAGEQUEUE_HPP
