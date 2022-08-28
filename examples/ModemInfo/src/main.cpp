/*
GSM modem info example

Build:       pio run -e m5stack-atom
Deploy:      pio run -e m5stack-atom -t upload
View result: pio device monitor --baud 115200
*/

/*
Log settings (set before including modem)
*/

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS
#define LOG_OUTPUT Serial

/*
Modem device
*/

#include "../../../src/SIM7020/SIM7020GsmModem.h"

#define TestModem SIM7020GsmModem
//#define TestModem SIM7080GsmModem


#define GSM_BAUDRATE 115200

/*
Board settings (also see the environment settings in platformio.ini)
*/

// Set serial for AT commands (to the module)
#ifndef SerialAT
#define SerialAT Serial1
#endif

// Set serial for output console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

/*
Sample code
*/

#include <Arduino.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TestModem testModem(debugger);
#else
TestModem testModem(SerialAT);
#endif

const char apn[] = "telstra.iot";
const PacketDataProtocolType pdp_type =
    PacketDataProtocolType::IPv4v6;  // default
// const PacketDataProtocolType pdp_type = PacketDataProtocolType::IP;

#define LOOP_INTERVAL_MS 200
#define LOOP_MAX_MS 2500

// Access via the API
int32_t max_report = 0;
GsmModem& modem = testModem;
int32_t next_report = 0;

void setup() {
#if ADVGSM_LOG_SEVERITY > 0
#ifdef LOG_OUTPUT
  AdvancedGsmLog.Log = &LOG_OUTPUT;
#endif
#endif

  SerialMon.begin(115200);
  delay(5000);
  SerialMon.print("Modem information\n");

  SerialAT.begin(GSM_BAUDRATE, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);

  modem.resetDefaultConfiguration();
  delay(100);
  modem.sendATCommand("E0"); // turn echo off
  while (modem.readResponseLine().indexOf("OK") < 0) {
    delay(100);
  }

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

  modem.begin(apn, pdp_type);
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
    // - Once network found, registration status becomes 1, and the network
    // operator details are available
    // - Once the packet data protocol connection is established, the dynamic
    // parameters (+CGCONTRDP) are available

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

    // Get non-link-local IP address
    String addresses[4];
    int8_t count = modem.getLocalIPs(addresses, 4);
    String ip_address = "";
    for (int8_t index = 0; index < count; index++) {
      // ignore IPv6 link local
      if (!addresses[index].startsWith("fe80:")) {
        ip_address = addresses[index];
        break;
      }
    }

    Serial.printf("***** [%d] *****\n", now);
    Serial.printf("RSSI (dBm): %d\n", rssidBm);
    Serial.printf("Registration status: %d%s\n", registrationStatus,
                  (registrationStatus == RegistrationStatus::Searching)
                      ? " (= Searching)"
                  : (registrationStatus,
                     registrationStatus == RegistrationStatus::RegisteredHome)
                      ? " (= Home)"
                      : "");
    Serial.printf("Operator: %s\n", network.c_str());
    Serial.printf("IP Address: %s\n", ip_address.c_str());
  }
}