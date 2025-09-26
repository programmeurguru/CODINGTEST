#pragma once
#include <chrono>
#include <queue>
#include <thread>
#include <ctime>
#include <mutex>

namespace throttler
{

    enum ThrottleResult
    {
        PROCEED, // publish, aggregate etc
        DO_NOT_PROCEED //
    };

    /// <summary>
	/// this class is used to limit the number of event processed in a given duration
    /// </summary>
    class Throttler
    {
        mutable bool _stop;

        std::recursive_mutex _mtxEvent;
        int _maxItem;
        int _currentItemCount;
        std::chrono::milliseconds _maxDuration;
        std::chrono::milliseconds _currentDuration;
        std::deque<std::chrono::milliseconds>& _timeDeque;// everytime a event proceed is sent we add a timestamp in queue 

        int _counted;
        std::chrono::milliseconds _countedDuration;


    public:
        Throttler(std::chrono::duration<int, std::milli> duration, int maxitem, std::deque<std::chrono::milliseconds>& timeq)
            : _maxDuration(duration), _maxItem(maxitem), _timeDeque(timeq), _stop(false)
        {
        }

        bool stop()
        {
            _stop = true;
        };

        bool start()
        {
            _stop = false;
        };

        // check if we can proceed (poll)
        ThrottleResult shouldProceed();

        // subscribe to be told when we can proceed (Push)
        void notifyWhenCanProceed();

        void countMaxDurationElem(std::deque<std::chrono::milliseconds>& dq,
            std::chrono::milliseconds current_time,
            std::chrono::milliseconds MaxDuration,
            int MaxElem,
            int& Counted,
            std::chrono::milliseconds& countedDuration);

        // delete all older that 2*max duration
        void ResetDurationAndEventcount(std::chrono::milliseconds lastToKeep);

        // shoul be private : it is here por testing purpose to avoid time issue in debug
        // normally in tests we just have to do shouldProceed and do a wait 
        ThrottleResult stampAndProceed(std::chrono::milliseconds lastTime);
    };
}// namespace throttler

