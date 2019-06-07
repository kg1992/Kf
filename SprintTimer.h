#ifndef SPRINT_TIMER_H_
#define SPRINT_TIMER_H_

#include <chrono>
#include "TimerTime.h"

class SprintTimer
{
public:
    SprintTimer();

    void Start();

    void Stop();

    TimerTime GetTimerTime();

private:
    bool m_stop;
    std::chrono::high_resolution_clock m_sprintClock;
    std::chrono::high_resolution_clock::time_point m_sprintBegin;
    std::chrono::high_resolution_clock::time_point m_sprintNow;
    TimerTime m_time;
};

#endif
