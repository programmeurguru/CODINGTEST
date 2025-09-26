#include <cassert>
#include "EventBus.h"

namespace bus_multithread
{
	void EventBus::stop()
	{
		_stop = true;
		_cvq.notify_all();
	}
	void EventBus::publishEvent(const Event & ev)
	{
		std::unique_lock<std::mutex> lock(_mQueue);
		_queue.push(ev);
		_cvq.notify_all();
	}

	void EventBus::start()
	{
		_stop = false;
	}

	void EventBus::consume()
	{
		Event anEvent;
		while (!_stop)
		{
			{
				std::unique_lock<std::mutex> lk(_mQueue);
				_cvq.wait(lk, [&]() {return !_queue.empty() || _stop; });
				if (_stop)
				{
					return;
				}
				anEvent = _queue.front();
				_queue.pop();
			}
			// the two _subscribersMap and _subscribersPredicateMap can be merged in _subscribersPredicateMap ( use filter = nullptr)
			// As the  _subscribersMap is faster to generate the subscriber list, I keep the two maps
			std::vector<std::shared_ptr<Subscriber>> subscribers = _subscribersMap.getSubscribersFromEvent(anEvent);
			for (auto it = subscribers.begin(), itEnd = subscribers.end(); it != itEnd; ++it)
			{
				(* it)->consume(anEvent);
			}

			std::vector<std::shared_ptr<Subscriber>> subscriberPreds = _subscribersPredicateMap.getSubscribersFromEvent(anEvent);
			for (auto it = subscriberPreds.begin(), itEnd = subscriberPreds.end(); it != itEnd; ++it)
			{
				(*it)->consume(anEvent);
			}
		}
	}

	// How would you denote the subscriber?
	void EventBus::addSubscriber(const std::shared_ptr<Subscriber> & subscriber, const Event::EventType& evType)
	{
		int ret = _subscribersMap.insertToSubscriberMap(subscriber, evType);
		//log here filter modified or insert failed
	}

	// Would you allow clients to filter the events they receive? How would the interface look like?
	void EventBus::addSubscriberForFilteredEvents(const std::shared_ptr<Subscriber> & subscriber, const Event::EventType& evType, Predicate filter)
	{
		int ret = _subscribersPredicateMap.insertToSubscriberMap(subscriber, evType, filter);
		//log here filter modified
	}
}// namespace bus_multithread

