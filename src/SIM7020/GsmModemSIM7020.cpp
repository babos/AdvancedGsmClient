#include "GsmModemSIM7020.h"

#include <Arduino.h>

GsmModemSIM7020::GsmModemSIM7020(Stream& stream)
    : GsmModemCommon(stream) {}

void GsmModemSIM7020::begin() {
  Serial.print("GsmModemSIM7020\n");
}

void GsmModemSIM7020::test() {
  Serial.print("Test\n");
}
