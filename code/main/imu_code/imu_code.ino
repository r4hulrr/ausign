#define FLEX_SENSOR_PIN 4  // GPIO4 = ADC1_CH3

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // Optional: 0–4095 range (12-bit)
}

void loop() {
  const int numSamples = 50;
  long total = 0;

  for (int i = 0; i < numSamples; i++) {
    total += analogRead(FLEX_SENSOR_PIN);
    delay(2); // Short delay for better averaging
  }

  int average = total / numSamples;
  Serial.println(average);

  delay(100); // Print ~10 values per second
}
