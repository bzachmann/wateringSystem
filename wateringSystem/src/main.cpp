#include <Arduino.h>
#include <RTClib.h>

#include "wifimanager.h"
#include "mqttmanager.h"
#include "wateringmanager.h"

#include <countdowntimer.h>


static WateringManager wateringManager;
//static RTC_DS1307 rtc;

CountdownTimer secondTimer;
uint32_t timeNow;

void setup() 
{
  Serial.begin(115200);
  delay(10);

  //rtc.begin();  
  MqttManager::inst.init();

  secondTimer.setDuration(1000);
  secondTimer.start();

  DateTime compileTime(F(__DATE__), F(__TIME__));
  timeNow = compileTime.unixtime();
  
}

void loop() 
{
  WifiManager::inst.update();
  
  MqttManager::inst.setWifiConnected(WifiManager::inst.isConnected());
  MqttManager::inst.update();

  //bool rtcAvailable = rtc.isrunning();
  //DateTime currentTime = rtc.now();

  secondTimer.update();
  if(secondTimer.isExpired())
  {
    secondTimer.setDuration(1000);
    timeNow++;
  }

  wateringManager.setCurrentTimeSec(timeNow, true); //wateringManager.setCurrentTimeSec(currentTime.unixtime(), rtcAvailable);
  wateringManager.setManualWateringDurationSec(MqttManager::inst.getManualWateringDurationSec());
  wateringManager.setStartManualWateringCmd(MqttManager::inst.getStartManualWateringCmd());
  wateringManager.setStopWateringCmd(MqttManager::inst.getStopWateringCmd());
  wateringManager.setDateTimeReferenceSec(MqttManager::inst.getDateTimeReferenceSec());
  wateringManager.setScheduledWateringDurationSec(MqttManager::inst.getScheduledWateringDurationSec());
  wateringManager.setScheduledWateringIntervalSec(MqttManager::inst.getScheduledWateringIntervalSec());
  wateringManager.setRainDelay(MqttManager::inst.getRainDelay());
  wateringManager.update();

  //update delayed until next loop when update is run
  MqttManager::inst.setPumpState(wateringManager.getPumpState()); 
  MqttManager::inst.setWateringTimeRemainingSec(wateringManager.getWateringTimeRemainingSec());
  MqttManager::inst.setTimeUntilScheduledWateringSec(wateringManager.getTimeUntilNextScheduledWateringSec());
  MqttManager::inst.setRainDelay(wateringManager.getRainDelay());
}