#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include "statistics.h"
#include "EventBus.h"
#include "Subscriber.h"
#include "SlidingWindowStatistics.h"
#include "Measurement.h"

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

namespace statistics_monitor
{
	/// <summary>
	///	test the MeanCalculator class get one by one and divide by the counted values	
	/// </summary>
	TEST(statistics_monitor, MeanCalculatorTest)
	{
		MeanCalculator m;
		m.AddAndcalculate(1);
		EXPECT_EQ(m.get(), 1.0);
		m.AddAndcalculate(1);
		EXPECT_EQ(m.get(), 1.0);
		m.AddAndcalculate(4);
		EXPECT_EQ(m.get(), 2.0);
		m.AddAndcalculate(4);
		EXPECT_EQ(m.get(), 2.5);
	}

	/// <summary>
	///	test the ModCalculator return the integer with the highest count
	/// </summary>
	TEST(statistics_monitor, ModCalculatorTest)
	{
		ModCalculator m;
		m.AddAndcalculate(1);
		EXPECT_EQ(m.get(), 1);
		m.AddAndcalculate(1);
		EXPECT_EQ(m.get(), 1);
		m.AddAndcalculate(4);
		EXPECT_EQ(m.get(), 1);
		m.AddAndcalculate(4);
		EXPECT_EQ(m.get(), 1);
		m.AddAndcalculate(4);
		EXPECT_EQ(m.get(), 4);
		m.AddAndcalculate(2);
		EXPECT_EQ(m.get(), 4);
		m.AddAndcalculate(2);
		EXPECT_EQ(m.get(), 4);
		m.AddAndcalculate(2);
		EXPECT_EQ(m.get(), 4);
		m.AddAndcalculate(2);
		EXPECT_EQ(m.get(), 2);
		m.AddAndcalculate(4);
		EXPECT_EQ(m.get(), 2);
		m.AddAndcalculate(4);
		EXPECT_EQ(m.get(), 4);
	}

	/// <summary>
	///	test 
	/// </summary>
	TEST(statistics_monitor, PctileCalculatorTest)
	{
		PctileCalculator m;
		m.AddAndcalculate(1);
		EXPECT_EQ(m.get(0), 1);
		EXPECT_EQ(m.get(10), 1);
		EXPECT_EQ(m.get(100), 1);
		m.AddAndcalculate(2);
		EXPECT_EQ(m.get(0), 1);
		EXPECT_EQ(m.get(10), 1);
		EXPECT_EQ(m.get(50), 1);
		EXPECT_EQ(m.get(51), 1);
		EXPECT_EQ(m.get(100), 2);

	}

	TEST(statistics_monitor, StatisticsTest)
	{
		Statistics s(1.1, 1, 2, 3);
		EXPECT_EQ(s.getMean(), 1.1);
		EXPECT_EQ(s.getMode(), 1);
		EXPECT_EQ(s.getPctile(), 2);
		EXPECT_EQ(s.getPctilethreshold(), 3);

		Statistics s1;
		s1.setMean(1.1);
		s1.setMode(1);
		s1.setPctile(2);
		s1.setPctilethreshold(3);
		EXPECT_EQ(s.getMean(), 1.1);
		EXPECT_EQ(s.getMode(), 1);
		EXPECT_EQ(s.getPctile(), 2);
		EXPECT_EQ(s.getPctilethreshold(), 3);

	}
	TEST(statistics_monitor, StatisticsProviderTest)
	{
		StatisticsProvider s;
		s.aquire(1);
		EXPECT_EQ(s.getMean(), 1);
		EXPECT_EQ(s.getMode(), 1);
		EXPECT_EQ(s.getPctile(0), 1);
		EXPECT_EQ(s.getPctile(10), 1);
		EXPECT_EQ(s.getPctile(100), 1);
		s.aquire(2);
		EXPECT_EQ(s.getMean(), 1.5);
		EXPECT_EQ(s.getMode(), 1);
		EXPECT_EQ(s.getPctile(0), 1);
		EXPECT_EQ(s.getPctile(10), 1);
		EXPECT_EQ(s.getPctile(50), 1);
		EXPECT_EQ(s.getPctile(51), 1);
		EXPECT_EQ(s.getPctile(99), 1);
		EXPECT_EQ(s.getPctile(100), 2);
	}
	TEST(statistics_monitor, EventStatisticsTest)
	{
		PctileCalculator pctileCalc;
		pctileCalc.AddAndcalculate(1);
		pctileCalc.AddAndcalculate(2);

		EventStatistics ev(1.1, 2, pctileCalc);
		EXPECT_EQ(ev.getMean(), 1.1);
		EXPECT_EQ(ev.getMode(), 2);
		EXPECT_EQ(ev.getPctile(0), 1);
		EXPECT_EQ(ev.getPctile(10), 1);
		EXPECT_EQ(ev.getPctile(50), 1);
		EXPECT_EQ(ev.getPctile(51), 1);
		EXPECT_EQ(ev.getPctile(100), 2);

		pctileCalc.AddAndcalculate(10);
		EXPECT_EQ(ev.getPctile(50), 1);
		EXPECT_EQ(ev.getPctile(51), 1);
		EXPECT_EQ(ev.getPctile(100), 2);
	}

