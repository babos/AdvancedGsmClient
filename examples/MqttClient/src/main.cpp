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
#define SEND_INTERVAL_MS 5000
//#define USE_INSECURE_HTTP

const char apn[] = "telstra.iot";
//const PacketDataProtocolType pdp_type = PacketDataProtocolType::IPv6;
const PacketDataProtocolType pdp_type = PacketDataProtocolType::IPv4v6;

// See https://test.mosquitto.org/
const char server[] = "test.mosquitto.org";
const char client_id[] = "testclient";
const char user_name[] = "rw";
const char password[] = "readwrite";
const char publish_topic[] = "dt/advgsm/demo/rw/txt";
const char subscribe_topic[] = "cmd/advgsm/demo/rw/#";

//const int16_t port = 1883;  // unencrypted, unauthenticated
//const int16_t port = 1884; // unencrypted, authenticated
const int16_t port = 8886; // encrypted: Lets Encrypt, unauthenticated
// const int16_t port = 8887; // encrypted: certificate deliberately expired

// Root certificate for Let's Encrypt
const String root_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIFQDCCBCigAwIBAgISA62DzWkTB/LTntyW0qvvPg8lMA0GCSqGSIb3DQEBCwUA\n"
    "MDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQD\n"
    "EwJSMzAeFw0yMjExMjEwMzIwNDNaFw0yMzAyMTkwMzIwNDJaMB0xGzAZBgNVBAMT\n"
    "EnRlc3QubW9zcXVpdHRvLm9yZzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC\n"
    "ggEBAMp0E8YXER6sK0q0IZEsi7yuBWlIsqrvXC8RePddMsF/61h6k9esj/UylMPV\n"
    "8pc7jJi2QxzMDxRwHyGWuHw/8FEtqcboV17OMjBF46K7tQb/eP38NFt+NmY/1It7\n"
    "Dd80OxNqRhGEdu+dQutORRFDAHa7TqcW2H5boQSHnHWf552tOzMzjLdLr8Os6nJE\n"
    "N9RaX024228ylPlFXS9wVfqY7BjBeOrwdD/l/n9f2IbxGo+l+vhuYL0G8WB7N0Q4\n"
    "T8sbgvxPNyAf8fgJhfdZ5i7i9YGiP5C77Y1Ap4QrMaeBH2jU/FmCHbfHct53Z3aS\n"
    "di8LZcJw635mser/rfrtZkVwpuUCAwEAAaOCAmMwggJfMA4GA1UdDwEB/wQEAwIF\n"
    "oDAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDAYDVR0TAQH/BAIwADAd\n"
    "BgNVHQ4EFgQUEL0LHjiq9v+MQs2Tig4ibsXTUuEwHwYDVR0jBBgwFoAUFC6zF7dY\n"
    "VsuuUAlA5h+vnYsUwsYwVQYIKwYBBQUHAQEESTBHMCEGCCsGAQUFBzABhhVodHRw\n"
    "Oi8vcjMuby5sZW5jci5vcmcwIgYIKwYBBQUHMAKGFmh0dHA6Ly9yMy5pLmxlbmNy\n"
    "Lm9yZy8wMgYDVR0RBCswKYISdGVzdC5tb3NxdWl0dG8ub3JnghN0ZXN0Ni5tb3Nx\n"
    "dWl0dG8ub3JnMEwGA1UdIARFMEMwCAYGZ4EMAQIBMDcGCysGAQQBgt8TAQEBMCgw\n"
    "JgYIKwYBBQUHAgEWGmh0dHA6Ly9jcHMubGV0c2VuY3J5cHQub3JnMIIBBQYKKwYB\n"
    "BAHWeQIEAgSB9gSB8wDxAHYAejKMVNi3LbYg6jjgUh7phBZwMhOFTTvSK8E6V6NS\n"
    "61IAAAGEmGq+AQAABAMARzBFAiEArHdygSmb2evaV5aCLCZCU3PbxrHGLBU6r7Wu\n"
    "bW6608ACICaznhejZHavVItLio80bJAOUPYeuCdw2RyyvHSKdAAlAHcA6D7Q2j71\n"
    "BjUy51covIlryQPTy9ERa+zraeF3fW0GvW4AAAGEmGq91AAABAMASDBGAiEAp5GD\n"
    "kIUQlFGNjB4QnSbxuic81uugK3mSf2IT6a/1KoICIQCLvb8UuYwtQJeYKrpGYhuF\n"
    "FnoKwgZvfgIc7+uC2D75BzANBgkqhkiG9w0BAQsFAAOCAQEAEIghttHZ9Q4krh6c\n"
    "tzhg5+hHGsU1QZQSbnZ9HeDn9RyPA8Jj2g5OD9+cLLipq2jb2y/ozoYCtX7dm0Yo\n"
    "T3qWgjQ/VQAUJYKJf8vgmpLrvD9JeJNa1PpLLk4eFiegRPsZKNq0/zqfxBv6F3MM\n"
    "+OQoHq+F8njSV7lRQaCBUQWEYjw/TUqM5wvUX2rHNdAbLFU9Z/hjRziMqzeqyaHX\n"
    "vfyy7hV0wt6poR9cjzeohr8yqLWM21RbXxTi1rw3iqfbK3A85ZOwT9DiOWGsvaxO\n"
    "6x9BFx49ck17Wygo/tGaIuZDy2dcdTtIbFeiA1j1XLVA4Mik2w8z7TQ1wjgiTFiu\n"
    "1xnnZw==\n"
    "-----END CERTIFICATE-----\n";

