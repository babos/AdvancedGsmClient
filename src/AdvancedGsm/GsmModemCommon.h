#ifndef Advanced_GsmModemCommon_h
#define Advanced_GsmModemCommon_h

#include "../api/GsmModem.h"

#include <Stream.h>

class GsmModemCommon : public GsmModem {
 public:
  GsmModemCommon(Stream& stream);
  void begin() override;
  void loop() override;
  virtual String manufacturer();
  virtual String readResponseLine();
  template <typename... Args>
  void writeAT(Args... cmd);

 protected:
  char gsmNL[3] = GSM_NL;

 private:
  Stream& stream;

  template <typename T>
  inline void streamWrite(T last);
  template <typename T, typename... Args>
  inline void streamWrite(T head, Args... tail);
};

#endif
