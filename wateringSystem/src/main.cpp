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

  initMqttConnection();

  printTimer.setDuration(1000);
  printTimer.start();
}

void loop() 
{
  bool wifiConnected = manageWifiConnection();
  bool mqttConnected = false;

  if(wifiConnected)
  {
    mqttConnected = manageMqttConnection();
  }

  runMqttProcess();

  wateringManager.setManualWateringDurationSec(getManualWateringDurationSec());
  wateringManager.setStartManualWateringCmd(getStartManualWateringCmd());
  wateringManager.setStopWateringCmd(getStopWateringCmd());
  wateringManager.update();


  printTimer.update();
  if(mqttConnected && printTimer.isExpired())
  {
    printTimer.setDuration(1000);
    Serial.print("manualWateringTimeRemaining: ");
    Serial.println(wateringManager.getWateringTimeRemainingSec());
    Serial.print("manualWateringOn: ");
    Serial.println(wateringManager.getPumpState());
  }
}