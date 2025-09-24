#include "throttler.h"

ThrottleResult Throttler::shouldProceed()
{
    auto nowTime = std::chrono::system_clock::now().time_since_epoch(); // in clock time
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime); // in duration

    std::lock_guard<std::recursive_mutex> lk(_mtxEvent);
    countMaxDurationElem(_timeDeque, current_time, _maxDuration, _maxItem, _counted, _countedDuration);
    if (_counted < _maxItem)
    {
        return stampAndProceed(current_time);
    }
    //in the duration periode we can't send more than maxItems
    // if _maxDuration reached we reset and resend
    // if (res.first >= _maxItem)
    return DO_NOT_PROCEED;
}

// subscribe to be told when we can proceed (Push)
void Throttler::notifyWhenCanProceed()
{
    std::chrono::milliseconds countedDuration;
    {
        std::lock_guard<std::recursive_mutex> lk(_mtxEvent);
        ThrottleResult retvalue = shouldProceed();
        countedDuration = _countedDuration;
        if (retvalue == PROCEED)
        {
            return;
        }
    }
    // maximun reached for the period waiot and reset
    std::this_thread::sleep_for(_maxDuration - countedDuration);
    auto nowTime = std::chrono::system_clock::now().time_since_epoch(); // in clock time
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime); // in duration

    stampAndProceed(current_time);
}

void Throttler::countMaxDurationElem(std::deque<std::chrono::milliseconds>& dq,
    std::chrono::milliseconds current_time,
    std::chrono::milliseconds MaxDuration,
    int MaxElem,
    int& Counted,
    std::chrono::milliseconds& countedDuration)
{
    //here we count the number of elem from current_time to the one just before MaxDuration, if more than MaxElem 
    Counted = 0;
    countedDuration = std::chrono::milliseconds(0);
    std::deque<std::chrono::milliseconds>::reverse_iterator it = dq.rbegin();
    std::chrono::milliseconds difftime = std::chrono::milliseconds(0);
    std::chrono::milliseconds difftimelast = std::chrono::milliseconds(0);
    while (it != dq.rend())
    {
        Counted++;
        if (Counted >= MaxElem)
        {
            break;
        }
        difftimelast = current_time - *it;
        if (difftimelast >= MaxDuration)
        {
            break;
        }
        else
        {
            countedDuration = difftimelast;
        }
        it++; //reverse iterator go to lower side
    }
    return;
}

ThrottleResult Throttler::stampAndProceed(std::chrono::milliseconds lastTime)
{
    std::lock_guard<std::recursive_mutex> lk(_mtxEvent);
    _timeDeque.push_back(lastTime);
    return PROCEED;
}
// delete all older that n*max where n is a  parameter tha can be passe in parameter
void Throttler::ResetDurationAndEventcount(std::chrono::milliseconds lastToKeep)
{
    std::lock_guard<std::recursive_mutex> lk(_mtxEvent);
    std::deque<std::chrono::milliseconds>::iterator it = _timeDeque.begin();
    while (it != _timeDeque.end() && *it < lastToKeep)
    {
        it++;
    }
    _timeDeque.erase(_timeDeque.begin(), it);
}

