#ifndef WATERINGMANAGER_H
#define WATERINGMANAGER_H

#include <stdint.h>

#include <countdowntimer.h>

class WateringManager
{
public:
    WateringManager();
    void update();

    void setStartManualWateringCmd(bool cmd);
    void setManualWateringDurationSec(uint16_t sec);
    
    void setStopWateringCmd(bool cmd);

    bool getPumpState();
    uint16_t getWateringTimeRemainingSec();

private:
    bool startManualWateringCmd;
    bool stopWateringCmd;

    uint16_t manualWateringDurationSec;

    bool pumpState;
    uint16_t wateringTimeRemainingSec;

    CountdownTimer manualWateringTimer;
};

#endif /* WATERINGMANAGER_H */