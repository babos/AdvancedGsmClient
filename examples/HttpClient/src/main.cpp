#define GSM_BAUDRATE 115200

#include "GsmModemSIM7020.h"
#include <Arduino.h>

GsmModemSIM7020 modem(Serial1);
//GsmModem modem = new GsmModemSIM7020(Serial1);

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.print("GsmHttpClient\n(with Arduino framework)\nvia PlatformIO");


}

void loop() {
}