#ifndef Advanced_GsmModem_h
#define Advanced_GsmModem_h

#include "Shared.h"

class GsmModem {
 public:
  virtual void begin() = 0;
  virtual String manufacturer() = 0;
  template <typename... Args>
  void sendAT(Args... cmd);
};

#endif