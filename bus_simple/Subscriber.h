#pragma once

#include "EventBus.h"
#include "Event.h"

namespace bus_simple
{
	class Subscriber
	{
	public:
		virtual void consume(const Event& e);

		// to implement : when destructed unsubscribe
		//void unsubscribe();

		virtual ~Subscriber();
	};
}// namespace bus_simple
