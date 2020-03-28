#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <stdint.h>

extern void initMqttConnection();
extern bool manageMqttConnection();
extern void runMqttProcess();

extern uint16_t manualWateringTimeRemainingSec;
extern bool manualWateringOn;


#endif /* MQTTMANAGER_H */