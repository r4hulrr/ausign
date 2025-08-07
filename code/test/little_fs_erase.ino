#include "FS.h"
#include "LittleFS.h"

void setup() {
  Serial.begin(115200);
  
  if (!LittleFS.begin(true)) {  // `true` here forces format if mounting fails
    Serial.println("LittleFS Mount Failed");
    return;
  }

  // Format filesystem explicitly
  LittleFS.format();  

  Serial.println("LittleFS formatted successfully.");
}