	TEST(statistics_monitor, ExtractLastest)
	{
		std::queue<StampedEvent<EventStatistics>> qe;
		std::unordered_map<std::string, unsigned long long int > eventMap;
		PctileCalculator p1;
		p1.AddAndcalculate(1);
		p1.AddAndcalculate(2);
		EventStatistics evs1(1.0, 1, p1);

		PctileCalculator p2;
		p2.AddAndcalculate(1);
		p2.AddAndcalculate(2);
		p2.AddAndcalculate(3);
		p2.AddAndcalculate(4);
		EventStatistics evs2(2.0, 10, p2);

		PctileCalculator p3;
		p3.AddAndcalculate(1);
		p3.AddAndcalculate(3);
		p3.AddAndcalculate(4);
		EventStatistics evs3(2.0, 10, p3);


		PctileCalculator p4;
		p4.AddAndcalculate(1);
		p4.AddAndcalculate(3);
		p4.AddAndcalculate(4);
		EventStatistics evs4(3.0, 11, p4);


		StampedEvent<EventStatistics> e1(Event<EventStatistics>("EV1", evs1));
		qe.push(e1);
		eventMap[e1.getEvent().getEventType()] = e1.getcount();

		StampedEvent<EventStatistics> e2(Event<EventStatistics>("EV2", evs1));
		qe.push(e2);
		eventMap[e2.getEvent().getEventType()] = e2.getcount();

		StampedEvent<EventStatistics> e3(Event<EventStatistics>("EV1", evs2));
		qe.push(e3);
		eventMap[e3.getEvent().getEventType()] = e3.getcount();

		StampedEvent<EventStatistics> e4(Event<EventStatistics>("EV3", evs1));
		qe.push(e4);
		eventMap[e4.getEvent().getEventType()] = e4.getcount();

		StampedEvent<EventStatistics> e5(Event<EventStatistics>("EV2", evs3));
		qe.push(e5);
		eventMap[e5.getEvent().getEventType()] = e5.getcount();

		StampedEvent<EventStatistics> e6(Event<EventStatistics>("EV3", evs4));
		qe.push(e6);
		eventMap[e6.getEvent().getEventType()] = e6.getcount();

		std::vector<Event<EventStatistics>> res = EventBus::ExtractLatest(qe, eventMap);
		EXPECT_EQ(res[0].getEventType(), "EV1");
		EXPECT_EQ(res[0].getData().getMean(), evs2.getMean());
		EXPECT_EQ(res[0].getData().getMode(), evs2.getMode());
		EXPECT_EQ(res[0].getData().getPctile(10), evs2.getPctile(10));

		EXPECT_EQ(res[1].getEventType(), "EV2");
		EXPECT_EQ(res[1].getData().getMode(), evs3.getMode());
		EXPECT_EQ(res[1].getData().getPctile(10), evs3.getPctile(10));

		EXPECT_EQ(res[2].getEventType(), "EV3");
		EXPECT_EQ(res[2].getData().getMean(), evs4.getMean());
		EXPECT_EQ(res[2].getData().getMode(), evs4.getMode());
		EXPECT_EQ(res[2].getData().getPctile(10), evs4.getPctile(10));

	}
	
