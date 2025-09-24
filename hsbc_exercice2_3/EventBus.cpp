#include <cassert>
#include "EventBus.h"

namespace EventBus3
{
	unsigned long long int StampedEvent::_globalCount = 0;

	void EventBus::stop()
	{
		_stop = true;
		_cvq.notify_all();
	}

	void EventBus::start()
	{
		_stop = false;
	}

	void EventBus::publishEvent(Event ev)
	{
		std::unique_lock<std::mutex> lock(_mQueue);
		_queue.push(StampedEvent(ev));// auto increment counter in StampedEvent
		_eventMap[_queue.back().getEvent().getEventType()] = _queue.back().getcount();
		_cvq.notify_all();
	}

	std::vector<Event> EventBus::ExtractLatest(std::queue<StampedEvent> & qe, const std::unordered_map<Event::EventType, unsigned long long int > & eventMap)
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
		Event anEvent;
		while (!_stop)
		{
			std::queue<StampedEvent> qe;
			std::unordered_map<Event::EventType, unsigned long long int > eventMap;
			{
				std::unique_lock<std::mutex> lk(_mQueue);
				_cvq.wait(lk, [&]() {return !_queue.empty() || _stop; });
				if (_stop)
				{
					return;
				}

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
	}

	// How would you denote the subscriber?
	void EventBus::addSubscriber(std::shared_ptr<Subscriber> subscriber, const Event::EventType& evType)
	{
		int ret = _subscribersPredicateMap.insertToSubscriberMap(subscriber, evType, nullptr);
		//log here filter modified or insert failed
	}

	// Would you allow clients to filter the events they receive? How would the interface look like?
	void EventBus::addSubscriberForFilteredEvents(std::shared_ptr<Subscriber> subscriber, const Event::EventType& evType, Predicate filter)
	{
		int ret = _subscribersPredicateMap.insertToSubscriberMap(subscriber, evType, filter);
		//log here filter modified
	}


}

