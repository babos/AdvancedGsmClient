#include "GsmModemCommon.h"

#include <Arduino.h>

// Public

GsmModemCommon::GsmModemCommon(Stream& stream) : stream(stream) {}

void GsmModemCommon::begin() {
  Serial.print("GsmModemCommon::begin\n");
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

template <typename... Args>
void GsmModemCommon::sendAT(Args... cmd) {
  // Serial.print("GsmModemCommon::sendAT\n");
  streamWrite("AT", cmd..., this->gsmNL);
  this->stream.flush();
};

// Protected

// inline bool GsmModemCommon::streamSkipUntil(const char c, const uint32_t timeout_ms = 1000L) {
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
