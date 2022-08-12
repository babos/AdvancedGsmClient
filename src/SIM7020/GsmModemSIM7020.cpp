#include "GsmModemSIM7020.h"

#include <Arduino.h>

GsmModemSIM7020::GsmModemSIM7020(const Stream& stream)
    : GsmModemCommon(stream) {}

void GsmModemSIM7020::begin() {
  Serial.print("GsmModemSIM7020");
}
