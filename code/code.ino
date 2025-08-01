#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <driver/i2s.h>

// I2S audio pins
#define I2S_DOUT 9
#define I2S_BCLK 10
#define I2S_LRC  11

// FLEX SENSOR definitions
#define THUMB_FLEX_SENSOR_PIN 4
#define INDEX_FLEX_SENSOR_PIN 5
#define MIDDLE_FLEX_SENSOR_PIN 6
#define RING_FLEX_SENSOR_PIN 7
#define LITTLE_FLEX_SENSOR_PIN 8

#define FLEX 1

// BLE definitions
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// LCD definitions
#define I2C_ADDRESS 0x3C
#define RST_PIN -1

// Includes
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "SparkFunLSM6DS3.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// Global instances
MAX30105 particleSensor;
LSM6DS3 myIMU(I2C_MODE, 0x6A);
SSD1306AsciiWire oled(Wire1);
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
String lastReceivedSign = "";
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

// BLE server callback
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

// BLE characteristic write callback
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

// Initialize I2S
void initI2S() {
  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRC,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
}

// Play audio corresponding to lastReceivedSign
void playAudioFromFile(const String& name) {
  String path = "/" + name + ".raw";
  File f = LittleFS.open(path.c_str(), "r");
  if (!f) {
    Serial.print("Failed to open ");
    Serial.println(path);
    return;
  }

  Serial.print("Playing: ");
  Serial.println(path);

  uint8_t buffer[512];
  size_t bytes_written;
  while (f.available()) {
    size_t len = f.read(buffer, sizeof(buffer));
    i2s_write(I2S_NUM_0, buffer, len, &bytes_written, portMAX_DELAY);
  }

  f.close();
  Serial.println("Done playing.");
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  Wire.begin(12, 13);  // heart sensor

  // LCD
  Wire1.begin(21, 47);
  Wire1.setClock(400000L);
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(Arial_bold_14);
  oled.displayRemap(true);
  oled.clear();
  oled.println("\nAuslan");

  // LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    while (1);
  }

  // I2S
  initI2S();

  // Heart sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 not found!");
    while (1);
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

  // IMU
  if (myIMU.begin() != 0)
    Serial.println("IMU failed to initialize!");
  else
    Serial.println("IMU initialized.");

  // BLE
  BLEDevice::init("Auslan_glove");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE |
      BLECharacteristic::PROPERTY_NOTIFY);
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
  // Flex sensor
  const int numSamples = 50;
  long totals[5] = {0};
  for (int i = 0; i < numSamples; i++) {
    totals[0] += analogRead(LITTLE_FLEX_SENSOR_PIN);
    totals[1] += analogRead(RING_FLEX_SENSOR_PIN);
    totals[2] += analogRead(MIDDLE_FLEX_SENSOR_PIN);
    totals[3] += analogRead(INDEX_FLEX_SENSOR_PIN);
    totals[4] += analogRead(THUMB_FLEX_SENSOR_PIN);
    delay(2);
  }

  int avgs[5];
  for (int i = 0; i < 5; i++) avgs[i] = totals[i] / numSamples;

  bool flex[5] = {
    avgs[0] > 3250, avgs[1] > 3300, avgs[2] > 3150,
    avgs[3] > 3150, avgs[4] > 3350
  };

  uint8_t flex_byte = 0;
  for (int i = 0; i < 5; i++) flex_byte |= (flex[i] << i);

  // Heartbeat
  uint8_t heartbeat_high = 0;
  long irValue = particleSensor.getIR();
  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);
    if (beatsPerMinute > 20 && beatsPerMinute < 255) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;
      beatAvg = 0;
      for (byte i = 0; i < RATE_SIZE; i++) beatAvg += rates[i];
      beatAvg /= RATE_SIZE;
    }
  }
  if (beatsPerMinute > 100) heartbeat_high = 1;

  // IMU
  int16_t ax = myIMU.readFloatAccelX() * 1000;
  int16_t ay = myIMU.readFloatAccelY() * 1000;
  int16_t az = myIMU.readFloatAccelZ() * 1000;

  // BLE send
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

  // Sign received
  if (lastReceivedSign.length() > 0) {
    Serial.print("ESP32 Displayed Sign: ");
    Serial.println(lastReceivedSign);
    oled.clear();
    oled.println();
    oled.println(lastReceivedSign);
    playAudioFromFile(lastReceivedSign);  // 🔊 Play .raw audio
    lastReceivedSign = "";
  }

  delay(100);
}
