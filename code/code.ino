// FLEX SENSOR definitions
#define THUMB_FLEX_SENSOR_PIN 4
#define INDEX_FLEX_SENSOR_PIN 5
#define MIDDLE_FLEX_SENSOR_PIN 6
#define RING_FLEX_SENSOR_PIN 7
#define LITTLE_FLEX_SENSOR_PIN 8

#define FLEX 1

// Bluetooth definitons
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// LCD address definition
#define I2C_ADDRESS 0x3C
#define RST_PIN -1

// HEART RATE includes
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <SPI.h>

// LCD Screen includes
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// IMU SENSOR includes
#include "SparkFunLSM6DS3.h"
#include "SPI.h"

// BLUETOOTH includes
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

MAX30105 particleSensor;
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;

float beatsPerMinute;
int beatAvg;

LSM6DS3 myIMU(I2C_MODE, 0x6A); // I2C, addr 0x6A

SSD1306AsciiWire oled(Wire1);

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
String lastReceivedSign = "";

// BLE server callbacks
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
  }
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
    BLEDevice::startAdvertising();
  }
};

// BLE write callback (receives detected sign from PC)
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) override {
    String rxValue = pChar->getValue();
    if (rxValue.length() > 0) {
      lastReceivedSign = rxValue;
      Serial.print("Received Sign from PC: ");
      Serial.println(lastReceivedSign);
    }
  }
};

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  Wire.begin(12, 13);  // SDA, SCL

  // LCD INIT
  Wire1.begin(21, 47);          // SDA, SCL
  Wire1.setClock(400000L);      // 400 kHz fast mode

  #if RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
  #else
    oled.begin(&Adafruit128x64, I2C_ADDRESS);
  #endif

  oled.setFont(Arial_bold_14);
  oled.displayRemap(true);
  oled.clear();
  oled.println();
  oled.println("Auslan");

  // HEART RATE INIT
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 not found!");
    while (1);
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

  // IMU INIT
  if (myIMU.begin() != 0) {
    Serial.println("IMU failed to initialize!");
  } else {
    Serial.println("IMU initialized.");
  }

  // BLE INIT
  BLEDevice::init("Auslan_glove");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
  pCharacteristic->setValue("Hello from ESP32");

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE advertising started");
}

void loop() {
  // --- FLEX SENSOR ---
  const int numSamples = 50;
  long thumb_total = 0, index_total = 0, middle_total = 0, ring_total = 0, little_total = 0;

  for (int i = 0; i < numSamples; i++) {
    thumb_total += analogRead(THUMB_FLEX_SENSOR_PIN);
    index_total += analogRead(INDEX_FLEX_SENSOR_PIN);
    middle_total += analogRead(MIDDLE_FLEX_SENSOR_PIN);
    ring_total += analogRead(RING_FLEX_SENSOR_PIN);
    little_total += analogRead(LITTLE_FLEX_SENSOR_PIN);
    delay(2);
  }

  int thumb_avg = thumb_total / numSamples;
  int index_avg = index_total / numSamples;
  int middle_avg = middle_total / numSamples;
  int ring_avg = ring_total / numSamples;
  int little_avg = little_total / numSamples;

  bool thumb = thumb_avg > 3300;
  bool index = index_avg > 3300;
  bool middle = middle_avg > 3200;
  bool ring = ring_avg > 3200;
  bool little = little_avg > 3000;

  uint8_t flex_byte = 0;
  flex_byte |= (little << 0);
  flex_byte |= (ring << 1);
  flex_byte |= (middle << 2);
  flex_byte |= (index << 3);
  flex_byte |= (thumb << 4);

  // --- HEART RATE ---
  uint8_t heartbeat_high = 0;
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      beatAvg = 0;
      for (byte i = 0; i < RATE_SIZE; i++) beatAvg += rates[i];
      beatAvg /= RATE_SIZE;
    }
  }
  if (beatsPerMinute > 100) heartbeat_high = 1;

  // --- IMU ---
  float ax_f = myIMU.readFloatAccelX();
  float ay_f = myIMU.readFloatAccelY();
  float az_f = myIMU.readFloatAccelZ();

  int16_t ax = (int16_t)(ax_f * 1000.0);
  int16_t ay = (int16_t)(ay_f * 1000.0);
  int16_t az = (int16_t)(az_f * 1000.0);

  // --- BLUETOOTH SEND ---
  uint8_t payload[8];
  memcpy(payload, &ax, 2);
  memcpy(payload + 2, &ay, 2);
  memcpy(payload + 4, &az, 2);
  payload[6] = flex_byte;
  payload[7] = heartbeat_high;

  if (deviceConnected) {
    pCharacteristic->setValue(payload, 8);
    pCharacteristic->notify();
  }

  // --- RECEIVED SIGN DISPLAY ---
  if (lastReceivedSign.length() > 0) {
    Serial.print("ESP32 Displayed Sign: ");
    Serial.println(lastReceivedSign);
    oled.clear();
    oled.println();
    oled.println(lastReceivedSign);
    
    lastReceivedSign = "";
  }

  delay(100);
}