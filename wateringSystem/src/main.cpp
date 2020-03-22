#include <Arduino.h>

#include "wifimanager.h"

void setup() 
{
  Serial.begin(115200);

  delay(10);
}

void loop() 
{
  manageWifiConnection();
}