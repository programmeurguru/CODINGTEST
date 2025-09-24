#pragma once

namespace hsbctest4 
{
	template<class T>
	class Measurement
	{
		T _val;
	public:
		Measurement(T val) : _val(val) {}
		void setValue(T val) { _val = val; }
		T getValue() const { return _val; }

	};
}

