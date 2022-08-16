#ifndef Advanced_SIM7020TcpClient_h
#define Advanced_SIM7020TcpClient_h

#include "../AdvancedGsm/GsmTcpClient.h"

class SIM7020TcpClient : public GsmTcpClient {
 public:
  SIM7020TcpClient(GsmModem& modem);
};

#endif