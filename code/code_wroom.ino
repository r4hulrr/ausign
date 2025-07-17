#include <Wire.h>
#include "MAX30105.h"

#include "heartRate.h"


#define THUMB_FLEX_SENSOR_PIN 34  // GPIO4 = ADC1_CH3
#define INDEX_FLEX_SENSOR_PIN 35  // GPIO4 = ADC1_CH3
#define MIDDLE_FLEX_SENSOR_PIN 32  // GPIO4 = ADC1_CH3
#define RING_FLEX_SENSOR_PIN 33  // GPIO4 = ADC1_CH3

#define FLEX 1

// param for hearbeat
MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

void setup() {
  Wire.begin(19,21);
  Serial.begin(115200);
  analogReadResolution(12); // Optional: 0–4095 range (12-bit)
  Serial.print("Started");
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void loop() {
  const int numSamples = 50;
  long thumb_total = 0;
  long index_total = 0;
  long middle_total = 0;
  long ring_total = 0;

  // 0 indicates straight;  1 indicates flex
  bool thumb = 0;
  bool index = 0;
  bool middle = 0;
  bool ring = 0;

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

  /*
  Serial.print(thumb_average);
  Serial.print(";");
  Serial.print(index_average);
  Serial.print(";");
  Serial.print(middle_average);
  Serial.print(";");
  Serial.println(ring_average);
  */
  
  // Tells if the finger is flexed or not
  
  if (thumb_average > 2800){
    Serial.print("Thumb flexed;");
  }else{
    Serial.print("Thumb not flexed;");
  };
  if (index_average > 3000){
    Serial.print("Index flexed;");
  }else{
    Serial.print("Index not flexed;");
  };
  if (middle_average > 3000){
    Serial.print("Middle flexed;");
  }else{
    Serial.print("Middle not flexed;");
  };
  if (ring_average > 3000){
    Serial.print("Ring flexed;");
  }else{
    Serial.print("Ring not flexed;");
  };
  
  // Logic for flex
  if (thumb_average > 3000){
    thumb = FLEX;
  };
  if (index_average > 3000){
    index = FLEX;
  };
  if (middle_average > 3000){
    middle = FLEX;
  };
  if (ring_average > 3000){
    ring = FLEX;
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

  // code for heartbeart
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  if (beatsPerMinute > 100){
    Serial.println("In Panic");
  }else if (irValue < 50000){
    Serial.println(" No finger?");
  } else{
    Serial.println("At rest");
  };
  
  delay(100); // Print ~10 values per second
}
