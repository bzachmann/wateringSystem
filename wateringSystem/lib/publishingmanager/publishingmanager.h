#ifndef PUBLISHINGMANAGER_H
#define PUBLISHINGMANAGER_H

#include <PubSubClient.h>
#include <countdowntimer.h>

class PublishingManager
{
public:
    PublishingManager(PubSubClient &client, String topic, bool retained, uint32_t publishingInterval, bool publishOnChange);
    void update();

    void setValue(String val);


private:
    CountdownTimer publishingTimer;
    PubSubClient * mqttClient;

    String topic;
    bool retained;
    uint32_t publishingInterval;
    bool publishOnChange;
    String value;
    String publishedValue;
    bool started;
};

#endif /* PUBLISHINGMANAGER_H */