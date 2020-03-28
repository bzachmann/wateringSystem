#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>

class WifiManager
{
public:
    WifiManager();

    void update();
    bool isConnected();

public:
    static WifiManager inst;

private:
    static String statusToStr(uint8_t status);

private:
    bool connected;

};

#endif /* WIFIMANAGER_H */