// Default Arduino Libraries
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// Libraries to download from Arduino IDE
#include <RTClib.h>
#include <Adafruit_MAX31855.h>
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DS3TRC.h> // use this for the breakout board
#include <Adafruit_LSM6DSOX.h> // use this for the actual IC

// Constants
#define MAX_READS 2000
#define STORE_ONBOARD_DELAY 5 // In sec
#define PRINT_SAMPLE_DELAY 5 // In sec
#define THERMAL_DELAY 20  // In milliseconds
#define REQUEST_DELAY 100  // In milliseconds
#define OUTPUT_REPEAT_DELAY 15 // In sec
#define SD_OUTPUT_DELAY 1 // In sec
#define SERIAL_SPEED 115200
#define MIN_TEMP -10 
#define MAX_TEMP 10
#define MIN_OUTPUT_REQUESTS 5
#define NUM_BME 4
#define NUM_IMU 2
#define NUM_I2C 2
#define NUM_DIM 3
#define RTC_LINE 0
#define SD_DET_PIN 4
#define SPI1_SO 12
#define SPI1_CSN 9
#define SPI1_SCK 14
#define SPI1_SI 15
#define SPI0_SO 16
#define SPI0_CSN 17
#define SPI0_SCK 18
#define I2C0_SDA 20
#define I2C0_SCL 21 
#define I2C1_SDA 26
#define I2C1_SCL 27 
#define CSN_SD2  13// Select for SD cards, GP22
#define OFFLOAD_TRIGGER 0 // Jumper cable GP12 (Pin 16) to GP13 (Pin 17) to request data offload
#define HEATER_PIN 22 // GP22 (Pin 29)

// Initialise I2C lines
TwoWire TwoWire1 = TwoWire(i2c0, I2C0_SDA, I2C0_SCL);
TwoWire TwoWire2 = TwoWire(i2c1, I2C1_SDA, I2C1_SCL);
TwoWire* Wires[NUM_I2C] = {&TwoWire1 , &TwoWire2};


// Initialise IMUs
Adafruit_LSM6DS3TRC imu[NUM_IMU];
Adafruit_LSM6DSOX imu[NUM_IMU];


TPHReading tphData[NUM_BME][MAX_READS];     // Memory size is NUM_BME * MAX_READS * 3 * 32
AGReading agData[NUM_IMU][MAX_READS];       // Memory size is NUM_IMU * MAX_READS * 6 * 32
double tcData[MAX_READS];                   // Memory size is MAX_READS * 64
DateTime timestamp[MAX_READS];              // Memory size is MAX_READS * 32    -> total is 21 * MAX_READS * 32 bits = 21 * MAX_READS * 4 bytes = 168 kB (for 2000 reads)
bool heaterStatus[MAX_READS];               // Memory size is MAX_READS


void i2cIMUattach() {
  for (int i = 0; i < NUM_IMU; i++) {
    Serial.print(String("Attempt IMU ") + i + String(" ------- "));

    if (!imu[i].begin_I2C((uint8_t)LSM6DS_I2CADDR_DEFAULT, Wires[i])) {
      Serial.println(String("IMU ") + i + String(" not found"));
      successful_setup = false;
    } else {
      Serial.println(String("IMU ") + i + String(" found"));
    }

    imu[i].setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
    imu[i].setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
    //imu[i].setAccelDataRate(LSM6DS_RATE_12_5_HZ);
    //imu[i].setGyroDataRate(LSM6DS_RATE_12_5_HZ);
  }
}







AGReading getIMUData(int numIMU) {
  sensors_event_t accel_event, gyro_event, temp_event;

  imu[numIMU].getEvent(&(accel_event), &(gyro_event), &(temp_event));

  sensors_vec_t currAcc = accel_event.acceleration;
  sensors_vec_t currGyro = gyro_event.gyro;

  AGReading IMUData = {};

  IMUData.acc[0] = currAcc.x;
  IMUData.acc[1] = currAcc.y;
  IMUData.acc[2] = currAcc.z;
  IMUData.gyro[0] = currGyro.x;
  IMUData.gyro[1] = currGyro.y;
  IMUData.gyro[2] = currGyro.z;

  return IMUData;
}


void printCurrentDataEntry() {
  if (elapsed(millis(), &last_print, PRINT_SAMPLE_DELAY)) {

    for (int i = 0; i < NUM_IMU; i++) {
      Serial.println(String("IMU ") + i + String(" :"));
      Serial.print(agData[i][num_read].acc[0] + String(","));
      Serial.print(agData[i][num_read].acc[1] + String(","));
      Serial.println(agData[i][num_read].acc[2]);
      Serial.print(agData[i][num_read].gyro[0] + String(","));
      Serial.print(agData[i][num_read].gyro[1] + String(","));
      Serial.println(agData[i][num_read].gyro[2]);
    }

    Serial.println(String("Thermo: ") + tcData[num_read]);
    Serial.println(String("Time: ") + String(timestamp[num_read].hour()) + String(":") + String(timestamp[num_read].minute()) + String(":") + String(timestamp[num_read].second()));
  }
}

void outputData() {
  // Output data to serial in CSV format
  Serial.println("Onboard data Formatted as:");
  Serial.println("numread,unixtime,internaltemp,sensornum,temp,pressure,humidity,heaterStatus,accelerationx,accelerationy,accelerationz,gyrox,gyroy,gyroz");
  for (int i = 0; i < num_read; i++) {
    for (int j = 0; j < NUM_BME; j++) {

      Serial.print(agData[j][i].acc[0] + String(","));
      Serial.print(agData[j][i].acc[1] + String(","));
      Serial.print(agData[j][i].acc[2] + String(","));
      Serial.print(agData[j][i].gyro[0] + String(","));
      Serial.print(agData[j][i].gyro[1] + String(","));
      Serial.print(agData[j][i].gyro[2] + String(","));
    }
  }


void setup() {
  Serial.begin(SERIAL_SPEED);
  delay(7500);

  Wires[0]->begin();  // Initialize I2C0
  Wires[1]->begin();  // Initialize I2C1
}


void loop() {
 
}  
