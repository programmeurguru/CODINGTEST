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
#include "Event.h"

namespace EventBus2
{
	class Subscriber
	{
	public:
		virtual void consume(Event e) = 0;

		virtual ~Subscriber() {};
	};

	typedef std::function<bool(Event)> Predicate;

	typedef std::pair<std::shared_ptr<Subscriber>, Predicate > SubscriberPredicatePair;
	typedef std::vector<SubscriberPredicatePair> VectSubscriberPredicatePair;
	typedef std::unordered_map<Event::EventType, VectSubscriberPredicatePair > SubscribersPredicateMap;

	class SharedSubscribersPredicateMap : public SubscribersPredicateMap
	{
	private:
		std::shared_mutex _mSubscriber;
	public:
		int insertToSubscriberMap(std::shared_ptr<Subscriber> subscriber, const Event::EventType& evtype, Predicate filter);
		std::vector<std::shared_ptr<Subscriber>> getSubscribersFromEvent(Event anEvent);
	};

	typedef std::vector <std::shared_ptr<Subscriber>> VectSubscriber;
	typedef std::unordered_map<Event::EventType, VectSubscriber > SubscribersMap;

	class SharedSubscribersMap : public SubscribersMap
	{
	private:
		std::shared_mutex _mSubscriber;
	public:
		int insertToSubscriberMap(std::shared_ptr<Subscriber> subscriber, const Event::EventType& evtype);
		VectSubscriber getSubscribersFromEvent(Event anEvent);
	};

};

