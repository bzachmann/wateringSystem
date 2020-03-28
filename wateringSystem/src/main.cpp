#include <Arduino.h>

#include "wifimanager.h"
#include "mqttmanager.h"
#include "wateringmanager.h"

#include <countdowntimer.h>

static CountdownTimer printTimer;
static WateringManager wateringManager;

void setup() 
{
  Serial.begin(115200);
  delay(10);

  MqttManager::inst.init();

  printTimer.setDuration(1000);
  printTimer.start();
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


  printTimer.update();
  if(printTimer.isExpired())
  {
    printTimer.setDuration(1000);
    Serial.print("manualWateringTimeRemaining: ");
    Serial.println(wateringManager.getWateringTimeRemainingSec());
    Serial.print("manualWateringOn: ");
    Serial.println(wateringManager.getPumpState());
  }
}