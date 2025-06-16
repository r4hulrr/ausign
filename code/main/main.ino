#define FLEX_PIN 34  // Use a valid ADC pin (GPIO34 is input-only and great for ADC)

// Setup once
void setup() {
  Serial.begin(115200);       // Start serial monitor
  analogReadResolution(12);   // ESP32 ADC is 12-bit (0–4095)
}

// Loop continuously
void loop() {
  int flexValue = analogRead(FLEX_PIN);  // Read raw ADC value
  Serial.print("Flex ADC Value: ");
  Serial.println(flexValue);             // Print it

  delay(200); // Wait 200 ms between reads
}
