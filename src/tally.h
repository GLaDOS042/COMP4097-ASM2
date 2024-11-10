#pragma once

#include <M5StickCPlus2.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class MyServerCallbacks;

class TallyPage {
public:
    TallyPage();
    void init();
    void update();
    void handleButtonA();
    void handleButtonB();
    void updateDisplay();

private:
    void updateCharacteristicValue();
    
    BLEServer *pServer;
    BLECharacteristic *pCharacteristic;
    BLE2902 *pBLE2902;
    bool deviceConnected;
    uint16_t count;
    
    static void handleConnect(BLEServer* pServer);
    static void handleDisconnect(BLEServer* pServer);
    
    static TallyPage* instance;

    friend class MyServerCallbacks;
}; 