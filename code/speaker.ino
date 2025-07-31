#include <Arduino.h>
#include <Audio.h>
#include <FS.h>
#include <LittleFS.h>

#define I2S_DOUT 26
#define I2S_BCLK 27
#define I2S_LRC  25

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
  Serial.println("Playing /hello.wav...");
  if (LittleFS.exists("/hello.wav")) {
    audio.connecttoFS(LittleFS, "/hello.wav");
  } else {
    Serial.println("File not found: /hello.wav");
  }
}

void loop() {
  audio.loop(); // must be called repeatedly
}
