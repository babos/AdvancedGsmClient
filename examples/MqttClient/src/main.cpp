/*
GSM MQTT client example

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
#include "../../../src/SIM7020/SIM7020MqttClient.h"

#define TestModem SIM7020GsmModem
#define TestTcpClient SIM7020TcpClient
#define TestMqttClient SIM7020MqttClient

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

//#define WAIT_FOR_NON_LOCAL_IPV6
#define SEND_INTERVAL_MS 10000
//#define USE_INSECURE_HTTP

const char apn[] = "telstra.iot";

// See https://test.mosquitto.org/
const char server[] = "test.mosquitto.org";
//const int16_t port = 1883; // unencrypted, unauthenticated
// const int16_t port = 1884; // unencrypted, authenticated
const int16_t port = 8886; // encrypted: Lets Encrypt, unauthenticated
// const int16_t port = 8887; // encrypted: certificate deliberately expired
const char client_id[] = "testclient";

// Root certificate for Let's Encrypt
const String root_ca = \
  "-----BEGIN CERTIFICATE-----\n" \
  "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\n" \
  "-----END CERTIFICATE-----\n";

#include <Arduino.h>

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

bool publish_done = false;

void setup() {
#ifdef ADVGSM_LOG_ENABLED
#ifdef LOG_OUTPUT
  AdvancedGsmLog.Log = &LOG_OUTPUT;
#endif
#endif
  SerialMon.begin(115200);
  delay(5000);
  SerialMon.printf("### MQTT client example started at %d\n", millis());

  SerialAT.begin(GSM_BAUDRATE, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);

  modem.begin(apn, PacketDataProtocolType::IP);
  //modem.begin(apn, IPv4v6);
  delay(100);
  SerialMon.print("Setup complete\n");
}

bool isReady() {
  // Get non-link-local IP address
  String addresses[4];
  int8_t count = modem.getLocalIPs(addresses, 4);
  for (int8_t index = 0; index < count; index++) {
#ifdef WAIT_FOR_NON_LOCAL_IPV6
    if (addresses[index].indexOf(":") > 0 &&
        !addresses[index].startsWith("fe80:")) {
      SerialMon.printf("### Ready with IPv6 address %s\n", addresses[index].c_str());
      return true;
    }
#else
    if (addresses[index] != "127.0.0.1") {
      SerialMon.printf("### Ready with address %s\n", addresses[index].c_str());
      return true;
    }
#endif
  }
  return false;
}

void connectedLoop() {
  modem.loop();
  int now = millis();
  if (now > next_message_ms) {
    if (!publish_done) {
      SerialMon.printf("Testing MQTT to: %s (%d)\n", server, port);

      bool use_tls = false;
      if (port == 8886) {
        modem.setRootCA(root_ca);
        use_tls = true;
      }

      TestTcpClient testTcpClient(testModem);
      TestMqttClient testMqttClient(testTcpClient, server, port, use_tls);
      MqttClient& mqtt = testMqttClient;

      int rc = mqtt.connect(client_id);
      if (rc != 0) {
        Serial.printf("MQTT connect error: %d\n", rc);
        return;
      }

      // Subscribe
      Serial.printf("Subscribing\n");
      mqtt.subscribe("advgsm/t");
      delay (200);

      // Publish
      Serial.printf("Publishing\n");
      mqtt.publish("advgsm/t", "TestMessage");

      // Wait for messages
      int finish = millis() + SEND_INTERVAL_MS;
      while (millis() < finish) {
        modem.loop();
        String topic = mqtt.receiveTopic();
        if (topic.length() > 0) {
          String body = mqtt.receiveBody();
          Serial.printf("Received [%s]: %s\n", topic.c_str(), body.c_str());
        }
        delay(200);
      }

      Serial.printf("Disconnecting\n");
      mqtt.disconnect();
      
      Serial.printf("Done\n");
      publish_done = true;
    }
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