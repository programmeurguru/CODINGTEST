#pragma once
#include <string>

namespace statistics_monitor
{
	/// 	
	/// <summary>
	/// this is a event template class with a type and a value 
	/// </summary>

	template<class T>
	class Event
	{
	public:
		Event() : _eventType(""), _data() {}

		Event(const std::string& eventType, T data) : _eventType(eventType), _data(data)
		{
		}

		const std::string & getEventType() const
		{
			return _eventType;
		}

		bool operator == (const Event& e) const
		{
			return _eventType == e.getEventType() && _data == e.getData();
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