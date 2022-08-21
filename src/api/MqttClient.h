#ifndef GsmTcpClient_h
#define GsmTcpClient_h

#include <Arduino.h>
#include <Client.h>

class MqttClient {
 public:
  static const int MqttPort = 1883;
  static const int MqttsPort = 8883;

  // TODO: Need beginPublish(), write(), endPublish()
  virtual boolean connect(const char client_id[],
                          const char user_name[],
                          const char password[]) = 0;
  // TODO retain, will, etc
  virtual boolean connected() = 0;
  virtual void disconnect() = 0;
  virtual void loop() = 0;
  virtual boolean publish(const char topic[], const char payload[]) = 0;
  // bool publish(const char topic[], const char payload[], bool retained =
  // false, int qos = 0);
  //  Need callback or have polling methods similar to HTTP client
  //  responseCode(), responseBody()
  virtual String receiveBody() = 0;
  virtual String receiveTopic() = 0;
  virtual boolean subscribe(const char topic[], int qos = 0) = 0;
  virtual boolean unsubscribe(const char topic[]) = 0;
};

#endif