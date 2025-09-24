#pragma once
#include <string>
namespace EventBus2
{
	class Event
	{
	public:
		typedef std::string EventType;
		Event() : _eventType(""), _value(0.0) {}

		Event(const EventType& eventType, double value) : _eventType(eventType), _value(value)
		{
		}

		const EventType& getEventType() const
		{
			return _eventType;
		}

		double getValue() const
		{
			return _value;
		}

		bool operator == (const Event & e) const
		{
			return _eventType == e.getEventType() && _value == e.getValue();
		}

	private:
		EventType _eventType;
		double _value;
	};
}