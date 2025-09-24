#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include "Subscriber.h"
#include "EventBus.h"

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

namespace EventBus3 {
	class SubscriberHistoriseTest : public Subscriber
	{
	public:
		std::vector <Event> _dataHisto;

		void consume(Event e)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			_dataHisto.push_back(e);
		}
	};

	TEST(EventBusTest3, ExtractLastest)
	{
		std::queue<StampedEvent> qe;
		std::unordered_map<Event::EventType, unsigned long long int > eventMap;

		StampedEvent e1(Event("EV1", 10.0));
		qe.push(e1);
		eventMap[e1.getEvent().getEventType()] = e1.getcount();

		StampedEvent e2(Event("EV2", 11.0));
		qe.push(e2);
		eventMap[e2.getEvent().getEventType()] = e2.getcount();
		
		StampedEvent e3(Event("EV1", 12.0));
		qe.push(e3);
		eventMap[e3.getEvent().getEventType()] = e3.getcount();

		StampedEvent e4(Event("EV3", 13.0));
		qe.push(e4);
		eventMap[e4.getEvent().getEventType()] = e4.getcount();

		StampedEvent e5(Event("EV2", 14.0));
		qe.push(e5);
		eventMap[e5.getEvent().getEventType()] = e5.getcount();

		StampedEvent e6(Event("EV3", 15.0));
		qe.push(e6);
		eventMap[e6.getEvent().getEventType()] = e6.getcount();

		std::vector<Event> res = EventBus::ExtractLatest(qe, eventMap);
		EXPECT_EQ(res[0].getEventType(), "EV1");
		EXPECT_EQ(res[0].getValue(), 12.0);
		EXPECT_EQ(res[1].getEventType(), "EV2");
		EXPECT_EQ(res[1].getValue(), 14);
		EXPECT_EQ(res[2].getEventType(), "EV3");
		EXPECT_EQ(res[2].getValue(), 15);

	}
	TEST(EventBusTest3, conflationTest)
	{
		std::vector<std::thread> consumers;
		EventQueue evQueue;
		SafeSubscribersPredicateMap subscribersPredicateMap;
		EventBus EventBus(subscribersPredicateMap, evQueue, consumers);
		EventBus.start();

		SubscriberHistoriseTest s1;
		SubscriberHistoriseTest s2;
		SubscriberHistoriseTest s3;

		Event e1("EV1", 10.0);
		Event e2("EV2", 11.0);
		Event e3("EV1", 12.0);
		Event e4("EV3", 13.0);
		Event e5("EV2", 14.0);
		Event e6("EV2", 14.0);
		Event e7("EV3", 15.0);

		EventBus.addSubscriber(&s1, e1.getEventType());
		EventBus.addSubscriber(&s2, e2.getEventType());
		EventBus.addSubscriber(&s3, e4.getEventType());

		std::thread t(&EventBus::consume, &EventBus);

		EventBus.publishEvent(e1);
		EventBus.publishEvent(e2);
		EventBus.publishEvent(e3);
		EventBus.publishEvent(e4);
		EventBus.publishEvent(e5);
		EventBus.publishEvent(e6);
		EventBus.publishEvent(e7);

		std::this_thread::sleep_for(std::chrono::milliseconds(6000));

		EventBus.stop();
		if (t.joinable())
		{
			t.join();
		}
		else
		{
			t.~thread();
		}

		EXPECT_EQ(s1._dataHisto.size(), 1);
		EXPECT_EQ(s1._dataHisto[0], e3);

		EXPECT_EQ(s2._dataHisto.size(), 1);
		EXPECT_EQ(s2._dataHisto[0], e6);

		EXPECT_EQ(s3._dataHisto.size(), 1);
		EXPECT_EQ(s3._dataHisto[0], e7);

	}
}
