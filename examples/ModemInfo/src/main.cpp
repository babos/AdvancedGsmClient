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

#define LOOP_INTERVAL_MS 200
#define LOOP_MAX_MS 2500

// Access via the API
int32_t max_report = 0;
GsmModem& modem = modemDevice;
int32_t next_report = 0;

void setup() {
  SerialMon.begin(115200);
  delay(5000);
  SerialMon.print("Modem Info");

  SerialAT.begin(GSM_BAUDRATE, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);

  String manufacturer = modem.manufacturer();
  String model = modem.model();
  String revision = modem.revision();
  String imei = modem.IMEI();
  String imsi = modem.IMSI();
  String iccid = modem.ICCID();

  Serial.printf("Manufacturer: %s\n", manufacturer.c_str());
  Serial.printf("Model: %s\n", model.c_str());
  Serial.printf("Revision: %s\n", revision.c_str());
  Serial.printf("IMEI: %s\n", imei.c_str());
  Serial.printf("IMSI: %s\n", imsi.c_str());
  Serial.printf("ICCID: %s\n", iccid.c_str());

  modem.begin(apn);
}

void loop() {
  modem.loop();

  int32_t now = millis();
  if (max_report == 0) {
    max_report = now + LOOP_MAX_MS;
  }

  if (now > next_report && now < max_report) {
    next_report = now + LOOP_INTERVAL_MS;

    // Lifecycle (after radio enabled)
    // - Initially no signal (RSSI 0)
    // - Once get signal, registration status becomes 2 (= Searching)
    // - Once network found, registration status becomes 1, and the network operator details are available
    // - Once the packet data protocol connection is established, the dynamic parameters (+CGCONTRDP) are available

    int32_t rssidBm = modem.RSSI();

    RegistrationStatus registrationStatus = modem.registrationStatus();
    String network = modem.network();

    modem.sendATCommand("+CGCONTRDP");  // read dynamic parameters
    String line;
    for (int i = 0; i < 10; i++) {
      line = modem.readResponseLine();
      if (line.indexOf("OK") > -1 || line.indexOf("ERROR") > -1) {
        break;
      }
    }
    String local_ip1 = modem.localIP(1);

    Serial.printf("***** [%d] *****\n", now);
    Serial.printf("RSSI (dBm): %d\n", rssidBm);
    Serial.printf(
        "Registration status: %d%s\n",
        (registrationStatus, registrationStatus == RegisrationStatus::Searching)
            ? " (= Searching)"
        : (registrationStatus,
           registrationStatus == RegisrationStatus::RegisteredHome)
            ? " (= Home)"
            : "");
    Serial.printf("Operator: %s\n", network.c_str());
    Serial.printf("Local IP: %s\n", local_ip1.c_str());
  }
}