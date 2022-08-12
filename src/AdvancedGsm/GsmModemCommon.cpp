#include "GsmModemCommon.h"

#include <Arduino.h>

// Public

GsmModemCommon::GsmModemCommon(Stream& stream) : stream(stream) {}

void GsmModemCommon::begin() {
  Serial.print("GsmModemCommon::begin\n");
}

String GsmModemCommon::manufacturer() {
  sendAT(GF("+CGMI"));
  return "unknown";
}

template <typename... Args>
void GsmModemCommon::sendAT(Args... cmd) {
  Serial.print("GsmModemCommon::sendAT\n");
  streamWrite("AT", cmd..., this->gsmNL);
  this->stream.flush();
};

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
