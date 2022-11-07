/*
GSM HTTP client example

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

//#define WAIT_FOR_NON_LOCAL_IPV6
#define SEND_INTERVAL_MS 5000

const char apn[] = "telstra.iot";
//const PacketDataProtocolType pdp_type = PacketDataProtocolType::IPv4v6;  // default
const PacketDataProtocolType pdp_type = PacketDataProtocolType::IPv6;
//const PacketDataProtocolType pdp_type = PacketDataProtocolType::IP;

// NOTE: Manual DNS is only supported for IPv4
//#define SET_MANUAL_DNS
//const char primaryDns[] = "8.8.8.8";
//const char secondaryDns[] = "8.8.4.4";

#define TLS_PER_CONNECTION
//const char server[] = "v4v6.ipv6-test.com";
const char server[] = "v6.ipv6-test.com";
// NOTE: IPv4 only host works on Telstra IPv6 only, via DNS64 + NAT64
//const char server[] = "v4.ipv6-test.com";
//const int16_t port = 443;  // encrypted
const int16_t port = 80;  // unencrypted

// Root certificate for http://v4v6.ipv6-test.com/api/myip.php
const String root_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB\n"
    "iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n"
    "cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n"
    "BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw\n"
    "MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV\n"
    "BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU\n"
    "aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy\n"
    "dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK\n"
    "AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B\n"
    "3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY\n"
    "tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/\n"
    "Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2\n"
    "VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT\n"
    "79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6\n"
    "c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT\n"
    "Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l\n"
    "c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee\n"
    "UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE\n"
    "Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd\n"
    "BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G\n"
    "A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF\n"
    "Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO\n"
    "VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3\n"
    "ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs\n"
    "8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR\n"
    "iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze\n"
    "Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ\n"
    "XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/\n"
    "qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB\n"
    "VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB\n"
    "L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG\n"
    "jjxDah2nGN59PRbxYvnKkKj9\n"
    "-----END CERTIFICATE-----\n";

#include <Arduino.h>
#include "../../../src/AdvancedGsm/GsmLog.h"

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
#if ADVGSM_LOG_SEVERITY > 0
#ifdef LOG_OUTPUT
  AdvancedGsmLog.Log = &LOG_OUTPUT;
#endif
#endif

  SerialMon.begin(115200);
  delay(5000);
  SerialMon.printf("### HTTP client example started at %d\n", millis());

  SerialAT.begin(GSM_BAUDRATE, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);

  modem.begin(apn, pdp_type);
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
      SerialMon.printf("### Ready with IPv6 address %s\n",
                       addresses[index].c_str());
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
  int now = millis();
  if (now > next_message_ms) {
    SerialMon.printf("Testing HTTP to: %s, %d\n", server, port);
    bool use_tls = false;
    if (port == 443) {
#ifndef TLS_PER_CONNECTION
      SerialMon.print("Setting modem root certificate\n");
      modem.setRootCA(root_ca);
#endif
      use_tls = true;
    }

    TestTcpClient testTcpClient(testModem);
    // Client& client = testTcpClient;

    TestHttpClient testHttpClient(testTcpClient, server, port, use_tls);
    HttpClient& http = testHttpClient;

#ifdef TLS_PER_CONNECTION
    if (use_tls) {
      SerialMon.print("Setting HTTP certificate\n");
      http.setRootCA(root_ca.c_str());
    }
#endif

    int rc = http.get("/api/myip.php");
    if (rc != 0) {
      Serial.printf("HTTP GET error: %d\n", rc);
    } else {
      int httpCode = http.responseStatusCode();
      if (httpCode != 200 && httpCode != 301) {
        Serial.printf("HTTP response code error: %d\n", httpCode);
      } else {
        Serial.printf("\nHTTP response code: %d\n", httpCode);
        String payload = http.responseBody();
        Serial.print("### PAYLOAD:\n");
        Serial.printf("%s\n\n", payload.c_str());
      }
    }

    http.stop();
    next_message_ms = millis() + SEND_INTERVAL_MS;
  }
}

void loop() {
  if (!ready) {
    ready = isReady();
    if (ready) {
      #ifdef SET_MANUAL_DNS
      modem.setDns(primaryDns, secondaryDns);
      #endif
    }
  }
  if (ready) {
    connectedLoop();
  }
  delay(1000);
}