#include <cassert>
#include "EventBus.h"

namespace bus_multithread_enhanced
{
	unsigned long long int StampedEvent::_globalCount = 0;

	void EventBus::stop()
	{
		std::lock_guard<std::mutex> g(_startstop);
		if (_ConsumeTerminated == false)
		{
			_needStop = true;
			_cvq.notify_all();
		}
	}

	void EventBus::start()
	{
		std::lock_guard<std::mutex> g(_startstop);
		if (_ConsumeTerminated == true && _needStop == false)
		{
			if (_consumerThread.get() == nullptr)
			{
				_consumerThread = std::make_unique<std::thread>(&EventBus::consume, this);
				_ConsumeTerminated = false;
			}
		}
	}

	void EventBus::publishEvent(Event ev)
	{
		std::unique_lock<std::mutex> lock(_mQueue);
		_queue.push(StampedEvent(ev));// auto increment counter in StampedEvent
		_eventMap[_queue.back().getEvent().getEventType()] = _queue.back().getcount();
		_cvq.notify_all();
	}

	std::vector<Event> EventBus::ExtractLatest(std::queue<StampedEvent> & qe, const std::unordered_map<std::string, unsigned long long int > & eventMap)
	{
		//the unordered map keep the last number of count
		std::vector<Event> retq;
		while (!qe.empty())
		{
			StampedEvent e = qe.front();
			qe.pop();
			auto it = eventMap.find(e.getEvent().getEventType());
			// if it is the last push it
			if (it != eventMap.end() && it->second == e.getcount())
			{
				retq.push_back(e.getEvent());
			}
		}
		return retq;
	}

	void EventBus::consume()
	{
		while (!_needStop)
		{
			std::queue<StampedEvent> qe;
			std::unordered_map<std::string, unsigned long long int > eventMap;
			{
				std::unique_lock<std::mutex> lk(_mQueue);
				_cvq.wait(lk, [&]() {return !_queue.empty() || _needStop; });
				if (_needStop)
				{
					_ConsumeTerminated = true;
					_needStop = false;
					return;
				}
				// we copy the queue and the map to limit the time we hold the lock
				qe.swap(_queue);
				eventMap.swap(_eventMap);
			}

			auto events = ExtractLatest(qe, eventMap);
			for (auto & anEvent : events)
			{
				std::vector<std::shared_ptr<Subscriber>> subscribers = _subscribersPredicateMap.getSubscribersFromEvent(anEvent);
				for (auto it = subscribers.begin(), itEnd = subscribers.end(); it != itEnd; ++it)
				{
					(*it)->consume(anEvent);
				}
			}
		}
		_ConsumeTerminated = true;
		_needStop = false;
	}

	// How would you denote the subscriber?
	void EventBus::addSubscriber(std::shared_ptr<Subscriber> subscriber, const std::string& evType)
	{
		int ret = _subscribersPredicateMap.insertToSubscriberMap(subscriber, evType, nullptr);
		//log here filter modified or insert failed
	}

	// Would you allow clients to filter the events they receive? How would the interface look like?
	void EventBus::addSubscriberForFilteredEvents(std::shared_ptr<Subscriber> subscriber, const std::string& evType, Predicate filter)
	{
		int ret = _subscribersPredicateMap.insertToSubscriberMap(subscriber, evType, filter);
		//log here filter modified
	}
}// namespace bus_multithread_enhanced

