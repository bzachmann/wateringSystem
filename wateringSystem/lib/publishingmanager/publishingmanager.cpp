#include "publishingmanager.h"

PublishingManager::PublishingManager(PubSubClient &client, String topic, bool retained, uint32_t publishingInterval, bool publishOnChange):
    publishingTimer(),
    mqttClient(&client),
    topic(topic),
    retained(retained),
    publishingInterval(publishingInterval),
    publishOnChange(publishOnChange),
    value(""),
    publishedValue(""),
    started(false)
{

}

void PublishingManager::update()
{
    publishingTimer.update();
    
    bool timeToPublish = (publishingTimer.isRunning() && publishingTimer.isExpired());
    bool valueChanged = value != publishedValue;
    bool mqttConnected = (mqttClient->state() == MQTT_CONNECTED);

    if( mqttConnected
         && (timeToPublish || (valueChanged && publishOnChange)))
    {
        publishingTimer.setDuration(publishingInterval);

        //+1 to account for added null terminator.  Without it, a null term is added, but the last character is sacrificed
        char topicCharArr[topic.length() + 1];
        topic.toCharArray(topicCharArr, topic.length() + 1);

        char valueCharArr[value.length() + 1];
        value.toCharArray(valueCharArr, value.length() + 1);

        mqttClient->publish(topicCharArr, valueCharArr, retained);

        publishedValue = value;
    }
}

void PublishingManager::setValue(String value)
{
    if(!started)
    {
        started = true;

        //force a publish on next update
        publishingTimer.setDuration(0);
        publishingTimer.start();
    }
    this->value = value;
}