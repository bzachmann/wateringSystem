#include "mqttmanager.h"

#include <RTClib.h>

#include "privateinfo.h"
#include "utilities.h"


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
  dateTimeReferenceSec(1588316400), // 05-01-2020 @ 7:00 am
  scheduledWateringIntervalSec(172800), //48 hours
  scheduledWateringDurationSec(90),
  pumpStatePublisher(mqttClient, String("/wateringSystem/status/pumpState"), true, 60000, true),
  wateringTimeRemainingPublisher(mqttClient, String("/wateringSystem/status/wateringTimeRemaining"), true, 60000, true),
  timeUntilNextScheduledWateringPublisher(mqttClient, String("/wateringSystem/status/timeUntilNextScheduledWatering"), true, 300000, true)
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
  timeUntilNextScheduledWateringPublisher.update();

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

uint32_t MqttManager::getDateTimeReferenceSec()
{
  return dateTimeReferenceSec;
}

uint32_t MqttManager::getScheduledWateringIntervalSec()
{
  return scheduledWateringIntervalSec;
}

uint16_t MqttManager::getScheduledWateringDurationSec()
{
  return scheduledWateringDurationSec;
}

void MqttManager::setPumpState(bool state)
{
  pumpStatePublisher.setValue(state ? String("1") : String("0"));
}

void MqttManager::setWateringTimeRemainingSec(uint16_t sec)
{
  wateringTimeRemainingPublisher.setValue(String(sec));
}

void MqttManager::setTimeUntilScheduledWateringSec(uint32_t sec)
{
  timeUntilNextScheduledWateringPublisher.setValue(formatTimeUntilScheduledWatering(sec));
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
  else if(strcmp(topic, "/wateringSystem/commands/dateTimeReference") == 0)
  {
    bool conversionOk = false;
    uint32_t newDateTimeRef = mqttPayloadToDateTimeRefSec(data, length, conversionOk);
    if(conversionOk)
    {
      dateTimeReferenceSec = newDateTimeRef;
    }
    else
    {
      //invaid value received, publish back the locally stored value
      String formatedDateTimeRef = formatDateTimeRefSec(dateTimeReferenceSec);
      char formatedDateTimeRefCharArr[formatedDateTimeRef.length() + 1];  //+1 for null term
      formatedDateTimeRef.toCharArray(formatedDateTimeRefCharArr, formatedDateTimeRef.length() + 1);
      
      mqttClient.publish("/wateringSystem/commands/dateTimeReference", formatedDateTimeRefCharArr, true);
    }
  }
  else if(strcmp(topic, "/wateringSystem/commands/scheduledWateringInterval") == 0)
  {
    uint32_t newScheduledWateringIntervalSec = static_cast<uint32_t>(mqttPayloadToInt(data, length)) * 60 * 60;
    if(newScheduledWateringIntervalSec != 0)
    {
      scheduledWateringIntervalSec = newScheduledWateringIntervalSec;
    }
    else
    {
      Serial.println("interval not ok");

      String formatedScheduledWateringIntervalHr(((scheduledWateringIntervalSec / 60) / 60));
      char formatedScheduledWateringIntervalHrCharAr[formatedScheduledWateringIntervalHr.length() + 1];
      formatedScheduledWateringIntervalHr.toCharArray(formatedScheduledWateringIntervalHrCharAr, formatedScheduledWateringIntervalHr.length() + 1);

      mqttClient.publish("/wateringSystem/commands/scheduledWateringInterval", formatedScheduledWateringIntervalHrCharAr, true);
    }
  }
  else if(strcmp(topic, "/wateringSystem/commands/scheduledWateringDuration") == 0)
  {
    scheduledWateringDurationSec = static_cast<uint16_t>(mqttPayloadToInt(data, length));
  }
}

