#include "GsmModemCommon.h"

#include <Arduino.h>

// Public

GsmModemCommon::GsmModemCommon(Stream& stream) : stream(stream) {}

void GsmModemCommon::begin(const char accessPointName[],
                           PacketDataProtocolType pdpType,
                           const char username[],
                           const char password[]) {
  if (!reset()) {
    return;
  }
  connect(accessPointName, pdpType, username, password);
}

String GsmModemCommon::iccid() { return ""; }

String GsmModemCommon::imei() {
  this->sendAT(GF("+CGSN"));
  String response;
  if (waitResponse(2000L, response) != 1) {
    return "unknown";
  }
  response.replace("\r\nOK\r\n", "");
  response.replace("\rOK\r", "");
  response.trim();
  return response;
}

String GsmModemCommon::imsi() {
  this->sendAT(GF("+CIMI"));
  String response;
  if (waitResponse(2000L, response) != 1) {
    return "unknown";
  }
  response.replace("\r\nOK\r\n", "");
  response.replace("\rOK\r", "");
  response.trim();
  return response;
}

void GsmModemCommon::loop() {
  Serial.print("GsmModemCommon::begin\n");
}

String GsmModemCommon::manufacturer() {
  this->sendAT(GF("+CGMI"));
  String response;
  if (waitResponse(2000L, response) != 1) {
    return "unknown";
  }
  response.replace("\r\nOK\r\n", "");
  response.replace("\rOK\r", "");
  response.trim();
  return response;
}

String GsmModemCommon::model() {
  this->sendAT(GF("+CGMM"));
  String response;
  if (waitResponse(2000L, response) != 1) {
    return "unknown";
  }
  response.replace("\r\nOK\r\n", "");
  response.replace("\rOK\r", "");
  response.trim();
  return response;
}

String GsmModemCommon::readResponseLine() {
  return this->stream.readStringUntil('\n');
}

String GsmModemCommon::revision() {
  this->sendAT(GF("+CGMR"));
  String response;
  if (waitResponse(2000L, response) != 1) {
    return "unknown";
  }
  response.replace("\r\nOK\r\n", "");
  response.replace("\rOK\r", "");
  response.trim();
  return response;
}

double GsmModemCommon::rssidBm() {
  this->sendAT(GF("+CSQ"));
  if (waitResponse(2000L, "+CSQ:") != 1) {
    return 0;
  }
  int16_t rssi = streamGetIntBefore(',');
  if (waitResponse(2000L) != 1) {
    return 0;
  }
  if (rssi == 99) {
    return 0;
  }
  double rssidBm = -113.0 + (rssi * 2);
  return rssidBm;
}


void GsmModemCommon::sendATCommand(const char command[]) {
  streamWrite("AT", command, this->gsmNL);
  this->stream.flush();
}

// Protected

int16_t GsmModemCommon::streamGetIntBefore(char lastChar) {
  char   buf[7];
  size_t bytesRead = this->stream.readBytesUntil(
      lastChar, buf, static_cast<size_t>(7));
  // if we read 7 or more bytes, it's an overflow
  if (bytesRead && bytesRead < 7) {
    buf[bytesRead] = '\0';
    int16_t res = atoi(buf);
    return res;
  }

  return -9999;
}

// inline bool GsmModemCommon::streamSkipUntil(const char c, const uint32_t
// timeout_ms = 1000L) {
//   uint32_t startMillis = millis();
//   while (millis() - startMillis < timeout_ms) {
//     while (millis() - startMillis < timeout_ms &&
//             !thisModem().stream.available()) {
//       TINY_GSM_YIELD();
//     }
//     if (thisModem().stream.read() == c) { return true; }
//   }
//   return false;
// }

int8_t GsmModemCommon::waitResponse() {
  return waitResponse(GSM_OK);
}

int8_t GsmModemCommon::waitResponse(GsmConstStr r1,
                                    GsmConstStr r2,
                                    GsmConstStr r3,
                                    GsmConstStr r4,
                                    GsmConstStr r5) {
  return waitResponse(1000, r1, r2, r3, r4, r5);
}

int8_t GsmModemCommon::waitResponse(uint32_t timeout_ms,
                                    GsmConstStr r1,
                                    GsmConstStr r2,
                                    GsmConstStr r3,
                                    GsmConstStr r4,
                                    GsmConstStr r5) {
  String s;
  return waitResponse(timeout_ms, s, r1, r2, r3, r4, r5);
}

int8_t GsmModemCommon::waitResponse(uint32_t timeout_ms,
                                    String& data,
                                    GsmConstStr r1,
                                    GsmConstStr r2,
                                    GsmConstStr r3,
                                    GsmConstStr r4,
                                    GsmConstStr r5) {
  return waitResponseDevice(timeout_ms, data, r1, r2, r3, r4, r5);
}
