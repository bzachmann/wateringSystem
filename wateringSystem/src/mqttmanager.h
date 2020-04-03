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
    uint32_t getDateTimeReferenceSec();
    uint32_t getScheduledWateringIntervalSec();
    uint16_t getScheduledWateringDurationSec();
    bool getRainDelay();

    void setPumpState(bool state);
    void setWateringTimeRemainingSec(uint16_t sec);
    void setTimeUntilScheduledWateringSec(uint32_t sec);
    void resetRainDelay();

private:
    void manageMqttConnection();
    static void staticMqttCallback(char * topic, byte * data, unsigned int length);
    void mqttCallback(char * topic, byte * data, unsigned int length);
    void subscribeToTopics();

    static String statusToStr(int8_t status);
    static void printMqttPayload(char* topic, byte * data, unsigned int length);
    static int32_t mqttPayloadToInt(byte * data, uint32_t length);
    static uint32_t mqttPayloadToDateTimeRefSec(byte * data, uint32_t length, bool & ok);
    static String formatDateTimeRefSec(uint32_t dateTimeRefSec);
    static String formatTimeUntilScheduledWatering(uint32_t sec);

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
    uint32_t dateTimeReferenceSec; //Unix time stamp (time since Epoch).  B/c uint32_t, will stop working in 2038
    uint32_t scheduledWateringIntervalSec;
    uint16_t scheduledWateringDurationSec;
    bool rainDelay;

    PublishingManager pumpStatePublisher;
    PublishingManager wateringTimeRemainingPublisher;
    PublishingManager timeUntilNextScheduledWateringPublisher;
};

#endif /* MQTTMANAGER_H */