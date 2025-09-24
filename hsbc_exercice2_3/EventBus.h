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

namespace EventBus3
{
	// here we put counter increment in constructor with event as parameter
	// default constructor and copy constructor don't increment it so no issues when pass by copy or when used in vector for instance
		
	typedef	std::queue<StampedEvent> EventQueue;

	class EventBus
	{
	private:
		mutable bool _stop;

		// the _consumers is for implementing the start in multinthreading to be implemented
		std::vector<std::thread>& _consumers;

		SafeSubscribersPredicateMap& _subscribersPredicateMap;

		EventQueue _queue;
		std::unordered_map<Event::EventType, unsigned long long int > _eventMap;

		std::mutex _mQueue;//used for exclusif push and pop Event
		std::condition_variable _cvq;

		EventBus(const EventBus& d) = delete; // no copy constructible
		EventBus(EventBus&& d) = delete; // no constructible move

	public:
		EventBus(SafeSubscribersPredicateMap& subscribersPredicateMap, EventQueue& queue, std::vector<std::thread>& consumers)
			: _subscribersPredicateMap(subscribersPredicateMap), _queue(queue), _stop(true), _consumers(consumers) {}

		void start();

		void consume();
		
		void stop();

		void publishEvent(Event e);

		void addSubscriber(Subscriber* subscriber, const Event::EventType& evType);

		// Would you allow clients to filter the events they receive? How would the interface look like?
		// I expect that the event contains data on which we can define a filter 
		// for instance a value reaches a threshold
		void addSubscriberForFilteredEvents(Subscriber* subscriber, const Event::EventType& evtype, Predicate filter);

		static std::vector<Event> ExtractLatest(std::queue<StampedEvent>& qe, const std::unordered_map<Event::EventType, unsigned long long int >& eventMap);

	};

}

