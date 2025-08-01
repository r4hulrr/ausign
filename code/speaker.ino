#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <driver/i2s.h>

#define I2S_DOUT 9
#define I2S_BCLK 10
#define I2S_LRC  11

void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    while (1);
  }

  File f = LittleFS.open("/hello.raw", "r");
  if (!f) {
    Serial.println("Failed to open /hello.raw");
    return;
  }

  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,  // stereo
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

  Serial.println("Streaming loud_audio.raw via I2S...");

  uint8_t buffer[512];
  size_t bytes_written;
  while (f.available()) {
    size_t len = f.read(buffer, sizeof(buffer));
    i2s_write(I2S_NUM_0, buffer, len, &bytes_written, portMAX_DELAY);
  }

  f.close();
  Serial.println("Done.");
}

void loop() {}
