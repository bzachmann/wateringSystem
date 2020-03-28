#include <Arduino.h>

#include "wifimanager.h"
#include "mqttmanager.h"

#include <countdowntimer.h>

static CountdownTimer printTimer;

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

  printTimer.update();
  if(printTimer.isExpired())
  {
    printTimer.setDuration(1000);
    Serial.print("manualWateringTimeRemaining: ");
    Serial.println(manualWateringTimeRemainingSec);
    Serial.print("manualWateringOn: ");
    Serial.println(manualWateringOn);
  }
}