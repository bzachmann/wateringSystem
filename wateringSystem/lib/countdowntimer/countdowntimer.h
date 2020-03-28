#ifndef COUNTDOWNTIMER_H
#define COUNTDOWNTIMER_H

#include <stdint.h>
#include <stdbool.h>

class CountdownTimer
{
public:
    CountdownTimer();

    void setDuration(uint32_t ms);
    void start();
    void stop();
    void update();

    bool isRunning();
    bool isExpired();
    uint32_t time();

private:
    uint32_t remainingTime;
    bool running;

    uint32_t prevTime;
};


#endif /* COUNTDOWNTIMER_H */