#include "GsmModemCommon.h"

#include <Arduino.h>

GsmModemCommon::GsmModemCommon(const Stream& stream) : stream(stream) {}

void GsmModemCommon::begin() {
  Serial.print("GsmModemCommon");
}

String GsmModemCommon::manufacturer() {
  return "unknown";
}
