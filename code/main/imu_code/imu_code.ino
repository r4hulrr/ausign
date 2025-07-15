
#define THUMB_FLEX_SENSOR_PIN 4  // GPIO4 = ADC1_CH3
#define INDEX_FLEX_SENSOR_PIN 6  // GPIO4 = ADC1_CH3
#define MIDDLE_FLEX_SENSOR_PIN 15  // GPIO4 = ADC1_CH3
#define RING_FLEX_SENSOR_PIN 17  // GPIO4 = ADC1_CH3

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // Optional: 0–4095 range (12-bit)
}

void loop() {
  const int numSamples = 50;
  long thumb_total = 0;
  long index_total = 0;
  long middle_total = 0;
  long ring_total = 0;

  for (int i = 0; i < numSamples; i++) {
    thumb_total += analogRead(THUMB_FLEX_SENSOR_PIN);
    index_total += analogRead(INDEX_FLEX_SENSOR_PIN);
    middle_total += analogRead(MIDDLE_FLEX_SENSOR_PIN);
    ring_total += analogRead(RING_FLEX_SENSOR_PIN);
    delay(2); // Short delay for better averaging
  }

  int thumb_average = thumb_total / numSamples;
  int index_average = index_total / numSamples;
  int middle_average = middle_total / numSamples;
  int ring_average = ring_total / numSamples;
  Serial.print("Thumb = ");
  Serial.print(thumb_average);
  Serial.print(";");
  Serial.print("Index = ");
  Serial.print(index_average);
  Serial.print(";");
  Serial.print("Middle = ");
  Serial.print(middle_average);
  Serial.print(";");
  Serial.print("Ring = ");
  Serial.println(ring_average);

  delay(100); // Print ~10 values per second
}
