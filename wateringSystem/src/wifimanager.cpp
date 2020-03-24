#include "wifimanager.h"

#include <Arduino.h>
#include <WiFi.h>

#include "privateinfo.h"

//********* Static Function Prototypes *********//
static String statusToStr(uint8_t status);


//********* Public Functions *********//
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
      Serial.print("Wifi status: ");
      Serial.println(statusToStr(status));
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

//********* Static Functions *********//
static String statusToStr(uint8_t status)
{
  String retVal = "";

  switch(status)
  {
    case static_cast<uint8_t>(WL_NO_SHIELD):
      retVal = "WL_NO_SHIELD";
      break;
    case static_cast<uint8_t>(WL_IDLE_STATUS):
      retVal = "WL_IDLE_STATUS";
      break;
    case static_cast<uint8_t>(WL_NO_SSID_AVAIL):
      retVal = "WL_NO_SSID_AVAIL";
      break;
    case static_cast<uint8_t>(WL_SCAN_COMPLETED):
      retVal = "WL_SCAN_COMPLETED";
      break;  
    case static_cast<uint8_t>(WL_CONNECTED):
      retVal = "WL_CONNECTED";
      break; 
    case static_cast<uint8_t>(WL_CONNECT_FAILED):
      retVal = "WL_CONNECT_FAILED";
      break; 
    case static_cast<uint8_t>(WL_CONNECTION_LOST):
      retVal = "WL_CONNECTION_LOST";
      break; 
    case static_cast<uint8_t>(WL_DISCONNECTED):
      retVal = "WL_DISCONNECTED";
      break; 
    default:
      retVal = "UNKNOWN";
      break;
  }

  return retVal;
}