#include "GsmModemCommon.h"

#include <Arduino.h>

// Public

GsmModemCommon::GsmModemCommon(Stream& stream) : stream(stream) {}

void GsmModemCommon::begin(const char access_point_name[],
                           PacketDataProtocolType pdp_type,
                           const char user_name[],
                           const char password[]) {
  this->access_point_name = access_point_name ? access_point_name : "";
  this->pdp_type = pdp_type;
  this->user_name = user_name ? user_name : "";
  this->password = password ? password : "";

  ADVGSM_LOG(GsmSeverity::Info, "GsmModemCommon", GF("Begin connection to %s@%s (%d)"), this->user_name, this->access_point_name, this->pdp_type);

  this->active = true;
  bool success = checkConnection();
  if (!success) {
    this->retry_count++;
    int32_t delay = retry_base_delay_ms * (1 << (this->retry_count - 1));
    ADVGSM_LOG(GsmSeverity::Debug, "GsmModemCommon", "Connection not ready, retry %d delaying for %d ms", this->retry_count, delay);
    this->next_check = millis() + delay;
  }
}

int8_t GsmModemCommon::getLocalIPs(String addresses[], uint8_t max) {
  // TS 27.007: no context should return addresses for all contexts
  // NOTE: +CGPADDR gets the address assigned during the last activation (even
  // if not currently connected)
  this->sendAT(GF("+CGPADDR"));
  bool response_finished = false;
  int8_t address_index = 0;
  while (address_index < max) {
    int8_t response = waitResponse(GFP(GSM_OK), GFP(GSM_ERROR), "+CGPADDR:");
    if (response != 3) {
      response_finished = true;
      break;
    }
    String address_line = this->stream.readStringUntil('\n');

    // Check first returned address
    int start1 = address_line.indexOf('"');
    if (start1 == -1) {
      continue;
    }
    int end1 = address_line.indexOf('"', start1 + 1);
    if (end1 < start1 + 2) {
      continue;
    }
    addresses[address_index] = address_line.substring(start1 + 1, end1);
    if (++address_index >= max) {
      break;
    }

    // Check if there is a second address (index 1)
    int start2 = address_line.indexOf('"', end1 + 1);
    if (start2 == -1) {
      continue;
    }
    int end2 = address_line.indexOf('"', start2 + 1);
    if (end2 < start1 + 2) {
      continue;
    }
    addresses[address_index] = address_line.substring(start2 + 1, end2);
  }
  if (!response_finished) {
    waitResponse();
  }
  return address_index;
}

String GsmModemCommon::ICCID() {
  return "";
}

String GsmModemCommon::IMEI() {
  this->sendAT(GF("+CGSN"));
  String response;
  if (waitResponse(1000, response) != 1) {
    return "unknown";
  }
  response.replace("\r\nOK\r\n", "");
  response.replace("\rOK\r", "");
  response.trim();
  return response;
}

String GsmModemCommon::IMSI() {
  this->sendAT(GF("+CIMI"));
  String response;
  if (waitResponse(1000, response) != 1) {
    return "unknown";
  }
  response.replace("\r\nOK\r\n", "");
  response.replace("\rOK\r", "");
  response.trim();
  return response;
}

String GsmModemCommon::localIP(uint8_t index) {
  String addresses[index];
  uint8_t count = getLocalIPs(addresses, index);
  if (count == 0) {
    return "";
  }
  return addresses[count];
}

void GsmModemCommon::loop() {
  // Serial.print("GsmModemCommon::loop\n");
  //  TODO: Heartbeat check on connection
  if (this->active) {

    // If not ready, then check connection status, with back off delay
    if (this->status < ModemStatus::PacketDataReady) {
      if (this->next_check > -1 && millis() > this->next_check) {
        bool success = checkConnection();
        if (success) {
          this->next_check = -1;
        } else {
          if (this->retry_count > this->retry_max) {
            ADVGSM_LOG(GsmSeverity::Fatal, "GsmModemCommon", "Connection retry count exceeded; modem shutting down");
            this->active = false;
            this->next_check = -1;
            // TODO: high level communication retry after 1 minute / communication sequence retry, with modem hard reset
          }
          this->retry_count++;
          int32_t delay = retry_base_delay_ms * (1 << (this->retry_count - 1));
          ADVGSM_LOG(GsmSeverity::Debug, "GsmModemCommon", "Connection not ready, retry %d delaying for %d ms", this->retry_count, delay);
          this->next_check = millis() + delay;
        }
      }
    }

    // For any unsolicited responses
    this->waitResponse(GSM_COMMAND_DELAY_MS, NULL, NULL);

  }
}

String GsmModemCommon::manufacturer() {
  this->sendAT(GF("+CGMI"));
  String response;
  if (waitResponse(1000, response) != 1) {
    return "unknown";
  }
  response.replace("\r\nOK\r\n", "");
  response.replace("\rOK\r", "");
  response.trim();
  return response;
}

