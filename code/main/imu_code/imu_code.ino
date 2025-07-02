// Default Arduino Libraries
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_LSM6DS3TRC.h> // IMU breakout board
#include <BluetoothSerial.h>

// Constants
#define MAX_READS 2000
#define SERIAL_SPEED 115200
#define NUM_IMU 1
#define NUM_DIM 3
#define I2C_SDA 21
#define I2C_SCL 22

// Struct to hold IMU data
typedef struct {
  float acc[3];
  float gyro[3];
} AGReading;

// Global variables
Adafruit_LSM6DS3TRC imu;
AGReading agData[MAX_READS];
int num_read = 0;
unsigned long last_print = 0;

// Bluetooth
BluetoothSerial SerialBT;

// Helper function to check if interval elapsed
bool elapsed(unsigned long now, unsigned long* last, int interval_sec) {
  if ((now - *last) > interval_sec * 1000UL) {
    *last = now;
    return true;
  }
  return false;
}

// IMU initialization
bool i2cIMUattach() {
  Serial.print("Attempting IMU init... ");
  if (!imu.begin_I2C(LSM6DS_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("IMU not found.");
    return false;
  } else {
    Serial.println("IMU found!");
    imu.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
    imu.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
    return true;
  }
}

// IMU data acquisition
AGReading getIMUData() {
  sensors_event_t accel_event, gyro_event, temp_event;
  imu.getEvent(&accel_event, &gyro_event, &temp_event);

  AGReading data;
  data.acc[0] = accel_event.acceleration.x;
  data.acc[1] = accel_event.acceleration.y;
  data.acc[2] = accel_event.acceleration.z;
  data.gyro[0] = gyro_event.gyro.x;
  data.gyro[1] = gyro_event.gyro.y;
  data.gyro[2] = gyro_event.gyro.z;

  return data;
}

// Print current sample to Serial and Bluetooth
void printCurrentDataEntry() {
  if (elapsed(millis(), &last_print, 2)) {
    AGReading& d = agData[num_read];

    String output = "IMU: ";
    output += "Acc = [" + String(d.acc[0]) + ", " + String(d.acc[1]) + ", " + String(d.acc[2]) + "] ";
    output += "Gyro = [" + String(d.gyro[0]) + ", " + String(d.gyro[1]) + ", " + String(d.gyro[2]) + "]";

    Serial.println(output);
    SerialBT.println(output);
  }
}

void setup() {
  Serial.begin(SERIAL_SPEED);
  SerialBT.begin("ESP32_IMU");  // Bluetooth SPP name
  Wire.begin(I2C_SDA, I2C_SCL);

  delay(2000);

  if (!i2cIMUattach()) {
    while (1) {
      Serial.println("IMU not responding.");
      delay(1000);
    }
  }
}

void loop() {
  if (num_read < MAX_READS) {
    agData[num_read] = getIMUData();
    printCurrentDataEntry();
    num_read++;
    delay(500);  // Adjust sample rate
  }
}
