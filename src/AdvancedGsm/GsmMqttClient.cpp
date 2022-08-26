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
  String s = String(this->received_body);
  this->received_body[0] = '\0';
  return s;
}

String GsmMqttClient::receiveTopic() {
  String s = String(this->received_topic);
  this->received_topic[0] = '\0';
  return s;
}
