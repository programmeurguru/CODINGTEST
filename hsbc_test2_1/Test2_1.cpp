#include <gtest/gtest.h>

#include <functional>
#include <memory>
#include "Subscriber.h"

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

namespace EventBus1 
{
	class SubscriberHistoriseTest : public Subscriber
	{
	public:

		SubscriberHistoriseTest() = default;
		SubscriberHistoriseTest(SubscriberHistoriseTest&&) = default;
		std::vector <Event> _dataHisto;

		void consume(const Event & e) override
		{
			_dataHisto.push_back(e);
		}

		~SubscriberHistoriseTest() = default;
	};

	TEST(EventBusTest, TestEvent)
	{
		Event e("EV1", 10.0);
		EXPECT_EQ(e.getEventType(), "EV1");
		EXPECT_EQ(e.getValue(), 10.0);
	}

	// need to test that the tester is working well as it is a critical component for test
	TEST(EventBusTest, SubscriberHistoriseTest)
	{
		SubscriberHistoriseTest s;
		EXPECT_EQ(s._dataHisto.size(), 0);

		Event e1("EV1", 10.0);
		s.consume(e1);
		EXPECT_EQ(s._dataHisto.size(), 1);
		EXPECT_EQ(s._dataHisto[0], e1);

		Event e2("EV2", 20.0);
		s.consume(e2);
		EXPECT_EQ(s._dataHisto.size(), 2);
		EXPECT_EQ(s._dataHisto[0], e1);
		EXPECT_EQ(s._dataHisto[1], e2);

		s.consume(e1);
		EXPECT_EQ(s._dataHisto.size(), 3);
		EXPECT_EQ(s._dataHisto[0], e1);
		EXPECT_EQ(s._dataHisto[1], e2);
		EXPECT_EQ(s._dataHisto[2], e1);
	}

	TEST(EventBusTest, TestAddSubscriber)
	{
		SubscribersMap subscribersMap;
		EventBus EventBus(subscribersMap);
		Event e1("EV1", 10.0);
		EventBus.publishEvent(e1);
		EXPECT_EQ(subscribersMap.size(), 0);

		std::shared_ptr<SubscriberHistoriseTest> s1 = std::make_shared<SubscriberHistoriseTest>();
		EventBus.addSubscriber(s1, "EV1");
		EXPECT_EQ(subscribersMap.size(), 1);

		auto it = subscribersMap.find("EV1");
		EXPECT_TRUE(it != subscribersMap.end());
		VectSubscriberPredicatePair& v = it->second;
		VectSubscriberPredicatePair::iterator found = std::find_if(v.begin(), v.end(), [&](auto s) { return s.first == s1; });
		EXPECT_TRUE(found != v.end());
		EXPECT_TRUE(found->first == s1);
	}

	TEST(EventBusTest, TestAddSubscriberForFilteredEvents)
	{
		//no subscriber
		SubscribersMap subscribersMap;
		EventBus EventBus(subscribersMap);
		Event e1("EV1", 10.0);
		EventBus.publishEvent(e1);
		EXPECT_EQ(subscribersMap.size(), 0);


		// 1 subscriber
		std::shared_ptr<SubscriberHistoriseTest> s1 = std::make_shared<SubscriberHistoriseTest>();
		EventBus.addSubscriber(s1, "EV1");
		EXPECT_EQ(subscribersMap.size(), 1);

		std::function<bool(const Event&)> predicate = [](const Event& s) {return false; };

		//we replace previous with predicate
		EventBus.addSubscriberForFilteredEvents(s1, "EV1", predicate);
		EXPECT_EQ(subscribersMap.size(), 1);

		// the  map is (eventtype, pair(subscriber, predicate))
		auto it = subscribersMap.find("EV1");
		EXPECT_TRUE(it != subscribersMap.end());

		// the  vector is (pair(subscriber,predicate)
		VectSubscriberPredicatePair& v = it->second;
		// search in vezctor the good subscriber
		VectSubscriberPredicatePair::iterator found = std::find_if(v.begin(), v.end(), [&](auto s) { return s.first == s1; });
		EXPECT_TRUE(found != v.end());
		// it is the good subscriber

		EXPECT_TRUE(found->first == s1);
		// it is the good predicate
		EXPECT_TRUE(!(found->second == nullptr));
		std::function<bool(Event&)> f = found->second;
		EXPECT_TRUE(f(e1) == false);
	}

	TEST(EventBusTest, TestAddSubscriberForFilteredEventsReplaceSubscriber)
	{
		//no subscriber
		SubscribersMap subscribersMap;
		EventBus EventBus(subscribersMap);
		Event e1("EV1", 10.0);
		EventBus.publishEvent(e1);
		EXPECT_EQ(subscribersMap.size(), 0);


		// 1 subscriber
		std::shared_ptr<SubscriberHistoriseTest> s1 = std::make_shared<SubscriberHistoriseTest>();
		std::function<bool(const Event&)> predicate = [](const Event& s) {return false; };

		EventBus.addSubscriberForFilteredEvents(s1, "EV1", predicate);
		EXPECT_EQ(subscribersMap.size(), 1);

		// the  map is (eventtype, pair(subscriber, predicate))
		auto it = subscribersMap.find("EV1");
		EXPECT_TRUE(it != subscribersMap.end());

		// the  vector is (pair(subscriber,predicate)
		VectSubscriberPredicatePair& v = it->second;
		// search in vezctor the good subscriber
		VectSubscriberPredicatePair::iterator found = std::find_if(v.begin(), v.end(), [&](auto s) { return s.first == s1; });
		EXPECT_TRUE(found != v.end());
		// it is the good subscriber

		EXPECT_TRUE(found->first == s1);
		// it is the good predicate
		EXPECT_TRUE(!(found->second == nullptr));
		std::function<bool(Event&)> f = found->second;
		EXPECT_TRUE(f(e1) == false);
	}

