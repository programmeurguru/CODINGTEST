#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include "Subscriber.h"
#include "EventBus.h"

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

namespace EventBus2 {
	class SubscriberHistorizeTest : public Subscriber
	{
	public:
		std::vector <Event> _dataHisto;

		void consume(Event e)
		{
			_dataHisto.push_back(e);
		}
	};
	// here I don't test unitary element as it is already tested in previous solution hsbc_exercice2_1

	TEST(EventBusTest2, TestAllinOne)
	{
		std::vector<std::thread> consumers;

		EventQueue evQueue;
		SharedSubscribersPredicateMap subscribersPredicateMap;
		SharedSubscribersMap subscribersMap;
		EventBus EventBus(subscribersMap, subscribersPredicateMap, evQueue, consumers);
		
		std::thread t(&EventBus::notify, &EventBus);

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		std::shared_ptr<SubscriberHistorizeTest> s1, s2, s3;
		s1 = std::make_shared<SubscriberHistorizeTest>();
		s2 = std::make_shared<SubscriberHistorizeTest>(); 
		s3 = std::make_shared<SubscriberHistorizeTest>(); 

		EventBus.addSubscriber(s1, "EV1");

		{
			auto it = subscribersMap.find("EV1");

			EXPECT_TRUE(it != subscribersMap.end());
			std::vector< std::shared_ptr<Subscriber>> & v = it->second;
			std::vector< std::shared_ptr<Subscriber>>::iterator found = std::find_if(v.begin(), v.end(), [&](auto s) { return s == s1; });
			EXPECT_TRUE(*found == s1);
		}

		Event e1("EV1", 1.1);
		EventBus.publishEvent(e1);
			
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		EXPECT_EQ(s1->_dataHisto[0], e1);

		EventBus.stop();
		t.join();
	}

	TEST(EventBusTest2, EventQueueSingleTest)
	{
		std::vector<std::thread> consumers;
		EventQueue evQueue;
		SharedSubscribersPredicateMap subscribersPredicateMap;
		SharedSubscribersMap subscribersMap;
		EventBus EventBus(subscribersMap, subscribersPredicateMap, evQueue, consumers);
		EventBus.start();

		std::shared_ptr<SubscriberHistorizeTest> s1 = std::make_shared<SubscriberHistorizeTest>();
		Event ev("Event1", 10.10);
		EventBus.addSubscriber(s1, ev.getEventType());
		std::thread t(&EventBus::notify, &EventBus);
		EventBus.publishEvent(ev);
		std::this_thread::sleep_for(std::chrono::milliseconds(4000));

		EventBus.stop();
		t.join();

		EXPECT_EQ(s1->_dataHisto[0], ev);
	}

	TEST(EventBusTest2, EventQueuethreeTest)
	{
		std::vector<std::thread> consumers;
		EventQueue evQueue;
		SharedSubscribersPredicateMap subscribersPredicateMap;
		SharedSubscribersMap subscribersMap;
		EventBus EventBus(subscribersMap, subscribersPredicateMap, evQueue, consumers);
		EventBus.start();

		std::shared_ptr<SubscriberHistorizeTest> s1, s2, s3;
		s1 = std::make_shared<SubscriberHistorizeTest>();
		s2 = std::make_shared<SubscriberHistorizeTest>();
		s3 = std::make_shared<SubscriberHistorizeTest>();
		Event ev1("Event1", 10.10);
		Event ev2("Event2", 11.11);
		Event ev3("Event3", 12.12);
		EventBus.addSubscriber(s1, ev1.getEventType());
		EventBus.addSubscriber(s2, ev2.getEventType());
		EventBus.addSubscriber(s3, ev3.getEventType());

		std::thread t(&EventBus::notify, &EventBus);

		EventBus.publishEvent(ev1);
		EventBus.publishEvent(ev2);
		EventBus.publishEvent(ev3);

		std::this_thread::sleep_for(std::chrono::milliseconds(4000));

		EventBus.stop();

		t.join();

		EXPECT_EQ(s1->_dataHisto[0], ev1);
		EXPECT_EQ(s2->_dataHisto[0], ev2);
		EXPECT_EQ(s3->_dataHisto[0], ev3);
	}


	TEST(EventBusTest2, EventQueueOveridesubscriptionTest)
	{
		// possible test : check that the thread ID is different for consumer and currentthread;
		// it is possible to use a thread in EventBus member and start it as normally it should have only one thread 
		std::vector<std::thread> consumers;
		EventQueue evQueue;
		SharedSubscribersPredicateMap subscribersPredicateMap;
		SharedSubscribersMap subscribersMap;
		EventBus EventBus(subscribersMap, subscribersPredicateMap, evQueue, consumers);
		EventBus.start();

		std::shared_ptr<SubscriberHistorizeTest> s1, s2, s3;
		s1 = std::make_shared<SubscriberHistorizeTest>();
		s2 = std::make_shared<SubscriberHistorizeTest>();
		s3 = std::make_shared<SubscriberHistorizeTest>();
		Event ev1("Event1", 10.10);
		Event ev2("Event2", 11.11);
		Event ev3("Event3", 12.12);
		EventBus.addSubscriber(s1, ev1.getEventType());
		EventBus.addSubscriber(s1, ev2.getEventType());
		EventBus.addSubscriber(s1, ev3.getEventType());
		EventBus.addSubscriber(s2, ev2.getEventType());
		EventBus.addSubscriber(s2, ev3.getEventType());
		EventBus.addSubscriber(s3, ev3.getEventType());

		std::thread t(&EventBus::notify, &EventBus);

		EventBus.publishEvent(ev1);
		EventBus.publishEvent(ev2);
		EventBus.publishEvent(ev3);

		std::this_thread::sleep_for(std::chrono::milliseconds(4000));

		EventBus.stop();
		t.join();

		EXPECT_EQ(s1->_dataHisto[0], ev1);
		EXPECT_EQ(s1->_dataHisto[1], ev2);
		EXPECT_EQ(s1->_dataHisto[2], ev3);

		EXPECT_EQ(s2->_dataHisto[0], ev2);
		EXPECT_EQ(s2->_dataHisto[1], ev3);

		EXPECT_EQ(s3->_dataHisto[0], ev3);


	}



}//end namespace

