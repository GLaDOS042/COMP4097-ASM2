#include "tally.h"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

TallyPage* TallyPage::instance = nullptr;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        TallyPage::handleConnect(pServer);
    }

    void onDisconnect(BLEServer* pServer) {
        TallyPage::handleDisconnect(pServer);
    }
};

TallyPage::TallyPage() : pServer(nullptr), pCharacteristic(nullptr), 
    pBLE2902(nullptr), deviceConnected(false), count(0) {
    instance = this;
}

void TallyPage::init() {
    BLEDevice::init("M5Tally");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_INDICATE
    );
    
    pBLE2902 = new BLE2902();
    pBLE2902->setNotifications(true);
    pCharacteristic->addDescriptor(pBLE2902);
    
    updateCharacteristicValue();
    pService->start();
    
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->stop();
    
    BLEAdvertisementData advertisementData;
    advertisementData.setFlags(0x06);
    advertisementData.setCompleteServices(BLEUUID(SERVICE_UUID));
    advertisementData.setName("M5Tally");
    
    BLEAdvertisementData scanResponseData;
    scanResponseData.setName("M5Tally");
    
    pAdvertising->setAdvertisementData(advertisementData);
    pAdvertising->setScanResponseData(scanResponseData);
    
    pAdvertising->setMinInterval(0x20);
    pAdvertising->setMaxInterval(0x40);
    
    BLEDevice::startAdvertising();
    
    updateDisplay();
}

void TallyPage::updateCharacteristicValue() {
    uint8_t bytes[2];  
    bytes[0] = (count >> 8) & 0xFF; 
    bytes[1] = count & 0xFF;         
    
    pCharacteristic->setValue(bytes, 2);
    if (deviceConnected) {
        pCharacteristic->notify();
    }
}

void TallyPage::updateDisplay() {
    M5.Lcd.fillScreen(BLACK);
    
    if (deviceConnected) {
        M5.Lcd.fillRect(0, 0, 135, 10, GREEN);
    } else {
        M5.Lcd.fillRect(0, 0, 135, 10, RED);
    }
    
    M5.Lcd.setTextSize(4);
    M5.Lcd.setTextColor(WHITE);
    
    String countStr = String(count);
    int textWidth = countStr.length() * 24;
    int xPos = (M5.Lcd.width() - textWidth) / 2;
    int yPos = (M5.Lcd.height() - 32) / 2;
    
    M5.Lcd.setCursor(xPos, yPos);
    M5.Lcd.print(countStr);
}

void TallyPage::handleButtonA() {
    count++;
    updateCharacteristicValue();
    updateDisplay();
}

void TallyPage::handleButtonB() {
    count = 0;
    updateCharacteristicValue();
    updateDisplay();
}

void TallyPage::update() {
    if (M5.BtnA.wasPressed()) {
        handleButtonA();
    }
    if (M5.BtnB.wasPressed()) {
        handleButtonB();
    }
}

void TallyPage::handleConnect(BLEServer* pServer) {
    if (instance) {
        instance->deviceConnected = true;
        instance->pBLE2902->setNotifications(true);
        instance->updateDisplay();
    }
}

void TallyPage::handleDisconnect(BLEServer* pServer) {
    if (instance) {
        instance->deviceConnected = false;
        instance->pBLE2902->setNotifications(false);
        BLEDevice::startAdvertising();
        instance->updateDisplay();
    }
} 