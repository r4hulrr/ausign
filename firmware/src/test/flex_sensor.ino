
#define THUMB_FLEX_SENSOR_PIN 4  // GPIO4 = ADC1_CH3
#define INDEX_FLEX_SENSOR_PIN 5  // GPIO4 = ADC1_CH3
#define MIDDLE_FLEX_SENSOR_PIN 6  // GPIO4 = ADC1_CH3
#define RING_FLEX_SENSOR_PIN 7  // GPIO4 = ADC1_CH3
#define LITTLE_FLEX_SENSOR_PIN 8  // GPIO4 = ADC1_CH3

#define FLEX 1
//3600
//3600
//3400
//3400
//3300

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // Optional: 0–4095 range (12-bit)
  Serial.print("Started");
}

void loop() {
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

  /*
  Serial.print(thumb_average);
  Serial.print(";");
  Serial.print(index_average);
  Serial.print(";");
  Serial.print(middle_average);
  Serial.print(";");
  Serial.print(ring_average);
  Serial.print(";");
  Serial.println(little_average);
  */
  
  // Tells if the finger is flexed or not
  /*
  if (thumb_average > 3500){
    Serial.print("Thumb flexed;");
  }else{
    Serial.print("Thumb not flexed;");
  };
  if (index_average > 3500){
    Serial.print("Index flexed;");
  }else{
    Serial.print("Index not flexed;");
  };
  if (middle_average > 3400){
    Serial.print("Middle flexed;");
  }else{
    Serial.print("Middle not flexed;");
  };
  if (ring_average > 3400){
    Serial.println("Ring flexed");
  }else{
    Serial.println("Ring not flexed");
  };
  */
  
  // Logic for flex
  if (thumb_average > 3500){
    thumb = FLEX;
  };
  if (index_average > 3500){
    index = FLEX;
  };
  if (middle_average > 3400){
    middle = FLEX;
  };
  if (ring_average > 3400){
    ring = FLEX;
  };
  if (little_average > 3300){
    little = FLEX;
  };
  /*
  // Some basic interpreations
  if ( !thumb & index & !middle & !ring ){
    Serial.println("1");
  };
  if ( !thumb & index & middle & !ring ){
    Serial.println("2");
  };
  if ( !thumb & index & middle & ring ){
    Serial.println("3");
  };
  */
  delay(100); // Print ~10 values per second
}