	TEST(EventBusTest, TestPublish)
	{
		SubscribersMap subscribersMap;
		EventBus EventBus(subscribersMap);
		std::shared_ptr<SubscriberHistoriseTest> s1 = std::make_shared<SubscriberHistoriseTest>();
		Event e1("EV1", 10.0);
		EventBus.addSubscriber(s1, e1.getEventType());

		EventBus.publishEvent(e1);
		EXPECT_EQ(s1->_dataHisto[0], e1);
	}

	TEST(EventBusTest, TestAllinOne)
	{
		SubscribersMap subscribersMap;
		EventBus EventBus(subscribersMap);

		std::shared_ptr<SubscriberHistoriseTest> s1, s2, s3;
		s1 = std::make_shared<SubscriberHistoriseTest>();
		s2 = std::make_shared<SubscriberHistoriseTest>();
		s3 = std::make_shared<SubscriberHistoriseTest>();
		
		EventBus.addSubscriber(s1, "EV1");
		{
			auto it = subscribersMap.find("EV1");
			EXPECT_TRUE(it != subscribersMap.end());
			VectSubscriberPredicatePair& v = it->second;
			VectSubscriberPredicatePair::iterator found = std::find_if(v.begin(), v.end(), [&](auto s) { return s.first == s1; });
			EXPECT_TRUE(found != v.end());
			EXPECT_TRUE(found->first == s1);
			EXPECT_TRUE(found->second == nullptr);
		}

		Event e1("EV1", 1.1);
		EventBus.publishEvent(e1);
		EXPECT_EQ(s1->_dataHisto[0], e1);


		EventBus.addSubscriber(s2, "EV2");
		{
			auto it = subscribersMap.find("EV2");
			EXPECT_TRUE(it != subscribersMap.end());
			VectSubscriberPredicatePair& v = it->second;
			auto found = std::find_if(v.begin(), v.end(), [&](auto s) { return s.first == s2; });
			EXPECT_TRUE(found != v.end());
			EXPECT_TRUE(found->first == s2);
			EXPECT_TRUE(found->second == nullptr);
		}

		Event e2("EV2", 2.2);
		EventBus.publishEvent(e2);
		EXPECT_EQ(s1->_dataHisto[0], e1);
		EXPECT_EQ(s2->_dataHisto[0], e2);

		EventBus.addSubscriber(s3, "EV3");
		{
			auto it = subscribersMap.find("EV3");
			EXPECT_TRUE(it != subscribersMap.end());
			VectSubscriberPredicatePair& v = it->second;
			auto found = std::find_if(v.begin(), v.end(), [&](auto s) { return s.first == s3; });
			EXPECT_TRUE(found != v.end());
			EXPECT_TRUE(found->first == s3);
			EXPECT_TRUE(found->second == nullptr);
		}

		Event e3("EV3", 3.3);
		EventBus.publishEvent(e3);
		EXPECT_EQ(s1->_dataHisto[0], e1);
		EXPECT_EQ(s2->_dataHisto[0], e2);
		EXPECT_EQ(s3->_dataHisto[0], e3);

		EventBus.addSubscriberForFilteredEvents(s1, "EV2", [](const Event& e) -> bool {return false; });
		{
			auto it = subscribersMap.find("EV2");
			EXPECT_TRUE(it != subscribersMap.end());
			VectSubscriberPredicatePair& v = it->second;
			auto found = std::find_if(v.begin(), v.end(), [&](auto s) { return s.first == s1; });
			EXPECT_TRUE(found != v.end());
			EXPECT_TRUE(found->first == s1);
			EXPECT_TRUE(found->second != nullptr);
		}
		Event e4("EV2", 3.3);
		EventBus.publishEvent(e4);
		// s1 reject EV2
		EXPECT_EQ(s1->_dataHisto[0], e1);
		EXPECT_EQ(s2->_dataHisto[0], e2);
		EXPECT_EQ(s3->_dataHisto[0], e3);
		// S2 take e4 as it is subscribed
		EXPECT_EQ(s2->_dataHisto[1], e4);


		//S2 subscribe now with the predicate
		EventBus.addSubscriberForFilteredEvents(s2, "EV2", [](const Event& e) -> bool {return e.getValue() > 2.0; });
		{
			auto it = subscribersMap.find("EV2");
			EXPECT_TRUE(it != subscribersMap.end());
			VectSubscriberPredicatePair& v = it->second;
			auto found = std::find_if(v.begin(), v.end(), [&](auto s) { return s.first == s2; });
			EXPECT_TRUE(found != v.end());
			EXPECT_TRUE(found->first == s2);
			EXPECT_TRUE(found->second != nullptr);
			// ther is only one
			found++;// check that is other subscriber for the EventType
			found = std::find_if(found, v.end(), [&](auto s) { return s.first == s2; });
			EXPECT_TRUE(found == v.end());
		}

		Event e5("EV2", 3.3);
		EventBus.publishEvent(e5);
		EXPECT_EQ(s1->_dataHisto[0], e1);
		EXPECT_EQ(s2->_dataHisto[0], e2);
		EXPECT_EQ(s3->_dataHisto[0], e3);
		// S2 take e5 as it is subscribed
		EXPECT_EQ(s2->_dataHisto[1], e4);
		EXPECT_EQ(s2->_dataHisto[2], e5);
	}
} //namespace EventBus1