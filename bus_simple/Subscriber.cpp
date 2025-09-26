#include "Subscriber.h"
namespace bus_simple
{
	void Subscriber::consume(const Event& e)
	{
		// do something
	}

	Subscriber::~Subscriber()
	{
		//unsubscribe();
	}
}// namespace bus_simple