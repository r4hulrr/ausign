// Simple I2C test for ebay 128x64 OLED on ESP32 I2C1 (Wire1)

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

// Use the second I2C bus (I2C1) provided by the ESP32 core.
SSD1306AsciiWire oled(Wire1);

void setup() {
  // Set your SDA/SCL for I2C1 here (example: ESP32-S3 GPIO21/47)
  Wire1.begin(21, 47);          // SDA, SCL
  Wire1.setClock(400000L);      // 400 kHz fast mode

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif

  oled.setFont(Adafruit5x7);
  oled.clear();
  oled.println("Auslan G");
}

void loop() {}
