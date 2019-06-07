#include "SprintTimer.h"

SprintTimer::SprintTimer()
    : m_stop(true)
    , m_sprintClock()
    , m_sprintBegin()
    , m_sprintNow()
    , m_time({ 0,0,0,0 })
{
}

void SprintTimer::Start()
{
    if (m_stop)
    {
        m_sprintBegin = m_sprintClock.now();
        m_stop = false;
    }
}

void SprintTimer::Stop()
{
    if (!m_stop)
    {
        m_sprintNow = m_sprintClock.now();
        m_stop = true;
    }
}

TimerTime SprintTimer::GetTimerTime()
{
    if (!m_stop)
        m_sprintNow = m_sprintClock.now();

    auto duration = m_sprintNow - m_sprintBegin;
    TimerTime tt = { 0,0,0 };
    long long total = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    tt.msec = total % 1000;
    tt.sec = (total / 1000) % 60;
    tt.min = (total / 60000) % 60;
    tt.hour = static_cast<int>(total / 3600000);

    return tt;
}
