#include "SprintTimer.h"

SprintTimer::SprintTimer()
    : stop(true)
    , sprintClock()
    , sprintBegin()
    , sprintNow()
    , time({ 0,0,0,0 })
{
}

void SprintTimer::Start()
{
    if (stop)
    {
        sprintBegin = sprintClock.now();
        stop = false;
    }
}

void SprintTimer::Stop()
{
    if (!stop)
    {
        sprintNow = sprintClock.now();
        stop = true;
    }
}

TimerTime SprintTimer::GetTimerTime()
{
    if (!stop)
        sprintNow = sprintClock.now();

    auto duration = sprintNow - sprintBegin;
    TimerTime tt = { 0,0,0 };
    long long total = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    tt.msec = total % 1000;
    tt.sec = (total / 1000) % 60;
    tt.min = (total / 60000) % 60;
    tt.hour = static_cast<int>(total / 3600000);

    return tt;
}
