#include "SlidingWindowStatistics.h"

namespace statistics_monitor
{

	void SlidingWindowStatistics::aquire(MeasurementInt m)
	{
		EventStatistics datacopy;
		std::unique_lock<std::mutex> lk(_mtx);
		{
			_statisticsProvider.aquire(m.getValue());
			datacopy = copyStatistics();
			_lastdata = datacopy;
		}
		publishEvent(Event<EventStatistics>(_statisticsEventName, datacopy));//deuxieme bus 
	}

	void SlidingWindowStatistics::publishEvent(Event<EventStatistics> e)
	{
		_EventBus.publishEvent(e);
	}

	EventStatistics SlidingWindowStatistics::copyStatistics()
	{
		EventStatistics datacopy(_statisticsProvider.getMean(), _statisticsProvider.getMode(), _statisticsProvider.getPctileCalculator());
		return datacopy;
	}

	EventStatistics SlidingWindowStatistics::getLatestStatistics()
	{
		std::unique_lock<std::mutex> lk(_mtx);
		{
			return _lastdata;
		}
	}

}