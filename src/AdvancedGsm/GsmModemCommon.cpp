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
  writeAT(GF("+CGMI"));
  return "unknown";
}

String GsmModemCommon::readResponseLine() {
  return this->stream.readStringUntil('\n');
}

template <typename... Args>
void GsmModemCommon::writeAT(Args... cmd) {
  //Serial.print("GsmModemCommon::writeAT\n");
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
