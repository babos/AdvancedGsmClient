#ifndef Advanced_GsmModemSIM7020_h
#define Advanced_GsmModemSIM7020_h

#include "../AdvancedGsm/GsmModemCommon.h"

class GsmModemSIM7020 : public GsmModemCommon {
 public:
  explicit GsmModemSIM7020(const Stream& stream);
  void begin() override;
  void test();
};

#endif