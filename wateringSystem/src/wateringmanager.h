#ifndef WATERINGMANAGER_H
#define WATERINGMANAGER_H

#include <stdint.h>

#include <countdowntimer.h>

class WateringManager
{
public:
    WateringManager();
    void update();

    void setStopWateringCmd(bool cmd);
    void setStartManualWateringCmd(bool cmd);
    void setManualWateringDurationSec(uint16_t sec);
    void setDateTimeReferenceSec(uint32_t sec);
    void setScheduledWateringIntervalSec(uint32_t sec);
    void setScheduledWateringDurationSec(uint16_t sec);
    void setCurrentTimeSec(uint32_t sec);
        
    bool getPumpState();
    uint16_t getWateringTimeRemainingSec();
    uint32_t getTimeUntilNextScheduledWateringSec();

private:
    void updateManualWatering(bool & manualWateringOn, uint16_t & manualWateringTimeRemaining);
    void updateScheduledWatering(bool & scheduledWateringOn, uint16_t & scheduledWateringTimeRemaining);

private:
    bool stopWateringCmd;

    bool startManualWateringCmd;
    uint16_t manualWateringDurationSec;
    uint32_t dateTimeReferenceSec; //Unix time stamp (time since Epoch).  B/c uint32_t, will stop working in 2038
    bool startScheduledWateringCmd;
    uint32_t scheduledWateringIntervalSec;
    uint16_t scheduledWateringDurationSec;
    uint32_t currentTimeSec;

    bool pumpState;
    uint16_t wateringTimeRemainingSec;
    uint32_t timeUntilNextScheduledWateringSec;

    CountdownTimer manualWateringTimer;
    CountdownTimer scheduledWateringTimer;
};

#endif /* WATERINGMANAGER_H */