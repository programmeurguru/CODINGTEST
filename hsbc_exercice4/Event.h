#pragma once
#include <string>
namespace hsbctest4
{
	// event type and all other can be templatized
	template<class T>
	class Event
	{
	public:
		Event() : _eventType(""), _value(0.0) {}

		Event(const std::string& eventType, double value, T data) : _eventType(eventType), _value(value), _data(data)
		{
		}

		const std::string & getEventType() const
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

		const T& getData() const
		{
			return _data;
		}
		T getDataCopy() const
		{
			return _data;
		}

	private:
		std::string _eventType;
		double _value;
		T _data;
	};

	template<class T>
	class StampedEvent
	{
	public:
		StampedEvent(Event<T> e) : _e(e), _count(++_globalCount)
		{
		}
		unsigned long long int getcount() const 
		{ 
			return _count; 
		}
		const Event<T>& getEvent() const 
		{ 
			return _e; 
		}
	private:
		Event<T> _e;
		unsigned long long int _count;
		static unsigned long long int _globalCount;
	};
}