String GsmModemCommon::model() {
  this->sendAT(GF("+CGMM"));
  String response;
  if (waitResponse(1000, response) != 1) {
    return "unknown";
  }
  response.replace("\r\nOK\r\n", "");
  response.replace("\rOK\r", "");
  response.trim();
  return response;
}

ModemStatus GsmModemCommon::modemStatus() {
  return this->status;
}

String GsmModemCommon::network() {
  // Gets the PLMN (Public Land Mobile Network) operator details
  this->sendAT(GF("+COPS?"));
  if (waitResponse("+COPS:") != 1) {
    return "";
  }
  String plmn_details = this->stream.readStringUntil('\n');
  waitResponse();
  int start = plmn_details.indexOf('"');
  if (start == -1) {
    return "";
  }
  int end = plmn_details.indexOf('"', start + 1);
  if (end < start + 2) {
    return "";
  }
  String network = plmn_details.substring(start + 1, end);
  // TODO: Could include the Access Technology, e.g. "(NB-S1)"
  return network;
}

String GsmModemCommon::readResponseLine() {
  return this->stream.readStringUntil('\n');
}

RegistrationStatus GsmModemCommon::registrationStatus() {
  // Registration status results are aligned across versions.
  // Override if needed:
  //  +CREG?
  //  +CGREP? (GPRS)
  //  +CEREG? (EPS)
  //  +C5GREG? (5G)
  this->sendAT(GF("+CEREG?"));
  if (waitResponse("+CEREG:") != 1) {
    return RegistrationStatus::UnknownRegistrationStatus;
  }
  streamSkipUntil(',');  // skip mode
  int16_t status = this->stream.parseInt();
  if (waitResponse() != 1) {
    return RegistrationStatus::UnknownRegistrationStatus;
  }
  return static_cast<RegistrationStatus>(status);
}

String GsmModemCommon::revision() {
  this->sendAT(GF("+CGMR"));
  String response;
  if (waitResponse(1000, response) != 1) {
    return "unknown";
  }
  response.replace("\r\nOK\r\n", "");
  response.replace("\rOK\r", "");
  response.trim();
  return response;
}

int32_t GsmModemCommon::RSSI() {
  this->sendAT(GF("+CSQ"));
  if (waitResponse("+CSQ:") != 1) {
    return 0;
  }
  int16_t rssi_index = streamGetIntBefore(',');
  if (waitResponse() != 1) {
    return 0;
  }
  if (rssi_index == 99) {
    return 0;
  }
  double rssidBm = -113.0 + (rssi_index * 2);
  return rssidBm;
}

void GsmModemCommon::sendATCommand(const char command[]) {
  streamWrite("AT", command, this->gsmNL);
  this->stream.flush();
}

inline int16_t GsmModemCommon::streamGetIntBefore(char lastChar) {
  char buf[7];
  size_t bytesRead =
      this->stream.readBytesUntil(lastChar, buf, static_cast<size_t>(7));
  // if we read 7 or more bytes, it's an overflow
  if (bytesRead && bytesRead < 7) {
    buf[bytesRead] = '\0';
    int16_t res = atoi(buf);
    return res;
  }

  return -9999;
}

inline bool GsmModemCommon::streamSkipUntil(const char c,
                                            const uint32_t timeout_ms) {
  uint32_t startMillis = millis();
  while (millis() - startMillis < timeout_ms) {
    while (millis() - startMillis < timeout_ms && !this->stream.available()) {
      TINY_GSM_YIELD();
    }
    if (this->stream.read() == c) {
      return true;
    }
  }
  return false;
}

int8_t GsmModemCommon::waitResponse() {
  return waitResponse(GSM_OK);
}

int8_t GsmModemCommon::waitResponse(GsmConstStr r1,
                                    GsmConstStr r2,
                                    GsmConstStr r3,
                                    GsmConstStr r4,
                                    GsmConstStr r5) {
  return waitResponse(1000, r1, r2, r3, r4, r5);
}

int8_t GsmModemCommon::waitResponse(uint32_t timeout_ms,
                                    GsmConstStr r1,
                                    GsmConstStr r2,
                                    GsmConstStr r3,
                                    GsmConstStr r4,
                                    GsmConstStr r5) {
  String s;
  return waitResponse(timeout_ms, s, r1, r2, r3, r4, r5);
}

int8_t GsmModemCommon::waitResponse(uint32_t timeout_ms,
                                    String& data,
                                    GsmConstStr r1,
                                    GsmConstStr r2,
                                    GsmConstStr r3,
                                    GsmConstStr r4,
                                    GsmConstStr r5) {
  return checkResponse(timeout_ms, data, r1, r2, r3, r4, r5);
}
