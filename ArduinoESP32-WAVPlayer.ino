#include "SPIFFS.h"
#include "wavPlayer.h"

void setup()
{
  Serial.begin(115200);
  Serial.print("Initializing SPIFFS...");
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  Serial.println("initialization done.");
  delay(1000);
}

void loop()
{
  root = SPIFFS.open("/music.wav");
  playWAVData(root);
}
