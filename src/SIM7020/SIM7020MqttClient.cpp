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
  ADVGSM_LOG(GsmSeverity::Debug, "SIM7200", GF("MQTT connect client %s"), client_id);

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

    ADVGSM_LOG(GsmSeverity::Debug, "SIM7200", GF("MQTT %d connecting"),
               mqtt_id);

    // Connect
    // TODO: Should store client_id as field
    this->modem.sendAT(GF("+CMQCON="), this->mqtt_id, ',', mqtt_version, ",\"", client_id, "\",", mqtt_keep_alive_s, ',', clean_session, ",0");
    rc = this->modem.waitResponse(60000);
    if (rc == 0) {
      return -710;
    } else if (rc != 1) {
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

  int16_t rc;
  this->modem.sendAT(GF("+CMQTSYNC=1"));
  rc = this->modem.waitResponse();
  if (rc == 0) {
    return -701;
  } else if (rc != 1) {
    return -601;
  }

  if (this->use_tls) {
    this->modem.sendAT(GF("+CMQTTSNEW=\""), server_name, "\",\"", server_port,
                       "\",", timeout_ms, ',', buffer_size);
    rc = this->modem.waitResponse(30000, GF(GSM_NL "+CMQTTSNEW:"));
    if (rc == 0) {
      return -703;
    } else if (rc != 1) {
      this->modem.sendAT(GF("+CMQTTSNEW?"));
      this->modem.waitResponse();
      return -603;
    }
  } else {
    this->modem.sendAT(GF("+CMQNEW=\""), server_name, "\",\"", server_port,
                       "\",", timeout_ms, ',', buffer_size);
    rc = this->modem.waitResponse(30000, GF(GSM_NL "+CMQNEW:"));
    if (rc == 0) {
      return -702;
    } else if (rc != 1) {
      this->modem.sendAT(GF("+CMQNEW?"));
      this->modem.waitResponse();
      return -602;
    }
  }

  int8_t mqtt_id = this->modem.streamGetIntBefore('\n');
  ADVGSM_LOG(GsmSeverity::Debug, "SIM7200", GF("MQTT %d created"), mqtt_id);
  rc = this->modem.waitResponse();
  if (rc == 0) {
    return -704;
  } else if (rc != 1) {
    return -604;
  }
  delay(100);

  return mqtt_id;
}

int16_t SIM7020MqttClient::createClientInstanceExtended() { return 0; }

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