void MqttManager::subscribeToTopics()
{
    mqttClient.subscribe("/wateringSystem/commands/manualWateringDuration", 1);
    mqttClient.loop();
    mqttClient.subscribe("/wateringSystem/commands/manualWateringCmd", 1);
    mqttClient.loop();
    mqttClient.subscribe("/wateringSystem/commands/dateTimeReference", 1);
    mqttClient.loop();
    mqttClient.subscribe("/wateringSystem/commands/scheduledWateringInterval", 1);
    mqttClient.loop();
    mqttClient.subscribe("/wateringSystem/commands/scheduledWateringDuration", 1);
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

uint32_t MqttManager::mqttPayloadToDateTimeRefSec(byte * data, uint32_t length, bool & ok)
{
  // 05/01/2020 07:00
  // 0123456789ABCDEF <-- length of 16

  uint32_t retVal = 0;
  ok = false;
  
  bool lengthCorrect = (length >= 16);
  if(!lengthCorrect) 
  {
    Serial.println("length not ok");
    return retVal;
  }

  bool formatCorrect =    (data[2] == '/')
                       && (data[5] == '/')
                       && (data[10] == ' ')
                       && (data[13] == ':');
  if(!formatCorrect) 
  {
    Serial.println("format not ok");
    return retVal;
  }

  String monthStr = "";
  monthStr.concat((char)data[0]);
  monthStr.concat((char)data[1]);
  uint8_t monthValue = monthStr.toInt();

  String dayStr = "";
  dayStr.concat((char)data[3]);
  dayStr.concat((char)data[4]);
  uint8_t dayValue = dayStr.toInt();

  String yearStr = "";
  yearStr.concat((char)data[6]);
  yearStr.concat((char)data[7]);
  yearStr.concat((char)data[8]);
  yearStr.concat((char)data[9]);
  uint16_t yearValue = yearStr.toInt();

  bool dateOk = Utilities::validDate(monthValue, dayValue, yearValue);
  if (!dateOk)
  {
    Serial.println("date not ok");
    return retVal;
  }

  String hourStr = "";
  hourStr.concat((char)data[11]);
  hourStr.concat((char)data[12]);
  uint8_t hourValue = hourStr.toInt();

  String minuteStr = "";
  minuteStr.concat((char)data[14]);
  minuteStr.concat((char)data[15]);
  uint16_t minuteValue = minuteStr.toInt();

  bool timeOk = Utilities::validTime(hourValue, minuteValue);
  if(!timeOk)
  {
    Serial.println("time not ok");
    return retVal;
  }

  //date and time are valid
  ok = true;
  
  DateTime dateTime(yearValue, monthValue, dayValue, hourValue, minuteValue);
  return dateTime.unixtime();
}

String MqttManager::formatDateTimeRefSec(uint32_t dateTimeRefSec)
{
  DateTime dateTime(dateTimeRefSec);

  char buffer[17];  //17 instead of 16 to make room for null term
  sprintf(buffer, "%02u/%02u/%04u %02u:%02u", 
                  dateTime.month(), 
                  dateTime.day(), 
                  dateTime.year(),
                  dateTime.hour(),
                  dateTime.minute());

  return String(buffer);
}

String MqttManager::formatTimeUntilScheduledWatering(uint32_t sec)
{
  sec = std::min(sec, (uint32_t)INT32_MAX);
  TimeSpan timeSpan((int32_t)sec);

  uint16_t days = timeSpan.days();
  uint16_t hours = timeSpan.hours();
  uint16_t minutes = timeSpan.minutes();
  uint16_t seconds = timeSpan.seconds();

  //round up to next minute
  if(seconds > 0)
  {
    minutes++;
  }
  if(minutes > 59)
  {
    minutes = 0;
    hours++;
  }
  if(hours > 23)
  {
    hours = 0;
    days++;
  }

  if (days > 99)
  {
    days = 99;
    hours = 23;
    minutes = 59;
  }

  char buffer[9];
  
  sprintf(buffer, "%02u:%02u:%02u",
                    days,
                    hours,
                    minutes);

  return String(buffer);

}

