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

void GsmModemCommon::loop() {
  Serial.print("GsmModemCommon::begin\n");
}

String GsmModemCommon::manufacturer() {
  sendAT(GF("+CGMI"));
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

void GsmModemCommon::sendATCommand(const char command[]) {
  streamWrite("AT", command, this->gsmNL);
  this->stream.flush();
}

// Protected

template <typename... Args>
void GsmModemCommon::sendAT(Args... command) {
  // Serial.print("GsmModemCommon::sendAT\n");
  streamWrite("AT", command..., this->gsmNL);
  this->stream.flush();
};

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

// Private

template <typename T>
inline void GsmModemCommon::streamWrite(T last) {
  this->stream.print(last);
}

template <typename T, typename... Args>
inline void GsmModemCommon::streamWrite(T head, Args... tail) {
  this->stream.print(head);
  streamWrite(tail...);
}
