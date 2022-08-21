#include "GsmMqttClient.h"

GsmMqttClient::GsmMqttClient(GsmTcpClient& client,
                             const char server_name[],
                             uint16_t server_port,
                             bool use_tls)
    : client(&client),
      server_name(server_name),
      server_port(server_port),
      use_tls(use_tls) {}

String GsmMqttClient::receiveBody() {
  return String(this->received_body);
}

String GsmMqttClient::receiveTopic() {
  return String(this->received_topic);
}
