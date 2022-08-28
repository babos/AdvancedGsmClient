#include "SIM7020MqttClient.h"

SIM7020MqttClient::SIM7020MqttClient(SIM7020TcpClient& client,
                                     const char* server_name,
                                     uint16_t server_port,
                                     bool use_tls)
    : GsmMqttClient(client, server_name, server_port, use_tls),
      modem(client.modem) {
  this->mqtt_id = -1;
  this->is_connected = false;
}

int16_t SIM7020MqttClient::connect(const char client_id[],
                                   const char user_name[],
                                   const char password[]) {
  ADVGSM_LOG(GsmSeverity::Debug, "SIM7200", GF("MQTT connect client %s"),
             client_id);

  int16_t rc;
  // Create if needed
  if (this->mqtt_id == -1) {
    // if (use_tls && this->server_ca != nullptr) {
    //   rc = createClientInstanceExtended();
    // } else {
    rc = createClientInstance();
    // }
    if (rc < 0) {
      return rc;
    }

    // Store the connection
    this->mqtt_id = rc;
    this->modem.mqtt_clients[this->mqtt_id] = this;
  }

  ADVGSM_LOG(GsmSeverity::Debug, "SIM7200", GF("MQTT %d connecting"), mqtt_id);

  // Connect
  // TODO: Should store client_id as field?
  this->modem.stream.printf(GF("AT+CMQCON=%d,%d,\"%s\",%d,%d"),
                            this->mqtt_id, mqtt_version, client_id, mqtt_keep_alive_s, clean_session);
  this->modem.stream.print(GF(",0")); // Will
  if (user_name != nullptr) {
    this->modem.stream.printf(GF(",%s,%s"), user_name, password);
  }
  this->modem.stream.print(GSM_NL);

  rc = this->modem.waitResponse(60000);
  if (rc == 0) {
    ADVGSM_LOG(GsmSeverity::Error, "SIM7200",
               GF("MQTT %s connect client '%s' timed out"), this->mqtt_id,
               client_id);
    return -710;
  } else if (rc != 1) {
    ADVGSM_LOG(GsmSeverity::Error, "SIM7200",
               GF("MQTT %s connect client '%s' error"), this->mqtt_id,
               client_id);
    return -610;
  }

  is_connected = true;
  return 0;
}

boolean SIM7020MqttClient::connected() {
  return this->is_connected;
}

int16_t SIM7020MqttClient::createClientInstance() {
  ADVGSM_LOG(GsmSeverity::Debug, "SIM7200",
             GF("MQTT creating instance %s, %d (%d)"), server_name, server_port,
             use_tls);

  // TODO: Confirm if it already exists and selectively clean up
  for (int8_t client_id = 0; client_id < 1; client_id++) {
    this->modem.sendAT(GF("+CMQDISCON="), client_id);
    this->modem.waitResponse();
  }

  int16_t rc;
  this->modem.sendAT(GF("+CMQTSYNC=1"));
  rc = this->modem.waitResponse();
  if (rc == 0) {
    ADVGSM_LOG(GsmSeverity::Error, "SIM7200", GF("MQTT sync timed out"));
    return -701;
  } else if (rc != 1) {
    ADVGSM_LOG(GsmSeverity::Error, "SIM7200", GF("MQTT sync error"));
    return -601;
  }

  if (this->use_tls) {
    this->modem.sendAT(GF("+CMQTTSNEW=\""), server_name, "\",\"", server_port,
                       "\",", timeout_ms, ',', buffer_size);
    rc = this->modem.waitResponse(timeout_ms, GF(GSM_NL "+CMQTTSNEW:"));
    if (rc == 0) {
      ADVGSM_LOG(GsmSeverity::Error, "SIM7200", GF("MQTTS new timed out"));
      return -703;
    } else if (rc != 1) {
      ADVGSM_LOG(GsmSeverity::Error, "SIM7200", GF("MQTTS new error"));
      this->modem.sendAT(GF("+CMQTTSNEW?"));
      this->modem.waitResponse();
      return -603;
    }
  } else {
    this->modem.sendAT(GF("+CMQNEW=\""), server_name, "\",\"", server_port,
                       "\",", timeout_ms, ',', buffer_size);
    rc = this->modem.waitResponse(timeout_ms, GF(GSM_NL "+CMQNEW:"));
    if (rc == 0) {
      ADVGSM_LOG(GsmSeverity::Error, "SIM7200",
                 GF("MQTT (no TLS) new timed out"));
      return -702;
    } else if (rc != 1) {
      ADVGSM_LOG(GsmSeverity::Error, "SIM7200", GF("MQTT (no TLS) new error"));
      this->modem.sendAT(GF("+CMQNEW?"));
      this->modem.waitResponse();
      return -602;
    }
  }

  int8_t mqtt_id = this->modem.streamGetIntBefore('\n');
  ADVGSM_LOG(GsmSeverity::Debug, "SIM7200", GF("MQTT %d created"), mqtt_id);
  rc = this->modem.waitResponse();
  if (rc == 0) {
    ADVGSM_LOG(GsmSeverity::Error, "SIM7200", GF("MQTT %d create ok timed out"),
               mqtt_id);
    return -704;
  } else if (rc != 1) {
    ADVGSM_LOG(GsmSeverity::Error, "SIM7200", GF("MQTT %d create ok error"),
               mqtt_id);
    return -604;
  }
  delay(100);

  return mqtt_id;
}

