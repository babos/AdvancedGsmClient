#define GSM_BAUDRATE 115200

// Set serial for output console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
#define SerialAT Serial1

// Set serial for debug, if wanted
#define ADVANCED_GSM_DEBUG Serial;

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS

#include "../../../src/SIM7020/GsmModemSIM7020.h"

#include <Arduino.h>

// Allocate memory for concrete object
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
GsmModemSIM7020 sim7020(debugger);
#else
GsmModemSIM7020 sim7020(SerialAT);
#endif

// Access via the API
GsmModem& modem = sim7020;

void setup() {
  SerialMon.begin(115200);
  delay(5000);
  SerialMon.print("GsmHttpClient\n(with Arduino framework)\nvia PlatformIO\n");

  modem.begin();
  //modem.test();
  sim7020.test();

  String manufacturer = modem.manufacturer();
  Serial.printf("Manufacturer: %s\n", manufacturer);
}

void loop() {}