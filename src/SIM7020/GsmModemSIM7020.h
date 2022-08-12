#ifndef Advanced_GsmModemSIM7020_h
#define Advanced_GsmModemSIM7020_h

#include "../AdvancedGsm/GsmModemCommon.h"

class GsmModemSIM7020 : public GsmModemCommon {
 public:
  explicit GsmModemSIM7020(Stream& stream);
  void test();

 protected:
  bool connect(const char apn[],
               PacketDataProtocolType pdpType,
               const char username[],
               const char password[]) override;
  bool reset() override;
//   int8_t waitResponse();
//   int8_t waitResponse(GsmConstStr r1,
//                       GsmConstStr r2 = GFP(GSM_ERROR),
//                       GsmConstStr r3 = NULL,
//                       GsmConstStr r4 = NULL,
//                       GsmConstStr r5 = NULL);
//   int8_t waitResponse(uint32_t timeout_ms,
//                       GsmConstStr r1 = GFP(GSM_OK),
//                       GsmConstStr r2 = GFP(GSM_ERROR),
//                       GsmConstStr r3 = NULL,
//                       GsmConstStr r4 = NULL,
//                       GsmConstStr r5 = NULL);
  virtual int8_t waitResponseDevice(uint32_t timeout_ms,
                              String& data,
                              GsmConstStr r1 = GFP(GSM_OK),
                              GsmConstStr r2 = GFP(GSM_ERROR),
                              GsmConstStr r3 = NULL,
                              GsmConstStr r4 = NULL,
                              GsmConstStr r5 = NULL) override;
};

#endif