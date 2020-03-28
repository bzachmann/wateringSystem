#include "wateringmanager.h"

#include <Arduino.h>


WateringManager::WateringManager():
    startManualWateringCmd(false),
    stopWateringCmd(false),
    manualWateringDurationSec(0),
    pumpState(false),
    wateringTimeRemainingSec(0),
    manualWateringTimer()
{

}

void WateringManager::update()
{
    bool manualWateringOn = false;
    uint16_t manualWateringTimeRemaining = 0;

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
    
    pumpState = manualWateringOn;
    wateringTimeRemainingSec = manualWateringTimeRemaining;
}

void WateringManager::setStartManualWateringCmd(bool cmd)
{
    startManualWateringCmd = cmd;
}

void WateringManager::setManualWateringDurationSec(uint16_t sec)
{
    manualWateringDurationSec = sec;
}

void WateringManager::setStopWateringCmd(bool cmd)
{
    stopWateringCmd = cmd;
}

bool WateringManager::getPumpState()
{
    return pumpState;
}

uint16_t WateringManager::getWateringTimeRemainingSec()
{
    return wateringTimeRemainingSec;
}