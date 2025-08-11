#include <Arduino.h>
#include <Audio.h>
#include <FS.h>
#include <LittleFS.h>

#define I2S_DOUT 9 //SD
#define I2S_BCLK 10 //SCK
#define I2S_LRC  11 //WS

Audio audio;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Mount LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    while (true); // halt
  }

  // Setup I2S and audio
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(10); // 0 (min) to 21 (max)

  // Play "hello.wav" from flash
  Serial.println("Playing /one.wav...");
  if (LittleFS.exists("/one.wav")) {
    audio.connecttoFS(LittleFS, "/one.wav");
  } else {
    Serial.println("File not found: /one.wav");
  }
}

void loop() {
  audio.loop(); // must be called repeatedly
}
