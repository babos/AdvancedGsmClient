#ifndef Advanced_GsmModemSIM7020_h
#define Advanced_GsmModemSIM7020_h

#include "../AdvancedGsm/GsmModemCommon.h"

class GsmModemSIM7020 : public GsmModemCommon {
 public:
  explicit GsmModemSIM7020(Stream& stream);
  void begin() override;
  void test();

 protected:
  int8_t waitResponse(uint32_t timeout_ms,
                      String& data,
                      GsmConstStr r1 = GFP(GSM_OK),
                      GsmConstStr r2 = GFP(GSM_ERROR),
                      GsmConstStr r3 = NULL,
                      GsmConstStr r4 = NULL,
                      GsmConstStr r5 = NULL) override;
};

#endif