#ifndef Advanced_GsmModemCommon_h
#define Advanced_GsmModemCommon_h

#include "../api/GsmModem.h"

#include <Stream.h>

class GsmModemCommon : public GsmModem {
 public:
  GsmModemCommon(Stream& stream);
  void begin() override;
  String manufacturer();
  template <typename... Args>
  void sendAT(Args... cmd);

 protected:
  char gsmNL[3] = "\r\n";

 private:
  template <typename T>
  inline void streamWrite(T last);
  template <typename T, typename... Args>
  inline void streamWrite(T head, Args... tail);

  Stream& stream;
};

#endif
