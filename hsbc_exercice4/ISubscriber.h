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
#include "Statistics.h"

namespace hsbctest4
{
	template<class T>
	class Subscriber
	{
	public:
		virtual void consume(Event<T> e);

		virtual ~Subscriber();
	};

	typedef std::function<bool(Event<EventStatistics>)> Predicate;

	typedef std::pair<std::shared_ptr<Subscriber<EventStatistics>>, Predicate > SubscriberPredicatePair;
	typedef std::vector<SubscriberPredicatePair> VectSubscriberPredicatePair;
	typedef std::unordered_map<std::string, VectSubscriberPredicatePair > SubscribersPredicateMap;


	template<class T>
	class SafeSubscribersPredicateMap : public SubscribersPredicateMap
	{
	private:
		std::shared_mutex _mSubscriber;
	public:
		int insertToSubscriberMap(std::shared_ptr < Subscriber<T>> subscriber, const std::string& evtype, Predicate filter);
		std::vector< std::shared_ptr<Subscriber<T>> > getSubscribersFromEvent(Event<T> anEvent);
	};

	template<class T>
	void Subscriber<T>::consume(Event<T> e)
	{
		// do something
	}

	template<class T>
	Subscriber<T>::~Subscriber()
	{
		//unsubscribe();
	}

	template<class T>
	std::vector<std::shared_ptr<Subscriber<T>>> SafeSubscribersPredicateMap<T>::getSubscribersFromEvent(Event<T> anEvent)
	{
		VectSubscriberPredicatePair subscribers;
		{
			// many reader possible at the same time
			std::shared_lock<std::shared_mutex> lk(_mSubscriber);

			SubscribersPredicateMap::iterator it = find(anEvent.getEventType());
			if (it == end())
			{
				return std::vector<std::shared_ptr<Subscriber<T>> >();//return empty
			}

			subscribers = it->second;// her a copy to avoid lock delay
		}
		std::vector<Subscriber<T> * > ret;
		//now copy all for all with predicate tested 
		for (SubscriberPredicatePair& subscriberPredPair : subscribers)
		{
			if (subscriberPredPair.second == nullptr || (subscriberPredPair.second != nullptr && subscriberPredPair.second(anEvent)))
			{
				ret.push_back(subscriberPredPair.first);
			}
		}
		return ret;
	};


	// int retcode : -1=not inserted; 0=OK, +1=filter replaced 
	template<class T>
	int SafeSubscribersPredicateMap<T>::insertToSubscriberMap(std::shared_ptr<Subscriber<T>> subscriber, const std::string& et, Predicate filter)
	{
		std::lock_guard<std::shared_mutex> lk(_mSubscriber);
		int ret = 0;
		SubscribersPredicateMap::iterator it = find(et);
		if (it == end())
		{
			auto pairBoolIt = insert(std::make_pair(et, VectSubscriberPredicatePair()));
			if (pairBoolIt.second)
			{
				auto& vecSubcriberPredicatePair = pairBoolIt.first->second;
				vecSubcriberPredicatePair.emplace_back(subscriber, filter);
			}
			else
			{
				ret = -1;
			}
		}
		else
		{
			auto& vecSubcriberPredicatePair = it->second;
			//if find the subscriber for the same eventType replace it
			VectSubscriberPredicatePair::iterator found =
				std::find_if(vecSubcriberPredicatePair.begin(), vecSubcriberPredicatePair.end(), [&](auto s) { return s.first == subscriber; });

			if (found != vecSubcriberPredicatePair.end())
			{
				//replace it 
				found->second = filter;
				ret = 1;
			}
			else
			{
				vecSubcriberPredicatePair.emplace_back(subscriber, filter);
			}
		}
		return ret;
	}
}

