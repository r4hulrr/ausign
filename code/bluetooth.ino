#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  }

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("GloveBLE");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService("12345678-1234-5678-1234-56789abcdef0");

  pCharacteristic = pService->createCharacteristic(
    "abcdefab-1234-5678-1234-56789abcdef1",
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("BLE Glove is advertising...");
}

void loop() {
  if (deviceConnected) {
    pCharacteristic->setValue("A");  // Replace with actual gesture
    pCharacteristic->notify();
    delay(2000);  // Send every 2 sec for testing
  }
}
