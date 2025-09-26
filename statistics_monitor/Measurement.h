#pragma once

namespace statistics_monitor
{
	/// <summary>
	/// this is a simple wrapper to store a measurement of type T
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<class T>
	class Measurement
	{
		T _val;
	public:
		Measurement(T val) : _val(val) {}
		void setValue(T val) { _val = val; }
		T getValue() const { return _val; }

	};
}// namespace statistics_monitor

