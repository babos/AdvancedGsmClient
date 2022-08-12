#ifndef Advanced_GsmModemCommon_h
#define Advanced_GsmModemCommon_h

#include "../api/GsmModem.h"

#include <Stream.h>

class GsmModemCommon : public GsmModem {
 public:
  GsmModemCommon(const Stream& stream);
  void begin() override;
  String manufacturer();
  template <typename... Args>
  void sendAT(Args... cmd);

 private:
  const Stream& stream;
};

#endif
