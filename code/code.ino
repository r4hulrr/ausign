// FLEX SENSOR
#define THUMB_FLEX_SENSOR_PIN 4  // GPIO4 = ADC1_CH3
#define INDEX_FLEX_SENSOR_PIN 5  // GPIO4 = ADC1_CH3
#define MIDDLE_FLEX_SENSOR_PIN 6  // GPIO4 = ADC1_CH3
#define RING_FLEX_SENSOR_PIN 7  // GPIO4 = ADC1_CH3
#define LITTLE_FLEX_SENSOR_PIN 8  // GPIO4 = ADC1_CH3

#define FLEX 1

// HEART RATE
#include <Wire.h>
#include "MAX30105.h"

#include "heartRate.h"

MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

// IMU SENSOR
#include "SparkFunLSM6DS3.h"
#include "Wire.h"
#include "SPI.h"

LSM6DS3 myIMU(I2C_MODE, 0x6A); //Default constructor is I2C, addr 0x6B

// BLUETOOTH CODE
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
  analogReadResolution(12); // Optional: 0–4095 range (12-bit)
  Serial.print("Started");
  Wire.begin(12, 13);

  // HEART RATE CODE BEGINS
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
  // HEART RATE CODE ENDS

  // IMU CODE BEGIN
  if (myIMU.begin() != 0) {
    Serial.println("IMU failed to initialize!");
  } else {
    Serial.println("IMU initialized successfully.");
  }
  // IMU CODE ENDS

  // BLUETOOTH CODE BEGINS
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
  // BLUETOOTH CODE ENDS
}

void loop() {
  // FLEX SENSOR CODE STARTS HERE
  const int numSamples = 50;
  long thumb_total = 0;
  long index_total = 0;
  long middle_total = 0;
  long ring_total = 0;
  long little_total = 0;

  // 0 indicates straight;  1 indicates flex
  bool thumb = 0;
  bool index = 0;
  bool middle = 0;
  bool ring = 0;
  bool little = 0;

  for (int i = 0; i < numSamples; i++) {
    thumb_total += analogRead(THUMB_FLEX_SENSOR_PIN);
    index_total += analogRead(INDEX_FLEX_SENSOR_PIN);
    middle_total += analogRead(MIDDLE_FLEX_SENSOR_PIN);
    ring_total += analogRead(RING_FLEX_SENSOR_PIN);
    little_total += analogRead(LITTLE_FLEX_SENSOR_PIN);
    delay(2); // Short delay for better averaging
  }

  int thumb_average = thumb_total / numSamples;
  int index_average = index_total / numSamples;
  int middle_average = middle_total / numSamples;
  int ring_average = ring_total / numSamples;
  int little_average = little_total / numSamples;

  // Logic for flex
  if (thumb_average > 3300){
    thumb = FLEX;
  };
  if (index_average > 3300){
    index = FLEX;
  };
  if (middle_average > 3200){
    middle = FLEX;
  };
  if (ring_average > 3200){
    ring = FLEX;
  };
  if (little_average > 3100){
    little = FLEX;
  };
  // Pack finger flex states into 1 byte
  uint8_t flex_byte = 0;
  flex_byte |= (little  << 0);
  flex_byte |= (ring  << 1);
  flex_byte |= (middle << 2);
  flex_byte |= (index   << 3);
  flex_byte |= (thumb  << 4);
  // FLEX SENSOR CODE ENDS HERE

  // HEART RATE CODE BEGINS HERE

  uint8_t heartbeat_high = 0;

  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  if (beatsPerMinute > 100){
    heartbeat_high = 1;
  } else {
    heartbeat_high = 0;
  };

  // HEART RATE CODE ENDS HERE

  // IMU CODE BEGINS HERE
  //Get accelerometer parameters
  float ax_f = myIMU.readFloatAccelX();
  float ay_f = myIMU.readFloatAccelY();
  float az_f = myIMU.readFloatAccelZ();

  int16_t ax = (int16_t)(ax_f * 1000.0);  // 1.234 → 1234
  int16_t ay = (int16_t)(ay_f * 1000.0);
  int16_t az = (int16_t)(az_f * 1000.0);

  // IMU CODE ENDS HERE
  
  // BLUETOOTH CODE BEGINS

  // Pack everything into an 8-byte array
  uint8_t payload[8];
  memcpy(payload,     &ax, 2);
  memcpy(payload + 2, &ay, 2);
  memcpy(payload + 4, &az, 2);
  payload[6] = flex_byte;
  payload[7] = heartbeat_high;

  static int counter = 0;

  if (deviceConnected) {
    // Send notification
    pCharacteristic->setValue(payload, 8);
    pCharacteristic->notify();
  }
  // BLUETOOTH CODE ENDS

  delay(100); // Print ~10 values per second
}
