#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template<class T> 
class SharedQueue  
{
	std::queue<T> _queue;
	std::mutex _mEvent;//used for exclusif push and pop Event
	std::condition_variable _cvq;
public:

	void safePush(T ev)
	{
		std::unique_lock<std::mutex> lock(_mEvent);
		_queue.push(ev);
		_cvq.notify_one();
	}

	T safePop()
	{
		std::unique_lock<std::mutex> lk(_mEvent);
		assert(!_queue.empty());
		T elem = _queue.front();
		_queue.pop();
		return elem;
	}

};


