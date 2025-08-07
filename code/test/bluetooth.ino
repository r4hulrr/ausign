#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>  // Needed for notifications

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
    BLEDevice::startAdvertising();  // Re-advertise
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE server");

  // 1. Initialize BLE
  BLEDevice::init("Auslan_glove");

  // 2. Create server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // 3. Create service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // 4. Create characteristic with notify property
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  // 5. Add descriptor (needed for notify to work on most clients)
  pCharacteristic->addDescriptor(new BLE2902());

  // 6. Set initial value (optional)
  pCharacteristic->setValue("Hello from ESP32");

  // 7. Start service
  pService->start();

  // 8. Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // Fix for iOS
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE advertising started");
}

void loop() {
  static int counter = 0;

  if (deviceConnected) {
    // Prepare data
    String value = "Counter: " + String(counter++);
    Serial.println("Sending: " + value);

    // Send notification
    pCharacteristic->setValue(value.c_str());
    pCharacteristic->notify();
  }

  delay(1000);  // Send once every second
}
