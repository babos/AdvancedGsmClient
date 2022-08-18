#include "SIM7020HttpClient.h"

SIM7020HttpClient::SIM7020HttpClient(SIM7020TcpClient& client,
                                     const char* server_name,
                                     uint16_t server_port,
                                     bool https)
    : GsmHttpClient(client, server_name, server_port), modem(client.modem) {
  this->is_connected = false;
  this->http_client_id = -1;
  this->scheme = https ? SCHEME_HTTPS : SCHEME_HTTP;
}

SIM7020GsmModem& SIM7020HttpClient::getModem() {
  return this->modem;
}

// Protected

int SIM7020HttpClient::startRequest(const char* url_path,
                                    const char* http_method,
                                    const char* content_type,
                                    int content_length,
                                    const byte body[]) {
  ADVGSM_LOG(GsmSeverity::Info, "SIM7200", GF("HTTP start %s %s (%d, %d)"),
             http_method, url_path, is_connected, http_client_id);
  int8_t rc;
  // Connect if needed
  if (!is_connected) {
    // Create if needed
    if (this->http_client_id == -1) {
      ADVGSM_LOG(GsmSeverity::Debug, "SIM7200", GF("HTTP creating %d, %s, %d"),
                 scheme, server_name, server_port);
      // Create
      if (scheme == SCHEME_HTTP) {
        this->modem.sendAT(GF("+CHTTPCREATE=\""), GSM_PREFIX_HTTP, server_name,
                           ':', server_port, "/\"");
      } else if (scheme == SCHEME_HTTPS) {
        this->modem.sendAT(GF("+CHTTPCREATE=\""), GSM_PREFIX_HTTPS, server_name,
                           ':', server_port, "/\"");
      } else {
        return -600;
      }

      int8_t rc = this->modem.waitResponse(30000, GF(GSM_NL "+CHTTPCREATE:"));
      if (rc == 0) {
        return -702;
      } else if (rc != 1) {
        this->modem.sendAT(GF("+CHTTPCREATE?"));
        this->modem.waitResponse();
        return -602;
      }
      int8_t http_client_id = this->modem.streamGetIntBefore('\n');
      ADVGSM_LOG(GsmSeverity::Debug, "SIM7200", GF("HTTP %d client created"),
                 http_client_id);
      rc = this->modem.waitResponse();
      if (rc == 0) {
        return -703;
      } else if (rc != 1) {
        return -603;
      }

      // Store the connection
      this->http_client_id = http_client_id;
      this->modem.http_clients[this->http_client_id] = this;
    }

    ADVGSM_LOG(GsmSeverity::Debug, "SIM7200", GF("HTTP %d connecting"),
               http_client_id);

    // Connect
    this->modem.sendAT(GF("+CHTTPCON="), this->http_client_id);
    rc = this->modem.waitResponse(60000);
    if (rc == 0) {
      return -704;
    } else if (rc != 1) {
      return -604;
    }

    is_connected = true;
  }

  // Send the request
  if (strcmp(http_method, HTTP_METHOD_GET) == 0) {
    this->modem.sendAT(GF("+CHTTPSEND="), this->http_client_id, ",0,",
                       url_path);
    rc = this->modem.waitResponse(5000);
    if (rc == 0) {
      return -705;
    } else if (rc != 1) {
      return -605;
    }
    // } else if (strcmp(http_method, GSM_HTTP_METHOD_POST) == 0) {

    // } else if (strcmp(http_method, GSM_HTTP_METHOD_PUT) == 0) {

    // } else if (strcmp(http_method, GSM_HTTP_METHOD_DELETE) == 0) {

  } else {
    return -601;
  }

  return 0;
}

void SIM7020HttpClient::stop() {
  if (this->http_client_id > -1) {
    this->modem.sendAT(GF("+CHTTPDISCON="), this->http_client_id);
    this->modem.waitResponse(30000);
    this->modem.sendAT(GF("+CHTTPDESTROY="), this->http_client_id);
    this->modem.waitResponse(30000);
  }
}
