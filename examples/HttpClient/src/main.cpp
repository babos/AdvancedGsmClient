#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.print("Hello Serial\n(with Arduino framework)\nvia PlatformIO");
}

void loop() {
}