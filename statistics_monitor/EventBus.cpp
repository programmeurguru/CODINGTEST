#include <cassert>
#include "EventBus.h"

namespace statistics_monitor
{
	unsigned long long int StampedEvent<EventStatistics>::_globalCount = 0;

	void EventBus::stop()
	{
		_stop = true;
		_cvq.notify_all();
	}

	void EventBus::start()
	{
		_stop = false;
	}

	void EventBus::publishEvent(Event<EventStatistics> ev)
	{
		std::unique_lock<std::mutex> lock(_mQueue);
		_queue.push(StampedEvent<EventStatistics>(ev));// auto increment counter in StampedEvent
		_eventMap[_queue.back().getEvent().getEventType()] = _queue.back().getcount();
		_cvq.notify_all();
	}

	std::vector<Event<EventStatistics>> EventBus::ExtractLatest(std::queue<StampedEvent<EventStatistics>> & qe, 
		const std::unordered_map<std::string, unsigned long long int > & eventMap)
	{
		//the unordered map keep the last number of count
		std::vector<Event<EventStatistics>> retq;
		while (!qe.empty())
		{
			StampedEvent<EventStatistics> e = qe.front();
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
		Event<EventStatistics> anEvent;
		while (!_stop)
		{
			std::queue<StampedEvent<EventStatistics>> qe;
			std::unordered_map<std::string, unsigned long long int > eventMap;
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
				std::vector< std::shared_ptr<Subscriber<EventStatistics>>> subscribers = _subscribersPredicateMap.getSubscribersFromEvent(anEvent);
				for (auto it = subscribers.begin(), itEnd = subscribers.end(); it != itEnd; ++it)
				{
					(*it)->consume(anEvent);
				}
			}
		}
	}

	// How would you denote the subscriber?
	void EventBus::addSubscriber(const std::shared_ptr<Subscriber<EventStatistics>> & subscriber, const std::string& evType)
	{
		int ret = _subscribersPredicateMap.insertToSubscriberMap(subscriber, evType, nullptr);
		//log here filter modified or insert failed
	}

	// Would you allow clients to filter the events they receive? How would the interface look like?
	void EventBus::addSubscriberForFilteredEvents(const std::shared_ptr<Subscriber<EventStatistics>> & subscriber,
													const std::string& evType, 
													Predicate filter)
	{
		int ret = _subscribersPredicateMap.insertToSubscriberMap(subscriber, evType, filter);
		//log here filter modified
	}


}

