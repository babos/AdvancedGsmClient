#ifndef Advanced_HttpClientSIM7020_h
#define Advanced_HttpClientSIM7020_h

#include "../AdvancedGsm/GsmHttpClient.h"
#include "SIM7020TcpClient.h"

class SIM7020HttpClient : public GsmHttpClient
{
 public:
  explicit SIM7020HttpClient(SIM7020TcpClient& client, const char* server_name, uint16_t server_port = 80);

 protected:
  int8_t http_client_id;
  SIM7020GsmModem& modem;
  bool is_connected;

  int startRequest(const char* url_path,
                    const char* http_method,
                    const char* content_type = NULL,
                    int content_length = -1,
                    const byte body[] = NULL) override;
  void stop() override;
};

#endif