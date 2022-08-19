#ifndef Advanced_HttpClientSIM7020_h
#define Advanced_HttpClientSIM7020_h

#include "../AdvancedGsm/GsmHttpClient.h"
#include "../api/GsmModem.h"
#include "SIM7020GsmModem.h"
#include "SIM7020TcpClient.h"

class SIM7020HttpClient : public GsmHttpClient {
  friend class SIM7020GsmModem;

 public:
  explicit SIM7020HttpClient(SIM7020TcpClient& client,
                             const char* server_name,
                             uint16_t server_port = 80,
                             bool https = false);

  uint8_t connected() override;
  SIM7020GsmModem& getModem() override;
  bool setClientCA(const String& certificate) override;
  bool setClientPrivateKey(const String& certificate) override;
  bool setRootCA(const String& certificate) override;
  void stop() override;

 protected:
  int8_t http_client_id;
  SIM7020GsmModem& modem;
  bool is_connected;

  int startRequest(const char* url_path,
                   const char* http_method,
                   const char* content_type = NULL,
                   int content_length = -1,
                   const byte body[] = NULL) override;

 private:
  int16_t createClientInstance();

  const char* server_ca;
  const char* client_certificate;
  const char* client_private_key;
};

#endif