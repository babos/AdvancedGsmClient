#ifndef Advanced_SIM7020MqttClient_h
#define Advanced_SIM7020MqttClient_h

#include "../AdvancedGsm/GsmMqttClient.h"
#include "SIM7020GsmModem.h"
#include "SIM7020TcpClient.h"

class SIM7020MqttClient : public GsmMqttClient {
  friend class SIM7020GsmModem;

 private:
  static const int16_t buffer_size = 1024;
  static const int16_t timeout_ms = 12000;

 public:
  SIM7020MqttClient(SIM7020TcpClient& client,
                    const char* server_name,
                    uint16_t server_port = MqttPort,
                    bool use_tls = false);

  int16_t connect(const char client_id[],
                  const char user_name[] = NULL,
                  const char password[] = NULL,
                  bool clean_session = true) override;
  boolean connected() override;
  void disconnect() override;
  void loop() override;
  boolean publish(const char topic[], const char payload[]) override;
  boolean subscribe(const char topic[], int qos = 0) override;
  boolean unsubscribe(const char topic[]) override;

 protected:
  int8_t mqtt_id;
  SIM7020GsmModem& modem;
  bool is_connected;

 private:
  int16_t createClientInstance();
  int16_t createClientInstanceExtended();

  const char* server_ca;
  const char* client_certificate;
  const char* client_private_key;
};

#endif