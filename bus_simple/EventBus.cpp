#include <queue>
#include <functional>
#include "EventBus.h"

namespace bus_simple
{
	void EventBus::publishEvent(const Event& anEvent)
	{
		SubscribersMap::iterator it = _subscribersMap.find(anEvent.getEventType());
		if (it != _subscribersMap.end())
		{
			VectSubscriberPredicatePair& subscribers = it->second;
			for (SubscriberPredicatePair& subscriberPred : subscribers)
			{
				if (subscriberPred.second == nullptr || (subscriberPred.second != nullptr && subscriberPred.second(anEvent)))
				{
					subscriberPred.first->consume(anEvent);
				}
			}
		}
	};

	void EventBus::insertToSubscriberMap(const std::shared_ptr<Subscriber>& subscriber, const Event::EventType& et, Predicate filter)
	{
		SubscribersMap::iterator it = _subscribersMap.find(et);
		if (it == _subscribersMap.end())
		{
			auto pairBoolIt = _subscribersMap.insert(std::make_pair(et, VectSubscriberPredicatePair()));
			if (!pairBoolIt.second)
			{
				throw "error : cannot subscribe";
			}
			else
			{
				auto& vecSubcriberPredicatePair = pairBoolIt.first->second;
				vecSubcriberPredicatePair.emplace_back(subscriber, filter);
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
			}
			else
			{
				vecSubcriberPredicatePair.emplace_back(subscriber, filter);
			}
		}

	}

	// How would you denote the subscriber?
	void EventBus::addSubscriber(const std::shared_ptr<Subscriber>& subscriber, const Event::EventType& evType)
	{
		insertToSubscriberMap(subscriber, evType, nullptr);

	}

	// Would you allow clients to filter the events they receive? How would the interface look like?
	void EventBus::addSubscriberForFilteredEvents(const std::shared_ptr<Subscriber>& subscriber, const Event::EventType& evType, Predicate filter)
	{
		insertToSubscriberMap(subscriber, evType, filter);
	}
}// namespace bus_simple

