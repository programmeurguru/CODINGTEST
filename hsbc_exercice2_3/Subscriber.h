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

namespace EventBus3
{
	class Subscriber
	{
	public:
		virtual void consume(Event e);

		virtual ~Subscriber();
	};

	typedef std::function<bool(Event)> Predicate;

	typedef std::pair<Subscriber*, Predicate > SubscriberPredicatePair;
	typedef std::vector<SubscriberPredicatePair> VectSubscriberPredicatePair;
	typedef std::unordered_map<Event::EventType, VectSubscriberPredicatePair > SubscribersPredicateMap;

	class SafeSubscribersPredicateMap : public SubscribersPredicateMap
	{
	private:
		std::shared_mutex _mSubscriber;
	public:
		int insertToSubscriberMap(Subscriber* subscriber, const Event::EventType& evtype, Predicate filter);
		std::vector<Subscriber*> getSubscribersFromEvent(Event anEvent);
	};

};

