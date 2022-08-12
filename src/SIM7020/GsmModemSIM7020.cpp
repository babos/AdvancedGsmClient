#include "GsmModemSIM7020.h"

#include <Arduino.h>

GsmModemSIM7020::GsmModemSIM7020(Stream& stream) : GsmModemCommon(stream) {}

void GsmModemSIM7020::test() {
  Serial.print("Test\n");
}

void GsmModemSIM7020::connect(const char apn[],
                              PacketDataProtocolType pdpType,
                              const char username[],
                              const char password[]) {}

void GsmModemSIM7020::reset() {}

int8_t GsmModemSIM7020::waitResponse(uint32_t timeout_ms,
                                     String& data,
                                     GsmConstStr r1,
                                     GsmConstStr r2,
                                     GsmConstStr r3,
                                     GsmConstStr r4,
                                     GsmConstStr r5) {
  /*String r1s(r1); r1s.trim();
  String r2s(r2); r2s.trim();
  String r3s(r3); r3s.trim();
  String r4s(r4); r4s.trim();
  String r5s(r5); r5s.trim();
  DBG("### ..:", r1s, ",", r2s, ",", r3s, ",", r4s, ",", r5s);*/
  data.reserve(64);
  uint8_t index = 0;
  uint32_t startMillis = millis();
  do {
    TINY_GSM_YIELD();
    while (this->stream.available() > 0) {
      TINY_GSM_YIELD();
      int8_t a = stream.read();
      if (a <= 0)
        continue;  // Skip 0x00 bytes, just in case
      data += static_cast<char>(a);
      if (r1 && data.endsWith(r1)) {
        index = 1;
        goto finish;
      } else if (r2 && data.endsWith(r2)) {
        index = 2;
        goto finish;
      } else if (r3 && data.endsWith(r3)) {
#if defined TINY_GSM_DEBUG
        if (r3 == GFP(GSM_CME_ERROR)) {
          streamSkipUntil('\n');  // Read out the error
        }
#endif
        index = 3;
        goto finish;
      } else if (r4 && data.endsWith(r4)) {
        index = 4;
        goto finish;
      } else if (r5 && data.endsWith(r5)) {
        index = 5;
        goto finish;
      } else if (data.endsWith(GF("+CLTS:"))) {
        //        streamSkipUntil('\n');  // Refresh time and time zone by
        //        network
        data = "";
        //        DBG("### Unsolicited local timestamp.");
      } else if (data.endsWith(GF("+CTZV:"))) {
        //        streamSkipUntil('\n');  // Refresh network time zone by
        //        network
        data = "";
        //        DBG("### Unsolicited time zone updated.");
      } else if (data.endsWith(GF(GSM_NL "SMS Ready" GSM_NL))) {
        data = "";
        //        DBG("### Unexpected module reset!");
        //        init();
        data = "";
      }
    }
  } while (millis() - startMillis < timeout_ms);
finish:
  if (!index) {
    data.trim();
    if (data.length()) {
      //      DBG("### Unhandled:", data);
    }
    data = "";
  }
  // data.replace(GSM_NL, "/");
  // DBG('<', index, '>', data);
  return index;
}
