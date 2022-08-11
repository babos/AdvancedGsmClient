#ifndef Advanced_GsmModemCommon_h
#define Advanced_GsmModemCommon_h

#include "../api/GsmModem.h"

#include <Stream.h>

class GsmModemCommon : public GsmModem
{
 public:
  GsmModemCommon(Stream& stream);
 private:
  Stream& stream;
};

#endif
