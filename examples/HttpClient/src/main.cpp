#define GSM_BAUDRATE 115200
#define ADVANCED_GSM_DEBUG Serial;

#include "../../../src/SIM7020/GsmModemSIM7020.h"

#include <Arduino.h>

// Allocate memory for concrete object
GsmModemSIM7020 sim7020(Serial1);
// But access it via the abstract interface
GsmModem& modem = sim7020;

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.print("GsmHttpClient\n(with Arduino framework)\nvia PlatformIO\n");

  modem.begin();
  String manufacturer = modem.getManufacturer();
  Serial.printf("Manufacturer: %", manufacturer);
}

void loop() {}