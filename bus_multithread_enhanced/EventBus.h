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

namespace bus_multithread_enhanced
{
	// here we put counter increment in constructor with event as parameter
	// default constructor and copy constructor don't increment it so no issues when pass by copy or when used in vector for instance
		
	typedef	std::queue<StampedEvent> EventQueue;

	class EventBus
	{
	private:
		std::atomic<bool> _needStop;
		std::atomic<bool> _ConsumeTerminated;

		std::thread _consumerThread;
		
		SafeSubscribersPredicateMap& _subscribersPredicateMap;

		EventQueue _queue;
		std::unordered_map<Event::Topic, unsigned long long int > _eventMap;

		std::mutex _mQueue;//used for exclusif push and pop Event
		std::mutex _startstop;//avoid 2 start
		std::condition_variable _cvq;

		EventBus(const EventBus& d) = delete; // no copy constructible
		EventBus(EventBus&& d) = delete; // no constructible move
		void consume();

	public:
		~EventBus()
		{
			// wait max 5 sec if not terminated then detach the thread
			int cpt = 0;
			while (!isTerminated() && ++cpt < 5)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
			if (!isTerminated())
			{
				_consumerThread.detach();
			}
			else
			{
				_consumerThread.join();
			}
		}

		EventBus(SafeSubscribersPredicateMap& subscribersPredicateMap, EventQueue& queue)
			: _subscribersPredicateMap(subscribersPredicateMap), _queue(queue), _needStop(false), _ConsumeTerminated(true){}

		void start();

		void stop();

		bool isTerminated() { return _ConsumeTerminated; }

		void publishEvent(Event e);

		void addSubscriber(std::shared_ptr<Subscriber> subscriber, const Event::Topic& evType);

		// Would you allow clients to filter the events they receive? How would the interface look like?
		// I expect that the event contains data on which we can define a filter 
		// for instance a value reaches a threshold
		void addSubscriberForFilteredEvents(std::shared_ptr<Subscriber> subscriber, const Event::Topic& evtype, Predicate filter);

		static std::vector<Event> ExtractLatest(std::queue<StampedEvent>& qe, const std::unordered_map<Event::Topic, unsigned long long int >& eventMap);

	};

}// namespace bus_multithread_enhanced

