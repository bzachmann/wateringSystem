#include "wateringmanager.h"

#include <Arduino.h>


WateringManager::WateringManager():
    stopWateringCmd(false),
    startManualWateringCmd(false),
    manualWateringDurationSec(0),
    dateTimeReferenceSec(0xFFFFFFFF),
    startScheduledWateringCmd(false),
    scheduledWateringIntervalSec(0xFFFFFFFF),
    scheduledWateringDurationSec(0),
    currentTimeSec(0),
    pumpState(false),
    wateringTimeRemainingSec(0),
    timeUntilNextScheduledWateringSec(0xFFFFFFFF),
    manualWateringTimer(),
    scheduledWateringTimer()
{

}

void WateringManager::update()
{
    bool manualWateringOn = false;
    uint16_t manualWateringTimeRemaining = 0;
    updateManualWatering(manualWateringOn, manualWateringTimeRemaining);
    
    bool scheduledWateringOn = false;
    uint16_t scheduledWateringTimeRemaining = 0;
    updateScheduledWatering(scheduledWateringOn, scheduledWateringTimeRemaining);

    pumpState = manualWateringOn || scheduledWateringOn;
    wateringTimeRemainingSec = std::max(manualWateringTimeRemaining, scheduledWateringTimeRemaining);
}

void WateringManager::setStopWateringCmd(bool cmd)
{
    stopWateringCmd = cmd;
}

void WateringManager::setStartManualWateringCmd(bool cmd)
{
    startManualWateringCmd = cmd;
}

void WateringManager::setManualWateringDurationSec(uint16_t sec)
{
    manualWateringDurationSec = sec;
}

void WateringManager::setDateTimeReferenceSec(uint32_t sec)
{
    dateTimeReferenceSec = sec;
}

void WateringManager::setScheduledWateringIntervalSec(uint32_t sec)
{
    scheduledWateringIntervalSec = sec;
}

void WateringManager::setScheduledWateringDurationSec(uint16_t sec)
{
    scheduledWateringDurationSec = sec;
}

void WateringManager::setCurrentTimeSec(uint32_t sec)
{
    currentTimeSec = sec;
}

bool WateringManager::getPumpState()
{
    return pumpState;
}

uint16_t WateringManager::getWateringTimeRemainingSec()
{
    return wateringTimeRemainingSec;
}

uint32_t WateringManager::getTimeUntilNextScheduledWateringSec()
{
    return timeUntilNextScheduledWateringSec;
}

void WateringManager::updateManualWatering(bool & manualWateringOn, uint16_t & manualWateringTimeRemaining)
{
    manualWateringOn = false;
    manualWateringTimeRemaining = 0;

    if(startManualWateringCmd)
    {
        manualWateringTimer.setDuration(static_cast<uint32_t>(manualWateringDurationSec) * 1000);
        manualWateringTimer.start();
    }

    if(stopWateringCmd)
    {
        manualWateringTimer.stop();
    }

    manualWateringTimer.update();

    if(manualWateringTimer.isRunning())
    {
        manualWateringOn = true;
        if(manualWateringTimer.isExpired())
        {
            manualWateringOn = false;
            manualWateringTimer.stop();
        }
        manualWateringTimeRemaining = manualWateringTimer.time() / 1000;
    }
    else
    {
        manualWateringOn = false;
        manualWateringTimeRemaining = 0;
    }
}

void WateringManager::updateScheduledWatering(bool & scheduledWateringOn, uint16_t & scheduledWateringTimeRemaining)
{
    scheduledWateringOn = false;
    scheduledWateringTimeRemaining = 0;

    if(currentTimeSec > dateTimeReferenceSec)
    {
        uint32_t timeSinceRefSec = currentTimeSec - dateTimeReferenceSec;
        uint16_t timeSinceLastScheduledStartSec = timeSinceRefSec % scheduledWateringIntervalSec;

        if(timeSinceLastScheduledStartSec < 5) //5 seconds to catch the edge.  This should give the processor plenty of time
        {
            if(!startScheduledWateringCmd) //rising edge
            {
                startScheduledWateringCmd = true;
                scheduledWateringTimer.setDuration(static_cast<uint32_t>(scheduledWateringDurationSec) * 1000);
                scheduledWateringTimer.start();
            }
        }
        else
        {
            startScheduledWateringCmd = false;
        }

        if(stopWateringCmd)
        {
            scheduledWateringTimer.stop();
        }
        
        scheduledWateringTimer.update();

        if(scheduledWateringTimer.isRunning())
        {
            scheduledWateringOn = true;
            if(scheduledWateringTimer.isExpired())
            {
                scheduledWateringOn = false;
                scheduledWateringTimer.stop();
            }
            scheduledWateringTimeRemaining = scheduledWateringTimer.time() / 1000;
        }
        else
        {
            scheduledWateringOn = false;
            scheduledWateringTimeRemaining = 0;
        }

        //time until next scheduled watering
        if(scheduledWateringOn)
        {
            //watering right now
            timeUntilNextScheduledWateringSec = 0;
        }
        else
        {
            timeUntilNextScheduledWateringSec =  scheduledWateringIntervalSec - timeSinceLastScheduledStartSec;
        }
    }
}