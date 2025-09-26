#pragma once
#include <string>
namespace bus_multithread_enhanced
{
	/// <summary>
	/// I take a string as event type to be more generic and a doble as value for example
	/// normlly need to be templated but to keep it simple I use a double
	/// </summary>
	class Event
	{
	public:
		Event() : _eventType(""), _value(0.0) {}

		Event(const std::string& eventType, double value) : _eventType(eventType), _value(value)
		{
		}

		const std::string& getEventType() const
		{
			return _eventType;
		}

		double getValue() const
		{
			return _value;
		}

		bool operator == (const Event& e) const
		{
			return _eventType == e.getEventType() && _value == e.getValue();
		}

	private:
		std::string _eventType;
		double _value;
	};


	class StampedEvent
	{
		Event _e;
		unsigned long long int _count;
		static unsigned long long int _globalCount;
	public:
		StampedEvent(Event e) : _e(e), _count(++_globalCount)
		{
		}
		unsigned long long int getcount() const 
		{ 
			return _count; 
		}
		const Event& getEvent() const 
		{ 
			return _e; 
		}
	};
}// namespace bus_multithread_enhanced