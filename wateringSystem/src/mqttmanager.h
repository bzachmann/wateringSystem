#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>

class MqttManager
{
public:
    MqttManager();

    void init();
    void update();

    void setWifiConnected(bool connected);
    
    bool getStartManualWateringCmd();
    uint16_t getManualWateringDurationSec();
    bool getStopWateringCmd();

private:
    bool manageMqttConnection();
    static void staticMqttCallback(char * topic, byte * data, unsigned int length); //this shouldnt be static but needs to be for PubSub lib
    void mqttCallback(char * topic, byte * data, unsigned int length); //this shouldnt be static but needs to be for PubSub lib
    void subscribeToTopics();

    static String statusToStr(int8_t status);
    static void printMqttPayload(char* topic, byte * data, unsigned int length);
    static int32_t mqttPayloadToInt(byte * data, uint32_t length);

public:
    static MqttManager inst;

private:
    bool wifiConnected;
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    char const * clientID;

    bool startNewManualWateringCmd;
    bool stopWateringCmd;
    uint16_t manualWateringDurationSec;
};

//extern void initMqttConnection();
//extern bool manageMqttConnection();
//extern void runMqttProcess();
//
//extern bool getStartManualWateringCmd(); //latched to true on rcv, set to false on consumption
//extern uint16_t getManualWateringDurationSec();
//
//extern bool getStopWateringCmd(); //latched to true on rcv, set to false on consumption



#endif /* MQTTMANAGER_H */