#include <Arduino.h>

#include "wifimanager.h"
#include "mqttmanager.h"
#include "wateringmanager.h"

#include <countdowntimer.h>

static WateringManager wateringManager;

void setup() 
{
  Serial.begin(115200);
  delay(10);

  MqttManager::inst.init();
}

void loop() 
{
  WifiManager::inst.update();
  
  MqttManager::inst.setWifiConnected(WifiManager::inst.isConnected());
  MqttManager::inst.update();

  wateringManager.setManualWateringDurationSec(MqttManager::inst.getManualWateringDurationSec());
  wateringManager.setStartManualWateringCmd(MqttManager::inst.getStartManualWateringCmd());
  wateringManager.setStopWateringCmd(MqttManager::inst.getStopWateringCmd());
  wateringManager.update();

  //update delayed until next loop when update is run
  MqttManager::inst.setPumpState(wateringManager.getPumpState()); 
  MqttManager::inst.setWateringTimeRemainingSec(wateringManager.getWateringTimeRemainingSec());
  MqttManager::inst.setTimeUntilScheduledWateringSec(87456); //todo this is for testing
}