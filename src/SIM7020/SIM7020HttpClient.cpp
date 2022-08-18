#include "SIM7020HttpClient.h"

SIM7020HttpClient::SIM7020HttpClient(SIM7020TcpClient& client,
                                     const char* server_name,
                                     uint16_t server_port)
    : GsmHttpClient(client, server_name, server_port), modem(client.modem) {
  this->http_client_id = -1;
  this->scheme = SCHEME_HTTP;
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
  ADVGSM_LOG(GsmSeverity::Info, "SIM7200", GF("### HTTP %s %s"), http_method, url_path);

  // Connect if needed
  if (!is_connected) {
    // Create if needed
    if (this->http_client_id == -1) {
      ADVGSM_LOG(GsmSeverity::Debug, "SIM7200", GF("### HTTP creating"));

      // Create
      if (scheme == SCHEME_HTTP) {
        this->modem.sendAT(GF("+CHTTPCREATE=\""), GSM_PREFIX_HTTP, server_name,
                           ':', server_port, "/\"");
      } else if (scheme == SCHEME_HTTPS) {
        // TODO: with cert
      } else {
        return HTTP_ERROR_API;
      }

      if (this->modem.waitResponse(30000, GF(GSM_NL "+CHTTPCREATE:")) != 1) {
        this->modem.sendAT(GF("+CHTTPCREATE?"));
        this->modem.waitResponse();
        return HTTP_ERROR_API;
      }
      int8_t http_client_id = this->modem.streamGetIntBefore('\n');
      ADVGSM_LOG(GsmSeverity::Debug, "SIM7200", GF("### HTTP client created:"), http_client_id);
      if (this->modem.waitResponse() != 1) {
        return HTTP_ERROR_API;
      }

      // Store the connection
      this->http_client_id = http_client_id;
      this->modem.http_clients[this->http_client_id] = this;
    }

    ADVGSM_LOG(GsmSeverity::Debug, "SIM7200", GF("### HTTP connecting"), http_client_id);

    // Connect
    this->modem.sendAT(GF("+CHTTPCON="), this->http_client_id);
    if (this->modem.waitResponse(30000) != 1) {
      return HTTP_ERROR_CONNECTION_FAILED;
    }

    is_connected = true;
  }

  // Send the request
  if (strcmp(http_method, HTTP_METHOD_GET) == 0) {
    this->modem.sendAT(GF("+CHTTPSEND="), this->http_client_id, ",0,",
                       url_path);
    if (this->modem.waitResponse(5000) != 1) {
      return HTTP_ERROR_INVALID_RESPONSE;
    }
    // } else if (strcmp(http_method, GSM_HTTP_METHOD_POST) == 0) {

    // } else if (strcmp(http_method, GSM_HTTP_METHOD_PUT) == 0) {

    // } else if (strcmp(http_method, GSM_HTTP_METHOD_DELETE) == 0) {

  } else {
    return HTTP_ERROR_API;
  }

  return HTTP_SUCCESS;
}

void SIM7020HttpClient::stop() {
  if (this->http_client_id > -1) {
    this->modem.sendAT(GF("+CHTTPDISCON="), this->http_client_id);
    this->modem.waitResponse(30000);
    this->modem.sendAT(GF("+CHTTPDESTROY="), this->http_client_id);
    this->modem.waitResponse(30000);
  }
}
