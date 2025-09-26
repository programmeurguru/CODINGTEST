#include <gtest/gtest.h>
#include <thread>
#include "throttler.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

using namespace throttler;

TEST(ThrottlerTest, TestshouldProceed1)
{
    std::deque<std::chrono::milliseconds> _timeDeque;
    std::chrono::milliseconds maxDuration = std::chrono::milliseconds(6000);
    int maxItem = 3;

    Throttler t(maxDuration, maxItem, _timeDeque);

    auto nowTime = std::chrono::system_clock::now().time_since_epoch(); // in clock time
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime); // in duration

    std::chrono::milliseconds last0 = std::chrono::milliseconds(current_time);
    std::chrono::milliseconds last1 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(2000));
    std::chrono::milliseconds last2 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(3000));
    std::chrono::milliseconds last3 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(4000));
    std::chrono::milliseconds last4 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(5000));

    ThrottleResult  res = t.stampAndProceed(last4);
    res = t.stampAndProceed(last3);
    res = t.stampAndProceed(last2);
    res = t.stampAndProceed(last1);
    res = t.stampAndProceed(last0);

    res = t.shouldProceed();
    EXPECT_EQ( res, DO_NOT_PROCEED );
    EXPECT_EQ(_timeDeque.back(), last0);
}

TEST(ThrottlerTest, TestshouldProceed2)
{
    std::deque<std::chrono::milliseconds> _timeDeque;
    std::chrono::milliseconds maxDuration = std::chrono::milliseconds(200);
    int maxItem = 3;

    Throttler t(maxDuration, maxItem, _timeDeque);

    auto nowTime = std::chrono::system_clock::now().time_since_epoch(); // in clock time
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime); // in duration

    std::chrono::milliseconds last0 = std::chrono::milliseconds(current_time);
    std::chrono::milliseconds last1 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(200));
    std::chrono::milliseconds last2 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(400));
    std::chrono::milliseconds last3 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(600));

    ThrottleResult res = t.stampAndProceed(last3);
    res = t.stampAndProceed(last2);
    res = t.stampAndProceed(last1);

    res = t.shouldProceed();
    EXPECT_EQ(res, PROCEED);
    EXPECT_EQ(_timeDeque.back(), last0);
}


TEST(ThrottlerTest, TestshouldProceed3)
{
    std::deque<std::chrono::milliseconds> _timeDeque;
    std::chrono::milliseconds maxDuration = std::chrono::milliseconds(1000);
    int maxItem = 7;
    
    Throttler t(maxDuration, maxItem, _timeDeque);

    auto nowTime = std::chrono::system_clock::now().time_since_epoch(); // in clock time
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime); // in duration

    std::chrono::milliseconds last0 = std::chrono::milliseconds(current_time);
    std::chrono::milliseconds last1 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(200));
    std::chrono::milliseconds last2 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(300));
    std::chrono::milliseconds last3 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(400));
    std::chrono::milliseconds last4 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(500));
    std::chrono::milliseconds last5 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(600));

    ThrottleResult res = t.stampAndProceed(last5);
    res = t.stampAndProceed(last4);
    res = t.stampAndProceed(last3);
    res = t.stampAndProceed(last2);
    res = t.stampAndProceed(last1);

    res = t.shouldProceed();
    EXPECT_EQ(res, PROCEED);
    EXPECT_EQ(_timeDeque.back(), last0);
}

TEST(ThrottlerTest, TestshouldProceed4)
{
    std::deque<std::chrono::milliseconds> _timeDeque;
    std::chrono::milliseconds maxDuration = std::chrono::milliseconds(350);
    int maxItem = 3;

    Throttler t(maxDuration, maxItem, _timeDeque);

    auto nowTime = std::chrono::system_clock::now().time_since_epoch(); // in clock time
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime); // in duration

    std::chrono::milliseconds last0 = std::chrono::milliseconds(current_time);
    std::chrono::milliseconds last1 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(200));
    std::chrono::milliseconds last2 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(300));
    std::chrono::milliseconds last3 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(400));
    std::chrono::milliseconds last4 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(500));
    std::chrono::milliseconds last5 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(600));

    ThrottleResult res = t.stampAndProceed(last5);
    res = t.stampAndProceed(last4);
    res = t.stampAndProceed(last3);
    res = t.stampAndProceed(last2);
    res = t.stampAndProceed(last1);
    res = t.stampAndProceed(last0);

    res = t.shouldProceed();
    EXPECT_EQ(res, DO_NOT_PROCEED);
    EXPECT_EQ(_timeDeque.back(), last0);
}
TEST(ThrottlerTest, TEstNotifyWhenCanProceed)
{
    std::deque<std::chrono::milliseconds> _timeDeque;
    std::chrono::milliseconds maxDuration = std::chrono::milliseconds(350);
    int maxItem = 3;

    Throttler t(maxDuration, maxItem, _timeDeque);

    auto nowTime = std::chrono::system_clock::now().time_since_epoch(); // in clock time
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime); // in duration

    std::chrono::milliseconds last0 = std::chrono::milliseconds(current_time);
    std::chrono::milliseconds last1 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(001));
    std::chrono::milliseconds last2 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(002));
    std::chrono::milliseconds last3 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(003));
    std::chrono::milliseconds last4 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(500));
    std::chrono::milliseconds last5 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(600));

    ThrottleResult res = t.stampAndProceed(last5);
    res = t.stampAndProceed(last4);
    res = t.stampAndProceed(last3);
    res = t.stampAndProceed(last2);
    res = t.stampAndProceed(last1);
 
    //monothread
    t.notifyWhenCanProceed();
    EXPECT_TRUE(_timeDeque.back() >= last2 + maxDuration);
}

