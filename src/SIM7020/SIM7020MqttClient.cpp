#include "SIM7020MqttClient.h"

SIM7020MqttClient::SIM7020MqttClient(SIM7020TcpClient& client,
                                     const char* server_name,
                                     uint16_t server_port,
                                     bool use_tls)
    : GsmMqttClient(client, server_name, server_port, use_tls),
      modem(client.modem) {
  this->is_connected = false;
}

boolean SIM7020MqttClient::connect(const char client_id[],
                                   const char user_name[],
                                   const char password[]) {
  return false;
}

boolean SIM7020MqttClient::connected() {
  return this->is_connected;
}

void SIM7020MqttClient::disconnect() {}

void SIM7020MqttClient::loop() {}

boolean SIM7020MqttClient::publish(const char topic[], const char payload[]) {
  return false;
}

boolean SIM7020MqttClient::subscribe(const char topic[], int qos) {
  return false;
}

boolean SIM7020MqttClient::unsubscribe(const char topic[]) {
  return false;
}