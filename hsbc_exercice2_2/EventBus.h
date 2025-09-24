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
#include <atomic>
#include "sharedQueue.h"
#include "Event.h"
#include "Subscriber.h"
#include "Event.h"

namespace EventBus2
{

	typedef	std::queue<Event> EventQueue;

	class EventBus
	{
		private:
			mutable bool _stop;
	
			std::vector<std::thread> & _consumers;

			SharedSubscribersPredicateMap& _subscribersPredicateMap;
			SharedSubscribersMap& _subscribersMap;

			EventQueue _queue;
			std::mutex _mQueue;//used for exclusif push and pop Event
			std::condition_variable _cvq;

			EventBus(const EventBus& d) = delete; // no copy constructible
			EventBus(EventBus&& d) = delete; // no constructible move

		public:
			EventBus(SharedSubscribersMap& subscribersMap, SharedSubscribersPredicateMap& subscribersPredicateMap, 
				EventQueue& queue, std::vector<std::thread>& consumers )
				: _subscribersMap(subscribersMap), _subscribersPredicateMap(subscribersPredicateMap), _queue (queue), _stop(false), _consumers(consumers) {}

			void start();

			void notify();

			void stop();

			void publishEvent(const Event & e);

			void addSubscriber(const std::shared_ptr<Subscriber>& subscriber, const Event::EventType& evType);

			// Would you allow clients to filter the events they receive? How would the interface look like?
			// I expect that the event contains data on which we can define a filter 
			// for instance a value reaches a threshold
			void addSubscriberForFilteredEvents(const std::shared_ptr<Subscriber>& subscriber, const Event::EventType& evtype, Predicate filter);

	};

}