int16_t SIM7020MqttClient::createClientInstanceExtended() {
  return 0;
}

void SIM7020MqttClient::disconnect() {
  if (this->mqtt_id > -1) {
    ADVGSM_LOG(GsmSeverity::Info, "SIM7200", GF("MQTT %s disconnect"),
               this->mqtt_id)
    this->modem.sendAT(GF("+CMQDISCON="), this->mqtt_id);
    this->modem.waitResponse(30000);
  }
}

void SIM7020MqttClient::loop() {}

boolean SIM7020MqttClient::publish(const char topic[], const char payload[]) {
  int8_t qos = 0;
  int8_t retained = 0;
  int8_t duplicate = 0;
  int16_t payload_length = strlen(payload);
  int16_t payload_hex_length = payload_length * 2;
  this->modem.stream.printf(GF("AT+CMQPUB=%d,\"%s\",%d,%d,%d,%d,\""),
                            this->mqtt_id, topic, qos, retained, duplicate,
                            payload_hex_length);
  int16_t payload_index = 0;
  while (payload_index < payload_length) {
    char c = payload[payload_index];
    this->modem.stream.printf("%02x", c);
    payload_index++;
  }
  this->modem.stream.print("\"\r\n");

  int16_t rc = this->modem.waitResponse(5000);
  if (rc == 0) {
    ADVGSM_LOG(GsmSeverity::Error, "SIM7200",
               GF("MQTT %s publish '%s' timed out"), this->mqtt_id, topic);
    return false;
  } else if (rc != 1) {
    ADVGSM_LOG(GsmSeverity::Error, "SIM7200", GF("MQTT %s publish '%s' failed"),
               this->mqtt_id, topic);
    return false;
  }

  return true;
}

boolean SIM7020MqttClient::subscribe(const char topic[], int qos) {
  this->modem.sendAT(GF("+CMQSUB="), this->mqtt_id, ",\"", topic, "\",", qos);
  int8_t rc = this->modem.waitResponse(5000);
  if (rc == 0) {
    ADVGSM_LOG(GsmSeverity::Error, "SIM7200",
               GF("MQTT %s subscribe '%s' timed out"), this->mqtt_id, topic);
    return false;
  } else if (rc != 1) {
    ADVGSM_LOG(GsmSeverity::Error, "SIM7200",
               GF("MQTT %s subscribe '%s' failed"), this->mqtt_id, topic);
    return false;
  }
  return false;
}

boolean SIM7020MqttClient::unsubscribe(const char topic[]) {
  this->modem.sendAT(GF("+CMQUNSUB="), this->mqtt_id, ",\"", topic, '"');
  int8_t rc = this->modem.waitResponse(5000);
  if (rc == 0) {
    ADVGSM_LOG(GsmSeverity::Error, "SIM7200",
               GF("MQTT %s unsubscribe '%s' timed out"), this->mqtt_id, topic);
    return false;
  } else if (rc != 1) {
    ADVGSM_LOG(GsmSeverity::Error, "SIM7200",
               GF("MQTT %s unsubscribe '%s' failed"), this->mqtt_id, topic);
    return false;
  }
  return true;
}