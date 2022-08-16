/*
GSM HTTP client example

Build:       pio run -e m5stack-atom
Deploy:      pio run -e m5stack-atom -t upload
View result: pio device monitor --baud 115200
*/

/*
Log settings (set before including modem)
*/

#define ADVGSM_LOG_DESTINATION Serial
#define ADVGSM_LOG_LEVEL 4

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS

/*
Modem device
*/

#include "../../../src/SIM7020/SIM7020GsmModem.h"
#include "../../../src/SIM7020/SIM7020HttpClient.h"

#define TestModem SIM7020GsmModem
#define TestTcpClient SIM7020TcpClient
#define TestHttpClient SIM7020HttpClient

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

const char apn[] = "telstra.iot";
const char server[] = "v4v6.ipv6-test.com";


#include <Arduino.h>

//#define WAIT_FOR_NON_LOCAL_IPV6
#define SEND_INTERVAL_MS 5000

// Allocate memory for concrete object
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TestModem testModem(debugger);
#else
TestModem testModem(SerialAT);
#endif

int next_message_ms = 0;
// Access via the API
GsmModem& modem = testModem;
bool ready = false;

void setup() {
  SerialMon.begin(115200);
  delay(5000);
  SerialMon.print("HTTP client example\n");
  ADVGSM_LOGI("MAIN", "Start");
  ADVGSM_LOGI("MAIN", "at %d", millis());

  SerialAT.begin(GSM_BAUDRATE, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);

  modem.begin(apn);
}

bool isReady() {
  // Get non-link-local IP address
  String addresses[4];
  int8_t count = modem.getLocalIPs(addresses, 4);
  for (int8_t index = 0; index < count; index++) {
#ifdef WAIT_FOR_NON_LOCAL_IPV6
    if (addresses[index].indexOf(":") > 0 &&
        !addresses[index].startsWith("fe80:")) {
      return true;
    }
#else
    if (addresses[index] != "127.0.0.1") {
      return true;
    }
#endif
  }
  return false;
}

void connectedLoop() {
  int now = millis();
  if (now > next_message_ms) {
    next_message_ms = now + SEND_INTERVAL_MS;

    TestTcpClient testTcpClient(testModem);
    //Client& client = testTcpClient;

    TestHttpClient testHttpClient(testTcpClient, server, 80);
    HttpClient& http = testHttpClient;

    int rc = http.get("/api/myip.php");
    if (rc != 0) {
      Serial.printf("HTTP GET error: %d", rc);
    } else {
      int httpCode = http.responseStatusCode();
      if (httpCode != 200 && httpCode != 301) {
        Serial.printf("HTTP response code error: %d", httpCode);
      } else {
        Serial.printf("HTTP response code: ", httpCode);
        String payload = http.responseBody();

        Serial.print("##### PAYLOAD:\n");
        Serial.printf("%s", payload.c_str());
      }
    }

    http.stop();
  }
}

void loop() {
  if (!ready) {
    ready = isReady();
  }
  if (ready) {
    connectedLoop();
  }
  delay(1000);
}