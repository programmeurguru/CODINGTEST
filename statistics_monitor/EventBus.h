#pragma once
#include <thread>
#include <queue>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <functional>
#include <condition_variable>	
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include "Event.h"
#include "Subscriber.h"
#include "Event.h"
#include "statistics.h"

namespace statistics_monitor
{
	// here we put counter increment in constructor with event as parameter
	// default constructor and copy constructor don't increment it so no issues when pass by copy or when used in vector for instance
		
	typedef	std::queue<StampedEvent<EventStatistics> > EventQueue;

	class EventBus
	{
	private:
		mutable bool _stop;

		SafeSubscribersPredicateMap<EventStatistics>& _subscribersPredicateMap;

		EventQueue _queue;
		std::unordered_map<std::string, unsigned long long int > _eventMap;

		std::mutex _mQueue;//used for exclusif push and pop Event
		std::condition_variable _cvq;

		EventBus(const EventBus& d) = delete; // no copy constructible
		EventBus(EventBus&& d) = delete; // no constructible move

	public:
		EventBus(SafeSubscribersPredicateMap<EventStatistics>& subscribersPredicateMap, EventQueue& queue)
			: _subscribersPredicateMap(subscribersPredicateMap), _queue(queue), _stop(true) {}

		void start();

		void consume();
		
		void stop();

		void publishEvent(Event<EventStatistics> e);

		void addSubscriber(const std::shared_ptr<Subscriber<EventStatistics>> & subscriber, const std::string& evType);

		// Would you allow clients to filter the events they receive? How would the interface look like?
		// I expect that the event contains data on which we can define a filter 
		// for instance a value reaches a threshold
		void addSubscriberForFilteredEvents(const std::shared_ptr<Subscriber<EventStatistics>> & subscriber, const typename std::string& evtype, Predicate filter);

		static std::vector<Event<EventStatistics> > ExtractLatest(std::queue<StampedEvent<EventStatistics> >& qe,
			const std::unordered_map<std::string, unsigned long long int >& eventMap);

	};

}

