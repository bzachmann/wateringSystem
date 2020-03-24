#include <Arduino.h>

#include "wifimanager.h"
#include "mqttmanager.h"

void setup() 
{
  Serial.begin(115200);
  delay(10);

  initMqttConnection();
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
}