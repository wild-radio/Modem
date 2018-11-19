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
	std::lock_guard<std::mutex> lock(m_mutex);

	T msg = this->m_queue.front();
	this->m_queue.pop();

	return msg;
}

template<class T>
void MessageQueue<T>::post(T msg) {
	std::lock_guard<std::mutex> lock(m_mutex);
	this->m_queue.push(msg);
}

template<class T>
bool MessageQueue<T>::hasMessage() {
	std::lock_guard<std::mutex> lock(m_mutex);
	bool has_message = !this->m_queue.empty();

	return has_message;
}

#endif //MODEM_MESSAGEQUEUE_HPP
