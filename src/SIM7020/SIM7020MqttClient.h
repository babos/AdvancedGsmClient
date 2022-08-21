#ifndef Advanced_SIM7020MqttClient_h
#define Advanced_SIM7020MqttClient_h

#include "../AdvancedGsm/GsmMqttClient.h"
#include "SIM7020GsmModem.h"
#include "SIM7020TcpClient.h"

class SIM7020MqttClient : public GsmMqttClient {
  friend class SIM7020GsmModem;

 public:
  SIM7020MqttClient(SIM7020TcpClient& client,
                    const char* server_name,
                    uint16_t server_port = 80,
                    bool use_tls = false);

  boolean connect(const char client_id[],
                  const char user_name[],
                  const char password[]) override;
  boolean connected() override;
  void disconnect() override;
  void loop() override;
  boolean publish(const char topic[], const char payload[]) override;
  boolean subscribe(const char topic[], int qos = 0) override;
  boolean unsubscribe(const char topic[]) override;

 protected:
  int8_t mqtt_client_id;
  SIM7020GsmModem& modem;
  bool is_connected;
};

#endif