#include <M5Unified.h>
#include <FastLED.h>
#include <Arduino.h>

// Allocate memory for concrete object
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TestModem testModem(debugger);
#else
TestModem testModem(SerialAT);
#endif

int delay_ms = SEND_INTERVAL_MS;
int next_message_ms = 0;
// Access via the API
GsmModem& modem = testModem;
bool ready = false;

bool publish_done = false;

void setup() {
#if ADVGSM_LOG_SEVERITY > 0
#ifdef LOG_OUTPUT
  AdvancedGsmLog.Log = &LOG_OUTPUT;
#endif
#endif
  M5.begin();

  delay(5000);

  SerialMon.printf("### MQTT client example started at %d\n", millis());

  SerialAT.begin(GSM_BAUDRATE, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);

  modem.begin(apn, pdp_type);
  delay(100);
  SerialMon.print("Setup complete\n");
}

bool isReady() {
  // Get non-link-local IP address
  String addresses[4];
  int8_t count = modem.getLocalIPs(addresses, 4);
  bool found_global_ipv6 = false;
  for (int8_t index = 0; index < count; index++) {
    SerialMon.printf("IP address[%d] = %s\n", index, addresses[index].c_str());
    if (addresses[index].indexOf(":") > 0 &&
        !addresses[index].startsWith("fe80:")) {
      found_global_ipv6 = true;
    }
  }
#ifdef WAIT_FOR_NON_LOCAL_IPV6
  return found_global_ipv6;
#else
  return modem.modemStatus() >= ModemStatus::PacketDataReady;
#endif
}

void connectedLoop() {
  int now = millis();
  if (now > next_message_ms) {
    if (!publish_done) {
      SerialMon.printf("### Testing MQTT to: %s (%d)\n", server, port);

      bool use_tls = false;
      if (port == 8886 || port == 8887) {
        bool ca_success = modem.setRootCA(root_ca);
        if (!ca_success) {
          SerialMon.printf("### Set Root CA failed, delaying %d ms", delay_ms);
          next_message_ms = millis() + delay_ms;
          delay_ms = delay_ms * 2;
          return;
        }
        use_tls = true;
      }

      TestTcpClient testTcpClient(testModem);
      TestMqttClient testMqttClient(testTcpClient, server, port, use_tls);
      MqttClient& mqtt = testMqttClient;

      int8_t rc;
      if (port == 1884) {
        rc = mqtt.connect(client_id, user_name, password);
      } else {
        rc = mqtt.connect(client_id);
      }
      if (rc != 0) {
        Serial.printf("### MQTT connect error: %d, delaying %d ms\n", rc,
                      delay_ms);
        next_message_ms = millis() + delay_ms;
        delay_ms = delay_ms * 2;
        return;
      }

      // Subscribe
      Serial.printf("Subscribing\n");
      mqtt.subscribe(subscribe_topic);
      delay(100);

      // Publish
      Serial.printf("Publishing\n");
      mqtt.publish(publish_topic, "Message from device");
      delay(100);

      // Wait for messages
      int finish = millis() + SEND_INTERVAL_MS;
      while (millis() < finish) {
        modem.loop();
        String topic = mqtt.receiveTopic();
        if (topic.length() > 0) {
          String body = mqtt.receiveBody();
          Serial.printf("Received [%s]: %s\n", topic.c_str(), body.c_str());
        }
        delay(100);
      }

      Serial.printf("Disconnecting\n");
      mqtt.disconnect();

      Serial.printf("Done\n");
      publish_done = true;
    }
  }
}

void loop() {
  modem.loop();
  if (modem.modemStatus() >= ModemStatus::PacketDataReady) {
    if (!ready) {
      ready = isReady();
      Serial.printf("Ready %d (%d)\n", ready, modem.modemStatus());
    }
    if (ready) {
      connectedLoop();
    }
  }
}