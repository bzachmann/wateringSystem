#include "mqttmanager.h"

#include "privateinfo.h"

MqttManager MqttManager::inst;

MqttManager::MqttManager():
  wifiConnected(false),
  mqttConnected(false),
  wifiClient(),
  mqttClient(wifiClient),
  clientID("WateringSystemPumpManager"),
  startNewManualWateringCmd(false),
  stopWateringCmd(false),
  manualWateringDurationSec(90),
  pumpStatePublisher(mqttClient, String("/wateringSystem/status/pumpState"), true, 60000, true),
  wateringTimeRemainingPublisher(mqttClient, String("/wateringSystem/status/wateringTimeRemaining"), true, 60000, true)
{

}

void MqttManager::init()
{
  mqttClient.setServer(mqtt_server_addr, mqtt_server_port);
  mqttClient.setCallback(staticMqttCallback);
}

void MqttManager::update()
{
  manageMqttConnection();

  pumpStatePublisher.update();
  wateringTimeRemainingPublisher.update();

  mqttClient.loop();
}

void MqttManager::setWifiConnected(bool connected)
{
  wifiConnected = connected;
}

bool MqttManager::getStartManualWateringCmd()
{
  bool retVal = startNewManualWateringCmd;
  startNewManualWateringCmd = false;
  return retVal;
}

uint16_t MqttManager::getManualWateringDurationSec()
{
  return manualWateringDurationSec;
}

bool MqttManager::getStopWateringCmd()
{
  bool retVal = stopWateringCmd;
  stopWateringCmd = false;
  return retVal;
}

void MqttManager::setPumpState(bool state)
{
  pumpStatePublisher.setValue(state ? String("1") : String("0"));
}

void MqttManager::setWateringTimeRemainingSec(uint16_t sec)
{
  wateringTimeRemainingPublisher.setValue(String(sec));
}

void MqttManager::manageMqttConnection() 
//Should only be called if connected to network
//returns true if successfully connected to mqtt server
{
  mqttConnected = false;
  
  if(wifiConnected)
  {
    static bool subscribed = false;

    if(mqttClient.state() != MQTT_CONNECTED)
    {
      subscribed = false;

      static uint32_t time = millis() + 6000;
      uint32_t newTime = millis();

      if(   (time > newTime) //rollover
         || ((newTime - time) > 5000))
      {
        time = newTime;
        Serial.println("");
        Serial.println("Connecting mqtt");
        mqttClient.disconnect();
        mqttClient.connect(clientID);
      }

      static int8_t state = 50;
      int8_t newState = mqttClient.state();

      if(newState != state)
      {
        state = newState;
        Serial.print("MQTT status: ");
        Serial.println(statusToStr(newState));
      }
    }
    else
    {    
      mqttConnected = true;

      if(!subscribed)
      {
        subscribed = true;

        Serial.println("");
        Serial.println("MQTT connected");
        Serial.println("Subscribing to topics");
        subscribeToTopics();
      }
    }
  }
} 

void MqttManager::staticMqttCallback(char * topic, byte * data, unsigned int length)
{
  inst.mqttCallback(topic, data, length);
}

void MqttManager::mqttCallback(char * topic, byte * data, unsigned int length)
{
  printMqttPayload(topic, data, length);

  if(strcmp(topic, "/wateringSystem/commands/manualWateringDuration") == 0)
  {
    manualWateringDurationSec = static_cast<uint16_t>(mqttPayloadToInt(data, length));
  }
  else if(strcmp(topic, "/wateringSystem/commands/manualWateringCmd") == 0)
  {
    if(static_cast<char>(data[0]) == '0')
    {
      stopWateringCmd = true;
    }
    else
    {
      startNewManualWateringCmd = true;
    }
  }
}

void MqttManager::subscribeToTopics()
{
    mqttClient.subscribe("/wateringSystem/commands/manualWateringDuration", 1);
    mqttClient.loop();
    mqttClient.subscribe("/wateringSystem/commands/manualWateringCmd", 1);
    mqttClient.loop();
}

String MqttManager::statusToStr(int8_t status)
{
    String retVal = "";

    switch(status)
    {
        case static_cast<int8_t>(MQTT_CONNECTION_TIMEOUT):
        retVal = "MQTT_CONNECTION_TIMEOUT";
        break;

        case static_cast<int8_t>(MQTT_CONNECTION_LOST):
        retVal = "MQTT_CONNECTION_LOST";
        break;

        case static_cast<int8_t>(MQTT_CONNECT_FAILED):
        retVal = "MQTT_CONNECT_FAILED";
        break;

        case static_cast<int8_t>(MQTT_DISCONNECTED):
        retVal = "MQTT_DISCONNECTED";
        break;

        case static_cast<int8_t>(MQTT_CONNECTED):
        retVal = "MQTT_CONNECTED";
        break;

        case static_cast<int8_t>(MQTT_CONNECT_BAD_PROTOCOL):
        retVal = "MQTT_CONNECT_BAD_PROTOCOL";
        break;

        case static_cast<int8_t>(MQTT_CONNECT_BAD_CLIENT_ID):
        retVal = "MQTT_CONNECT_BAD_CLIENT_ID";
        break;

        case static_cast<int8_t>(MQTT_CONNECT_UNAVAILABLE):
        retVal = "MQTT_CONNECT_UNAVAILABLE";
        break;

        case static_cast<int8_t>(MQTT_CONNECT_BAD_CREDENTIALS):
        retVal = "MQTT_CONNECT_BAD_CREDENTIALS";
        break;

        case static_cast<int8_t>(MQTT_CONNECT_UNAUTHORIZED):
        retVal = "MQTT_CONNECT_UNAUTHORIZED";
        break;

        default:
        retVal = "UKNOWN";
        break;
    }

    return retVal;
}

void MqttManager::printMqttPayload(char* topic, byte * data, unsigned int length)
{
  Serial.println("");
  Serial.print("New MQTT message from topic ");
  Serial.print(topic);
  Serial.print(": ");
  for(uint16_t i = 0; i < length; i++)
  {
      Serial.print((char)data[i]);
  }
  Serial.println("");
}

int32_t MqttManager::mqttPayloadToInt(byte * data, uint32_t length)
{
    String str = "";
    for(uint32_t i = 0; i < length; i++)
    {
      str.concat(static_cast<char>(data[i]));
    }
    return str.toInt();
}

