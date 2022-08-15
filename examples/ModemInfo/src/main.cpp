#define GSM_BAUDRATE 115200

// Set serial for AT commands (to the module)
#ifndef SerialAT
#define SerialAT Serial1
#endif

// Set serial for output console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for debug, if wanted
#define ADVANCED_GSM_DEBUG Serial;

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS

#include "../../../src/SIM7020/SIM7020GsmModem.h"

#include <Arduino.h>

#define ModemDevice SIM7020GsmModem
//#define ModemDevice SIM7080GsmModem

// Allocate memory for concrete object
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
ModemDevice modemDevice(debugger);
#else
ModemDevice modemDevice(SerialAT);
#endif

const char apn[] = "telstra.iot";

// Access via the API
GsmModem& modem = modemDevice;

void setup() {
  SerialMon.begin(115200);
  delay(5000);
  SerialMon.print("Modem Info");

  SerialAT.begin(GSM_BAUDRATE, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);

  String manufacturer = modem.manufacturer();
  String model = modem.model();

  Serial.printf("Manufacturer: %s\n", manufacturer.c_str());
  Serial.printf("Model: %s\n", model.c_str());
}

void loop() {


}