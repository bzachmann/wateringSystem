#include "countdowntimer.h"

#include <Arduino.h>

CountdownTimer::CountdownTimer():
    remainingTime(0),
    running(false)
{

}

void CountdownTimer::setDuration(uint32_t ms)
{
    remainingTime = ms;
}

void CountdownTimer::start()
{
    if(!running)
    {
        running = true;
        prevTime = millis();
    }
}

void CountdownTimer::stop()
{
    running = false;
}

void CountdownTimer::update()
{
    if(running && (remainingTime != 0))
    {
        uint32_t newTime = millis();
        uint32_t elapsedTime = 0;

        if(newTime < prevTime) //rollover
        {
            elapsedTime = (UINT32_MAX - prevTime) + newTime + 1; //+1 for count 0
        }
        else if (newTime > prevTime)
        {
            elapsedTime = newTime - prevTime;
        }

        if(elapsedTime > remainingTime)
        {
            remainingTime = 0;
        }
        else
        {
            remainingTime -= elapsedTime;
        }  

        prevTime = newTime;
    }
}

bool CountdownTimer::isRunning()
{
    return running;
}

bool CountdownTimer::isExpired()
{
    return (remainingTime == 0);
}

uint32_t CountdownTimer::time()
{
    return remainingTime;
}