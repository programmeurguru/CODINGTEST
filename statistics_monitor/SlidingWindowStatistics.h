#pragma once
#include "Measurement.h"
#include "subscriber.h"
#include <mutex>
#include "statistics.h"
#include "EventBus.h"
#include "Event.h"

namespace statistics_monitor
{
	typedef Measurement<int> MeasurementInt;
	
	struct Caracteristics
	{
		int pctileThreshold;
	};

	class SlidingWindowStatistics
	{
		std::mutex _mtx;
		// the calculator contains also the data
		StatisticsProvider& _statisticsProvider;
		EventBus& _EventBus;
		const std::string _statisticsEventName;
		EventStatistics _lastdata;
		 
		EventStatistics copyStatistics();

		//second bus for statistique publishing
		void publishEvent(Event<EventStatistics> e);
		EventStatistics getLatestStatistics();
	public:

		SlidingWindowStatistics(EventBus& EventBus, StatisticsProvider& statisticsProvider, const std::string& statisticsEventName) :
			_EventBus(EventBus), _statisticsProvider(statisticsProvider), _statisticsEventName(statisticsEventName)
		{
		}
		
		void aquire(MeasurementInt m);

		// subscriber will have a callback that'll deliver a Statistics instance (push)
		void subscribeForStatistics(const std::shared_ptr<Subscriber<EventStatistics>> & subscriber, const std::string & t, Predicate p)
		{
			_EventBus.addSubscriberForFilteredEvents(subscriber, t, p);
		}


	};

}