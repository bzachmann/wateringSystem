#include "wifimanager.h"

#include <Arduino.h>
#include <WiFi.h>

#include "privateinfo.h"

bool manageWifiConnection()
{ 
  bool retVal = false;
  
  static bool displayedIp = false;

  if(WiFi.status() != WL_CONNECTED)
  {
    displayedIp = false;
    
    static uint32_t time = millis() + 6000;
    uint32_t newTime = millis();
  
    if(   (time > newTime) //rollover
       || ((newTime - time) > 5000))
    {
      time = newTime;
      Serial.println("");
      Serial.println("Connecting wifi");
      WiFi.disconnect();
      WiFi.begin(wifi_ssid, wifi_password);
    }

    static uint8_t status = 50;
    uint8_t newStatus = WiFi.status();
  
    if(newStatus != status)
    {
      status = newStatus;
      Serial.print(newStatus);
    }
  }
  else
  {    
    retVal = true;
    
    if(!displayedIp)
    {
      displayedIp = true;

      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }
  }

  return retVal;
}