void runSeq(Throttler& throttler)
{
    throttler.notifyWhenCanProceed();
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    throttler.notifyWhenCanProceed();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    throttler.notifyWhenCanProceed();
    std::this_thread::sleep_for(std::chrono::milliseconds(001));
    throttler.notifyWhenCanProceed();
}


TEST(ThrottlerTest, TEstNotifyWhenCanProceedMultithreaded)
{
    std::deque<std::chrono::milliseconds> _timeDeque;
    std::chrono::milliseconds maxDuration = std::chrono::milliseconds(1000);
    int maxItem = 3;

    Throttler throttler(maxDuration, maxItem, _timeDeque);

    auto nowTime = std::chrono::system_clock::now().time_since_epoch(); // in clock time
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime); // in duration

    std::chrono::milliseconds last0 = std::chrono::milliseconds(current_time);
    std::chrono::milliseconds last1 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(001));
    std::chrono::milliseconds last2 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(002));
    std::chrono::milliseconds last3 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(003));
    std::chrono::milliseconds last4 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(500));
    std::chrono::milliseconds last5 = std::chrono::milliseconds(current_time - std::chrono::milliseconds(600));

    ThrottleResult res = throttler.stampAndProceed(last5);
    res = throttler.stampAndProceed(last4);
    res = throttler.stampAndProceed(last3);
    res = throttler.stampAndProceed(last2);
    res = throttler.stampAndProceed(last1);

    //multithreaded
    std::thread t(&runSeq, std::ref(throttler));
    t.join();

    EXPECT_TRUE(_timeDeque[5] >= last2 + maxDuration); //>  
    EXPECT_TRUE(_timeDeque[6] >= _timeDeque[5] + std::chrono::milliseconds(400));//PROCEED
    EXPECT_TRUE(_timeDeque[7] >= _timeDeque[6] + std::chrono::milliseconds(200));//PROCEED  here the 3 are in the same 600ms 
    EXPECT_TRUE(_timeDeque[8] >= _timeDeque[6] + maxDuration);////DO_NOT_PROCEED (because immediattly after thee 7
}

