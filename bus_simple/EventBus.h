#pragma once
#include <queue>
#include <mutex>
#include <vector>
#include <functional>
#include <unordered_map>
#include <map>
#include "Event.h"
#include "Subscriber.h"

namespace bus_simple
{
	class Subscriber;

	typedef std::function<bool(Event)> Predicate;

	typedef std::pair<std::shared_ptr<Subscriber>, Predicate > SubscriberPredicatePair;
	typedef std::vector<SubscriberPredicatePair> VectSubscriberPredicatePair;
	typedef std::unordered_map<Event::EventType, VectSubscriberPredicatePair > SubscribersMap;

	class EventBus
	{
	private:

		SubscribersMap& _subscribersMap;

		EventBus(const EventBus& d) = delete; // no copy constructible
		EventBus(EventBus&& d) = delete; // no constructible move

		void insertToSubscriberMap(const std::shared_ptr<Subscriber>& subscriber, const Event::EventType& evtype, Predicate filter);

	public:
		EventBus(SubscribersMap& subscribersMap) : _subscribersMap(subscribersMap) {}

		void publishEvent(const Event& anEvent);

		void addSubscriber(const std::shared_ptr<Subscriber>& subscriber, const Event::EventType& evType);

		//void removeSubscriber(Subscriber* subscriber, const Event::EventType& evtype);

		// Would you allow clients to filter the events they receive? How would the interface look like?
		// I expect that the event contains data that is for instance stock market value,
		// and we can define a filter when the stock reach a threshold
		// I don't have implemented unsubscribe but it works like subcribe if find the subscriber remove it
		// as subscriber is a pointer there is no issue to remove from vector
		void addSubscriberForFilteredEvents(const std::shared_ptr<Subscriber>& subscriber, const Event::EventType& evtype, Predicate filter);
	};
}// namespace bus_simple