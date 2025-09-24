#pragma once

#include "EventBus.h"
#include "Event.h"


class Subscriber
{
public :
		virtual void consume(const Event & e);

		// to implement : when destructed unsubscribe
		//void unsubscribe();

		virtual ~Subscriber();
};
