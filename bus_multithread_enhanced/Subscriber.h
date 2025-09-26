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

namespace bus_multithread_enhanced
{
	class Subscriber
	{
	public:
		virtual void consume(Event e);

		virtual ~Subscriber();
	};

	typedef std::function<bool(Event)> Predicate;

	typedef std::pair<std::shared_ptr<Subscriber>, Predicate > SubscriberPredicatePair;
	typedef std::vector<SubscriberPredicatePair> VectSubscriberPredicatePair;
	typedef std::unordered_map<Event::Topic, VectSubscriberPredicatePair > SubscribersPredicateMap;

	class SafeSubscribersPredicateMap : public SubscribersPredicateMap
	{
	private:
		std::shared_mutex _mSubscriber;
	public:
		int insertToSubscriberMap(std::shared_ptr<Subscriber> subscriber, const Event::Topic& evtype, Predicate filter);
		std::vector<std::shared_ptr<Subscriber>> getSubscribersFromEvent(Event anEvent);
	};
}// namespace bus_multithread_enhanced