TEST(ThrottlerTest, TEstCountMaxDurationElem)
{
    std::deque<std::chrono::milliseconds> _timeDeque;
    std::chrono::milliseconds maxDuration = std::chrono::milliseconds(600);
    int maxItem = 3;
    int counted;
    std::chrono::milliseconds countedDuration;


    Throttler t(maxDuration, maxItem, _timeDeque);

    auto nowTime = std::chrono::system_clock::now().time_since_epoch(); // in clock time
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime); // in duration

    std::chrono::milliseconds last0 = std::chrono::milliseconds(current_time);
    std::chrono::milliseconds last1 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(200));
    std::chrono::milliseconds last2 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(300));
    std::chrono::milliseconds last3 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(400));
    std::chrono::milliseconds last4 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(500));
    std::chrono::milliseconds last5 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(600));
    std::chrono::milliseconds last6 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(700));
    std::chrono::milliseconds last7 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(800));
    std::chrono::milliseconds last8 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(900));
    std::chrono::milliseconds last9 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(1900));
    std::chrono::milliseconds last10 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(2900));

    ThrottleResult res = t.stampAndProceed(last0);
    res = t.stampAndProceed(last1);
    res = t.stampAndProceed(last2);
    res = t.stampAndProceed(last3);
    res = t.stampAndProceed(last4);
    res = t.stampAndProceed(last5);
    res = t.stampAndProceed(last6);
    res = t.stampAndProceed(last7);
    res = t.stampAndProceed(last8);
    res = t.stampAndProceed(last9);
    res = t.stampAndProceed(last10);

    // the current time is the last one or after the last one, so >last10
    t.Throttler::countMaxDurationElem(_timeDeque, last10, maxDuration, maxItem, counted, countedDuration);

    EXPECT_EQ(counted, 2);
    EXPECT_EQ(countedDuration, std::chrono::milliseconds(0));

    t.Throttler::countMaxDurationElem(_timeDeque, last10, std::chrono::milliseconds(1500), maxItem, counted, countedDuration);
    EXPECT_EQ(counted, 3);
    EXPECT_EQ(countedDuration, std::chrono::milliseconds(1000));

    t.Throttler::countMaxDurationElem(_timeDeque, last10, std::chrono::milliseconds(2000), maxItem, counted, countedDuration);
    EXPECT_EQ(counted, 3);
    EXPECT_EQ(countedDuration, std::chrono::milliseconds(1000));

    t.Throttler::countMaxDurationElem(_timeDeque, last10, std::chrono::milliseconds(2100), maxItem, counted, countedDuration);
    EXPECT_EQ(counted, 3);
    EXPECT_EQ(countedDuration, std::chrono::milliseconds(1000));

    t.Throttler::countMaxDurationElem(_timeDeque, last10, std::chrono::milliseconds(1500), 5, counted, countedDuration);
    EXPECT_EQ(counted, 3);
    EXPECT_EQ(countedDuration, std::chrono::milliseconds(1000));

    t.Throttler::countMaxDurationElem(_timeDeque, last10, std::chrono::milliseconds(3000), 5, counted, countedDuration);
    EXPECT_EQ(counted, 5);
    EXPECT_EQ(countedDuration, std::chrono::milliseconds(2100));

    t.Throttler::countMaxDurationElem(_timeDeque, last10, std::chrono::milliseconds(3000), 12, counted, countedDuration);
    EXPECT_EQ(counted, 11);
    EXPECT_EQ(countedDuration, std::chrono::milliseconds(2900));
}
  

// delete all older that 2*max duration
TEST(ThrottlerTest, TestResetDurationAndEventcount)
{
    std::deque<std::chrono::milliseconds> _timeDeque; 
    std::chrono::milliseconds maxDuration = std::chrono::milliseconds(600);
    int maxItem = 3;

    Throttler t(maxDuration, maxItem, _timeDeque);

    auto nowTime = std::chrono::system_clock::now().time_since_epoch(); // in clock time
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime); // in duration

    std::chrono::milliseconds last1 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(200));
    std::chrono::milliseconds last2 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(300));
    std::chrono::milliseconds last3 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(400));
    std::chrono::milliseconds last4 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(500));
    std::chrono::milliseconds last5 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(600));
    std::chrono::milliseconds last6 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(700));
    std::chrono::milliseconds last7 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(800));
    std::chrono::milliseconds last8 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(900));
    std::chrono::milliseconds last9 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(1900));
    std::chrono::milliseconds last10 = std::chrono::milliseconds(current_time + std::chrono::milliseconds(2900));

    ThrottleResult res = t.stampAndProceed(last1);
    res = t.stampAndProceed(last2);
    res = t.stampAndProceed(last3);
    res = t.stampAndProceed(last4);
    res = t.stampAndProceed(last5);
    res = t.stampAndProceed(last6);
    res = t.stampAndProceed(last7);
    res = t.stampAndProceed(last8);
    res = t.stampAndProceed(last9);
    res = t.stampAndProceed(last10);

    t.Throttler::ResetDurationAndEventcount(last6);
    EXPECT_EQ(_timeDeque[0], last6 );
    EXPECT_EQ(_timeDeque[4], last10);
    EXPECT_EQ(_timeDeque.size(), 5);
}

TEST(ThrottlerTest, TestStampAndProceed)
{

    std::deque<std::chrono::milliseconds> _timeDeque;// everytime a event proceed is sent we add a timestamp in queue 
    std::chrono::milliseconds maxDuration = std::chrono::milliseconds(10000);
    int maxItem = 3;

    Throttler t(maxDuration, maxItem, _timeDeque);

    std::chrono::milliseconds last1 = std::chrono::milliseconds(200000);

    ThrottleResult res = t.stampAndProceed(last1);
    
    EXPECT_EQ(_timeDeque.size(), 1);
    EXPECT_EQ(_timeDeque[0], last1);
    EXPECT_EQ(res,PROCEED);

}
