#include <cassert>
#include "Subscriber.h"
namespace EventBus2
{
	std::vector<Subscriber*> SharedSubscribersPredicateMap::getSubscribersFromEvent(Event anEvent)
	{
		VectSubscriberPredicatePair subscribers;
		{
			// many reader possible at the same time
			std::shared_lock<std::shared_mutex> lk(_mSubscriber);

			SubscribersPredicateMap::iterator it = find(anEvent.getEventType());
			if (it == end())
			{
				return std::vector<Subscriber*>();//return empty
			}

			subscribers = it->second;// her a copy to avoid lock delay
		}
		std::vector<Subscriber*> ret;

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
	int SharedSubscribersPredicateMap::insertToSubscriberMap(Subscriber* subscriber, const Event::EventType& et, Predicate filter)
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


	int SharedSubscribersMap::insertToSubscriberMap(Subscriber* subscriber, const Event::EventType& et)
	{
		std::lock_guard<std::shared_mutex> lk(_mSubscriber);
		int ret = 0;
		SubscribersMap::iterator it = find(et);
		if (it == end())
		{
			auto pairBoolIt = insert(std::make_pair(et, std::vector<Subscriber*>()));
			if (pairBoolIt.second)
			{
				auto& vecSubcriber = pairBoolIt.first->second;
				vecSubcriber.emplace_back(subscriber);
			}
			else
			{
				ret = -1;
			}
		}
		else
		{
			auto& vecSubcriber = it->second;
			//if find the subscriber for the same eventType replace it
			std::vector<Subscriber*>::iterator found =
				std::find_if(vecSubcriber.begin(), vecSubcriber.end(), [&](auto s) { return s == subscriber; });

			if (found != vecSubcriber.end())
			{
				//do nothing 
				ret = 1;
			}
			else
			{
				vecSubcriber.emplace_back(subscriber);
			}
		}
		return ret;

	}
	std::vector<Subscriber*> SharedSubscribersMap::getSubscribersFromEvent(Event anEvent)
	{
		std::vector<Subscriber*> subscribers;
		{
			// many reader possible at the same time
			std::shared_lock<std::shared_mutex> lk(_mSubscriber);

			SubscribersMap::iterator it = find(anEvent.getEventType());
			if (it == end())
			{
				return std::vector<Subscriber*>();//return empty
			}
			else
			{
				subscribers = it->second;// her a copy to avoid lock delay

			}
		}
		return subscribers;
	}

}
