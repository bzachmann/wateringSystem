#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <stdint.h>

extern void initMqttConnection();
extern bool manageMqttConnection();
extern void runMqttProcess();

extern uint16_t manualWateringTimeRemainingSec;
extern bool manualWateringOn;

extern bool getStartManualWateringCmd(); //latched to true on rcv, set to false on consumption
extern uint16_t getManualWateringDurationSec();

extern bool getStopWateringCmd(); //latched to true on rcv, set to false on consumption


#endif /* MQTTMANAGER_H */