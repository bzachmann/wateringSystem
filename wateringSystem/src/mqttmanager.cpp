#include "mqttmanager.h"

#include <WiFi.h>
#include <PubSubClient.h>

#include "privateinfo.h"

#include <countdowntimer.h>



//********* Static Objects *********//
static WiFiClient wifiClient;
static PubSubClient mqttClient(wifiClient);
static char const * clientID = "WateringSystemPumpManager";

static uint16_t manualWateringDurationSec = 90;
uint16_t manualWateringTimeRemainingSec = 0;
bool manualWateringOn = false;
static bool startNewManualWateringCmd = false;
static bool stopManualWateringCmd = false;

//********* Static Function Prototypes *********//
static void mqttCallback(char* topic, byte * data, unsigned int length);
static void subscribeToTopics();
static String statusToStr(int8_t status);

static void updateManualWateringTimer();

//********* Public Functions *********//
void initMqttConnection()
{
    mqttClient.setServer(mqtt_server_addr, mqtt_server_port);
    mqttClient.setCallback(mqttCallback);
}

bool manageMqttConnection() 
//Should only be called if connected to network
//returns true if successfully connected to mqtt server
{
  bool retVal = false;
  
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
    retVal = true;
    
    if(!subscribed)
    {
      subscribed = true;

      Serial.println("");
      Serial.println("MQTT connected");
      Serial.println("Subscribing to topics");
      subscribeToTopics();
    }
  }

  return retVal;
}

void runMqttProcess()
{
  mqttClient.loop();

  updateManualWateringTimer();
}

//********* Static Functions *********//
static void mqttCallback(char* topic, byte * data, unsigned int length)
{
    Serial.println("");
    Serial.print("New MQTT message from topic ");
    Serial.print(topic);
    Serial.print(": ");
    for(uint16_t i = 0; i < length; i++)
    {
        Serial.print((char)data[i]);
    }

    if(strcmp(topic, "/wateringSystem/commands/manualWateringDuration") == 0)
    {
      String temp = "";
      for(uint16_t i = 0; i < length; i++)
      {
        temp.concat((char)data[i]);
      }
      manualWateringDurationSec = temp.toInt();//String(((char*)data)).toInt();//atoi((char*)data);
    }
    else if(strcmp(topic, "/wateringSystem/commands/manualWateringCmd") == 0)
    {
      if(atoi((char*)data) == 0)
      {
        stopManualWateringCmd = true;
      }
      else
      {
        startNewManualWateringCmd = true;
      }
    }
}

static void subscribeToTopics()
{
    mqttClient.subscribe("/#", 1);
    mqttClient.loop();
}


static String statusToStr(int8_t status)
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

void updateManualWateringTimer()
{
  static CountdownTimer manualWateringTimer;

  if (startNewManualWateringCmd)
  {
    startNewManualWateringCmd = false;
    manualWateringTimer.setDuration((uint32_t)manualWateringDurationSec * 1000);
    manualWateringTimer.start();
  }

  if(stopManualWateringCmd)
  {
    stopManualWateringCmd = false;
    manualWateringTimer.stop();
  }

  manualWateringTimer.update();

  if(manualWateringTimer.isRunning())
  {
    manualWateringOn = true;
    if(manualWateringTimer.isExpired())
    {
      manualWateringOn = false;
      manualWateringTimer.stop();
    }
    manualWateringTimeRemainingSec = manualWateringTimer.time() / 1000;
  }
  else
  {
    manualWateringOn = false;
    manualWateringTimeRemainingSec = 0;
  }
}