	class SubscriberHistoriseTest : public Subscriber<EventStatistics>
	{
	public:

		SubscriberHistoriseTest() = default;
		SubscriberHistoriseTest(const SubscriberHistoriseTest&) = default;
		SubscriberHistoriseTest(SubscriberHistoriseTest &&) = default;
		std::vector<Event<EventStatistics>> _dataHisto;

		void consume(Event<EventStatistics> e)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			_dataHisto.push_back(e);
		}
	};	

	TEST(statistics_monitor, conflationTest)
	{
		EventQueue evQueue;
		SafeSubscribersPredicateMap<EventStatistics> subscribersPredicateMap;
		EventBus EventBus(subscribersPredicateMap, evQueue);

		std::shared_ptr<SubscriberHistoriseTest> s1 = std::make_shared<SubscriberHistoriseTest>();
		std::shared_ptr<SubscriberHistoriseTest> s2 = std::make_shared<SubscriberHistoriseTest>();

		StatisticsProvider s;

		EventBus.start();
		std::thread t(&EventBus::consume, &EventBus);

		SlidingWindowStatistics slws(EventBus, s, std::string("EVENTTYPE"));

		slws.subscribeForStatistics(s1, "EVENTTYPE", nullptr);
		slws.subscribeForStatistics(s2, "EVENTTYPE", [](Event < EventStatistics> e) { return e.getData().getMean() > 3; });

		slws.aquire(MeasurementInt(3));

		std::this_thread::sleep_for(std::chrono::milliseconds(3000));


		EXPECT_EQ(s1->_dataHisto[0].getEventType(), "EVENTTYPE");
		EXPECT_EQ(s1->_dataHisto[0].getData().getMean(), 3);
		EXPECT_EQ(s1->_dataHisto[0].getData().getMode(), 3);
		EXPECT_EQ(s1->_dataHisto[0].getData().getPctile(10), 3);

		EXPECT_EQ(s2->_dataHisto.size(), 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		slws.aquire(MeasurementInt(2));
		slws.aquire(MeasurementInt(2));
		slws.aquire(MeasurementInt(1));
		slws.aquire(MeasurementInt(22));

		std::this_thread::sleep_for(std::chrono::milliseconds(3000));

		EventBus.stop();
		t.join();

		EXPECT_EQ(s1->_dataHisto.size(), 3);

		EXPECT_EQ(s1->_dataHisto[0].getEventType(), "EVENTTYPE");
		EXPECT_EQ(s1->_dataHisto[0].getData().getMean(), 3);
		EXPECT_EQ(s1->_dataHisto[0].getData().getMode(), 3);
		EXPECT_EQ(s1->_dataHisto[0].getData().getPctile(10), 3);

		EXPECT_EQ(s1->_dataHisto[1].getEventType(), "EVENTTYPE");
		EXPECT_EQ(s1->_dataHisto[1].getData().getMean(), 2.5);
		EXPECT_EQ(s1->_dataHisto[1].getData().getMode(), 3);
		EXPECT_EQ(s1->_dataHisto[1].getData().getPctile(10), 2);

		EXPECT_EQ(s1->_dataHisto[2].getEventType(), "EVENTTYPE");
		EXPECT_EQ(s1->_dataHisto[2].getData().getMean(), 6);
		EXPECT_EQ(s1->_dataHisto[2].getData().getMode(), 2);
		EXPECT_EQ(s1->_dataHisto[2].getData().getPctile(10), 1);

		EXPECT_EQ(s2->_dataHisto.size(), 1);
		EXPECT_EQ(s2->_dataHisto[0].getEventType(), "EVENTTYPE");
		EXPECT_EQ(s2->_dataHisto[0].getData().getMean(), 6);
		EXPECT_EQ(s2->_dataHisto[0].getData().getMode(), 2);
		EXPECT_EQ(s2->_dataHisto[0].getData().getPctile(1), 1);
		EXPECT_EQ(s2->_dataHisto[0].getData().getPctile(10), 1);
	}
}
