#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <publishingmanager.h>

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

    void setPumpState(bool state);
    void setWateringTimeRemainingSec(uint16_t sec);

private:
    void manageMqttConnection();
    static void staticMqttCallback(char * topic, byte * data, unsigned int length);
    void mqttCallback(char * topic, byte * data, unsigned int length);
    void subscribeToTopics();

    void managePumpStatePublishing();

    static String statusToStr(int8_t status);
    static void printMqttPayload(char* topic, byte * data, unsigned int length);
    static int32_t mqttPayloadToInt(byte * data, uint32_t length);

public:
    static MqttManager inst;

private:
    bool wifiConnected;
    bool mqttConnected;
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    char const * clientID;

    bool startNewManualWateringCmd;
    bool stopWateringCmd;
    uint16_t manualWateringDurationSec;

    PublishingManager pumpStatePublisher;
    PublishingManager wateringTimeRemainingPublisher;
};

#endif /* MQTTMANAGER_H */