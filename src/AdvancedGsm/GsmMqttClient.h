#ifndef Advanced_GsmMqttClient_h
#define Advanced_GsmMqttClient_h

#include "../api/MqttClient.h"
#include "GsmTcpClient.h"

class GsmMqttClient : public MqttClient {
 private:
  static const int16_t BodyBufferSize = 2000;
  static const int16_t TopicBufferSize = 100;

 public:
  GsmMqttClient(GsmTcpClient& client,
                const char server_name[],
                uint16_t server_port = MqttPort,
                bool use_tls = false);

  String receiveBody() override;
  String receiveTopic() override;

 protected:
  GsmTcpClient* client;
  char received_body[BodyBufferSize] = {0};
  char received_topic[TopicBufferSize] = {0};
  const char* server_name;
  uint16_t server_port;
  bool use_tls;
};

#endif