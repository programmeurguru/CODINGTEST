#include <cassert>
#include "Subscriber.h"
namespace EventBus3
{
	void Subscriber::consume(Event e)
	{
		// do something
	}

	Subscriber::~Subscriber()
	{
		//unsubscribe();
	}


	std::vector<Subscriber*> SafeSubscribersPredicateMap::getSubscribersFromEvent(Event anEvent)
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
	int SafeSubscribersPredicateMap::insertToSubscriberMap(Subscriber* subscriber, const Event::EventType& et, Predicate filter)
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
