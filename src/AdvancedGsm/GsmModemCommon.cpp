#include "GsmModemCommon.h"

#include <Arduino.h>

GsmModemCommon::GsmModemCommon(const Stream& stream) : stream(stream) {}

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
};
