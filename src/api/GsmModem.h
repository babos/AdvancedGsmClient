#ifndef Advanced_GsmModem_h
#define Advanced_GsmModem_h

#include "Shared.h"

#define GSM_NL "\r\n"
#define GSM_OK "OK" GSM_NL
#define GSM_ERROR "ERROR" GSM_NL
// The DTE shall not begin issuing a subsequent command line until at least one-tenth of a second 
// has elapsed after receipt of the entire result code issued by the DCE
#define GSM_COMMAND_DELAY_MS 100

class GsmModem {
 public:
  virtual void begin() = 0;
  virtual void loop() = 0;
  virtual String manufacturer() = 0;
  virtual String readResponseLine();
  template <typename... Args>
  void sendAT(Args... command);
};

#endif