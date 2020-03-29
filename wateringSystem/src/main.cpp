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
  MqttManager::inst.setPumpState(wateringManager.getPumpState()); //delayed one loop
  MqttManager::inst.setWateringTimeRemainingSec(wateringManager.getWateringTimeRemainingSec()); //delayed one loop
  MqttManager::inst.update();

  wateringManager.setManualWateringDurationSec(MqttManager::inst.getManualWateringDurationSec());
  wateringManager.setStartManualWateringCmd(MqttManager::inst.getStartManualWateringCmd());
  wateringManager.setStopWateringCmd(MqttManager::inst.getStopWateringCmd());
  wateringManager.update();


}