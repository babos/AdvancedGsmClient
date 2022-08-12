#define GSM_BAUDRATE 115200

// Set serial for AT commands (to the module)
#ifndef SerialAT
#define SerialAT Serial1
#endif

// M5 Atom NB-IoT DTU, Serial1
//#define GSM_TX_PIN 22
//#define GSM_RX_PIN 19

// M5 Atom, PORT.A: GPIO26 I2C0_SDA, GPIO32 I2C0_SCL

// M5 StickC, PORT.A: GPIO32 I2C0_SDA, GPIO33 I2C0_SCL

// M5 Stack Core2, PORT.A (Red): GPIO32 I2C0_SDA, GPIO33 I2C0_SCL
// M5 Stack Core2, PORT.B (Black): GPIO26 DAC2, GPIO36 ADC1_CH0

// M5 Stack Core2, PORT.C (Blue), Serial2: GPIO14 TXD2, GPIO13 RXD2
//#define GSM_TX_PIN 14
//#define GSM_RX_PIN 13

// M5 Stack, PORT.A: GPIO21 I2C0_SDA, GPIO22 I2C0_SCL
// M5 Stack Fire, PORT.B: GPIO26 DAC2, GPIO36 ADC1_CH0

// M5 Stack, PORT.C: GPIO16 RXD2, GPIO17 TXD2

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
  SerialMon.print("GsmHttpClient\n(with Arduino framework)\nvia PlatformIO\n");

  SerialAT.begin(GSM_BAUDRATE, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);
  modem.begin(apn);
  //modem.test();
  modemDevice.test();

  String manufacturer = modem.manufacturer();
  Serial.printf("Manufacturer: %s\n", manufacturer.c_str());

  modem.sendATCommand(GF("I"));
  for (size_t i = 0; i < 3; i++) {
    String line = modem.readResponseLine();
    Serial.println(line);
  }
}

void loop() {}