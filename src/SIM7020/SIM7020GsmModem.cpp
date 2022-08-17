#include "SIM7020GsmModem.h"

#include "../AdvancedGsm/GsmLog.h"
#include <Arduino.h>

SIM7020GsmModem::SIM7020GsmModem(Stream& stream) : GsmModemCommon(stream) {}

// Public

int8_t SIM7020GsmModem::getLocalIPs(String addresses[], uint8_t max) {
  // SIM7020 requires to specify the context ID to query addresses, i.e.
  // "+CGPADDR=1" works (but "+CGPADDR" does not). The value returned for IPv6
  // (address 2) is only the suffix:
  //   +CGPADDR: 1,"10.89.132.147","::5B5B:EA87:AF0C:8447"
  //
  // Reading the dynamic parameters ("AT+CGCONTRDP") shows the DNS settings, but
  // also only has the IPv6 suffix:
  //   +CGCONTRDP:
  //   1,5,"telstra.iot","10.89.132.147.255.255.255.0",,"101.168.244.101","101.168.244.103",,,,,1500
  //   +CGCONTRDP:
  //   1,5,"telstra.iot","::5B5B:EA87:AF0C:8447/64",,"2001:8004:2D43:C00::","2001:8004:2C42:B16::1",,,,,1500
  //
  // The "+IPCONFIG" command shows full addresses
  // Before connection, it has a single localhost address "127.0.0.1".
  // Once the context is connected (as above), it shows: link local IPv6
  // ("fe80..."), IPv4, and localhost (It can calculate the link local from the
  // suffix) After getting the network prefix it then shows four addresses: link
  // local IPv6, then global IPv6, then IPv4, then localhost
  //   +IPCONFIG: fe80:0:0:0:719d:1439:899a:42d7
  //   +IPCONFIG: 2001:8004:4810:0:719d:1439:899a:42d7
  //   +IPCONFIG: 10.88.134.167
  //   +IPCONFIG: 127.0.0.1
  //
  // The global IPv6 address has to wait for a router advertisement with the
  // prefix (not exposed in AT commands)

  this->sendAT(GF("+IPCONFIG"));

  bool response_finished = false;
  int8_t address_index = 0;
  while (address_index < max) {
    int8_t response = waitResponse(GFP(GSM_OK), GFP(GSM_ERROR), "+IPCONFIG:");
    if (response != 3) {
      response_finished = true;
      break;
    }
    String address_line = this->stream.readStringUntil('\n');
    address_line.trim();
    addresses[address_index] = address_line;
    address_index++;
  }
  if (!response_finished) {
    waitResponse();
  }
  return address_index;
}

String SIM7020GsmModem::ICCID() {
  this->sendAT(GF("+CCID"));
  String response;
  if (waitResponse(1000, response) != 1) {
    return "unknown";
  }
  response.replace("\r\nOK\r\n", "");
  response.replace("\rOK\r", "");
  response.trim();
  return response;
}

// Protected

bool SIM7020GsmModem::connect(const char apn[],
                              PacketDataProtocolType pdpType,
                              const char username[],
                              const char password[]) {
  // Based on "APN Manual Configuration", from SIM7020 TCPIP Application Note

  const char* pdpTypeString = pdpType == PacketDataProtocolType::IPv4v6
                                  ? "IPV4V6"
                              : PacketDataProtocolType::IPv6 ? "IPV6"
                                                             : "IP";

  ADVGSM_LOG(6, "SIM7200", GF("### Connecting %s %s"), pdpTypeString, apn);

  sendAT(GF("+CFUN=0"));
  waitResponse();

  // Set Default PSD Connection Settings
  // Set the user name and password
  // AT*MCGDEFCONT=<PDP_type>[,<APN>[,<username>[,<password>]]]
  // <pdp_type> IPV4V6: Dual PDN Stack
  //            IPV6: Internet Protocol Version 6
  //            IP: Internet Protocol Version 4
  //            Non-IP: external packet data network
  if (password && strlen(password) > 0 && username && strlen(username) > 0) {
    sendAT(GF("*MCGDEFCONT=\""), pdpTypeString, "\",\"", apn, "\",\"", username,
           "\",\"", password, '"');
  } else if (username && strlen(username) > 0) {
    // Set the user name only
    sendAT(GF("*MCGDEFCONT=\""), pdpTypeString, "\",\"", apn, "\",\"", username,
           '"');
  } else {
    // Set the APN only
    sendAT(GF("*MCGDEFCONT=\""), pdpTypeString, "\",\"", apn, '"');
  }
  if (waitResponse() != 1) {
    return false;
  }

  sendAT(GF("+CFUN=1"));
  if (waitResponse(20000L, GF(GSM_NL "+CPIN: READY")) != 1) {
    return false;
  }
  waitResponse();

  // // Check signal strength
  // sendAT(GF("+CSQ"));
  // if (waitResponse() != 1) { return false; }

  // // Check attached
  // sendAT(GF("+CGATT?"));
  // if (waitResponse() != 1) { return false; }

  // // Check operator
  // sendAT(GF("+COPS?"));
  // if (waitResponse() != 1) { return false; }

  // sendAT(GF("+CGREG?"));
  // res = waitResponse(20000L, GF(GSM_NL "+CGREG: 0,1"));
  // waitResponse();
  // res = waitForNetwork(60000, true);
  // delay(100);

  // // Check registeration details
  // sendAT(GF("+CGCONTRDP"));
  // if (waitResponse() != 1) { return false; }

  return true;
}

bool SIM7020GsmModem::reset() {
  //    DBG(GF("### TinyGSM Version:"), TINYGSM_VERSION);
  //    DBG(GF("### TinyGSM Compiled Module:  TinyGsmClientSIM7020"));

  //    if (!testAT()) { return false; }

  sendAT(GF("Z"));  // Reset (to user settings)
  if (waitResponse() != 1) {
    return false;
  }

  sendAT(GF("E0"));  // Echo Off
  if (waitResponse() != 1) {
    return false;
  }

  // TODO: Support error codes
  sendAT(GF("+CMEE=0"));  // turn off error codes
  waitResponse();

  //  DBG(GF("### Modem:"), getModemName());

  // Enable Local Time Stamp for getting network time
  sendAT(GF("+CLTS=1"));
  if (waitResponse(10000) != 1) {
    return false;
  }

  // Enable battery checks
  sendAT(GF("+CBATCHK=1"));
  if (waitResponse() != 1) {
    return false;
  }

  // Set IPv6 format
  sendAT(GF("+CGPIAF=1,1,0,1"));
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
