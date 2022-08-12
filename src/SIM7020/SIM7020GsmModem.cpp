#include "SIM7020GsmModem.h"

#include <Arduino.h>

SIM7020GsmModem::SIM7020GsmModem(Stream& stream) : GsmModemCommon(stream) {}

void SIM7020GsmModem::test() {
  Serial.print("Test\n");
}

bool SIM7020GsmModem::connect(const char apn[],
                              PacketDataProtocolType pdpType,
                              const char username[],
                              const char password[]) {
  // Based on "APN Manual Configuration", from SIM7020 TCPIP Application Note

  const char* pdpTypeString = pdpType == PacketDataProtocolType::IPv4v6 ? "IPV4V6"
                    : PacketDataProtocolType::IPv6            ? "IPV6"
                                                              : "IP";

  GsmModemCommon::sendAT(GF("+CFUN=0"));
  waitResponse();

  // Set Default PSD Connection Settings
  // Set the user name and password
  // AT*MCGDEFCONT=<PDP_type>[,<APN>[,<username>[,<password>]]]
  // <pdp_type> IPV4V6: Dual PDN Stack
  //            IPV6: Internet Protocol Version 6
  //            IP: Internet Protocol Version 4
  //            Non-IP: external packet data network
  bool res = false;
  if (password && strlen(password) > 0 && username && strlen(username) > 0) {
    GsmModemCommon::sendAT(GF("*MCGDEFCONT=\""), pdpTypeString, "\",\"", apn, "\",\"", username, "\",\"",
           password, '"');
  } else if (username && strlen(username) > 0) {
    // Set the user name only
    GsmModemCommon::sendAT(GF("*MCGDEFCONT=\""), pdpTypeString, "\",\"", apn, "\",\"", username, '"');
  } else {
    // Set the APN only
    GsmModemCommon::sendAT(GF("*MCGDEFCONT=\""), pdpTypeString, "\",\"", apn, '"');
  }
  waitResponse();

  GsmModemCommon::sendAT(GF("+CFUN=1"));
  res = waitResponse(20000L, GF(GSM_NL "+CPIN: READY"));
  if (res != 1) {
    return res;
  }
  waitResponse();

  // sendAT(GF("+CGREG?"));
  // res = waitResponse(20000L, GF(GSM_NL "+CGREG: 0,1"));
  // waitResponse();
  // res = waitForNetwork(60000, true);
  // delay(100);

  return res;
}

bool SIM7020GsmModem::reset() {
  //    DBG(GF("### TinyGSM Version:"), TINYGSM_VERSION);
  //    DBG(GF("### TinyGSM Compiled Module:  TinyGsmClientSIM7020"));

  //    if (!testAT()) { return false; }

  this->sendAT(GF("E0"));  // Echo Off
  if (waitResponse() != 1) {
    return false;
  }

  this->sendAT(GF("+CMEE=0"));  // turn off error codes
  waitResponse();

  //  DBG(GF("### Modem:"), getModemName());

  // Enable Local Time Stamp for getting network time
  this->sendAT(GF("+CLTS=1"));
  if (waitResponse(10000) != 1) {
    return false;
  }

  // Enable battery checks
  this->sendAT(GF("+CBATCHK=1"));
  if (waitResponse() != 1) {
    return false;
  }

  // Set IPv6 format
  this->sendAT(GF("+CGPIAF=1,1,0,1"));
  if (waitResponse() != 1) {
    return false;
  }

  // SimStatus ret = getSimStatus();
  // // if the sim isn't ready and a pin has been provided, try to unlock the
  // sim if (ret != SIM_READY && pin != NULL && strlen(pin) > 0) {
  //   simUnlock(pin);
  //   return (getSimStatus() == SIM_READY);
  // } else {
  //   // if the sim is ready, or it's locked but no pin has been provided,
  //   // return true
  //   return (ret == SIM_READY || ret == SIM_LOCKED);
  // }
  return true;
}

int8_t SIM7020GsmModem::waitResponseDevice(uint32_t timeout_ms,
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
