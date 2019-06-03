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
    bool stop;
    std::chrono::high_resolution_clock sprintClock;
    std::chrono::high_resolution_clock::time_point sprintBegin;
    std::chrono::high_resolution_clock::time_point sprintNow;
    TimerTime time;
};

#endif
