#ifndef Advanced_GsmModemCommon_h
#define Advanced_GsmModemCommon_h

#include "../api/GsmModem.h"

#include <Stream.h>

class GsmModemCommon : public GsmModem {
 public:
  GsmModemCommon(Stream& stream);
  void begin(const char accessPointName[],
             PacketDataProtocolType pdpType,
             const char username[],
             const char password[]) override;
  void loop() override;
  String manufacturer() override;
  String model() override;
  virtual String readResponseLine();
  virtual void sendATCommand(const char command[]);

 protected:
  char gsmNL[3] = GSM_NL;
  Stream& stream;

  virtual bool connect(const char apn[],
                       PacketDataProtocolType pdpType,
                       const char username[],
                       const char password[]) = 0;
  virtual bool reset() = 0;
//  template <typename... Args>
//  void sendAT(Args... command);


  // inline bool streamSkipUntil(const char c, const uint32_t timeout_ms =
  // 1000L):
  int8_t waitResponse();
  int8_t waitResponse(GsmConstStr r1,
                      GsmConstStr r2 = GFP(GSM_ERROR),
                      GsmConstStr r3 = NULL,
                      GsmConstStr r4 = NULL,
                      GsmConstStr r5 = NULL);
  int8_t waitResponse(uint32_t timeout_ms,
                      GsmConstStr r1 = GFP(GSM_OK),
                      GsmConstStr r2 = GFP(GSM_ERROR),
                      GsmConstStr r3 = NULL,
                      GsmConstStr r4 = NULL,
                      GsmConstStr r5 = NULL);
  int8_t waitResponse(uint32_t timeout_ms,
                              String& data,
                              GsmConstStr r1 = GFP(GSM_OK),
                              GsmConstStr r2 = GFP(GSM_ERROR),
                              GsmConstStr r3 = NULL,
                              GsmConstStr r4 = NULL,
                              GsmConstStr r5 = NULL);
  virtual int8_t waitResponseDevice(uint32_t timeout_ms,
                              String& data,
                              GsmConstStr r1 = GFP(GSM_OK),
                              GsmConstStr r2 = GFP(GSM_ERROR),
                              GsmConstStr r3 = NULL,
                              GsmConstStr r4 = NULL,
                              GsmConstStr r5 = NULL) = 0;

 // Define template functions in the header, so that derived classes instantiate
 // See: https://www.modernescpp.com/index.php/surprise-included-inheritance-and-member-functions-of-class-templates
 protected:
  template <typename... Args>
  void sendAT(Args... command) {
    // Serial.print("GsmModemCommon::sendAT\n");
    streamWrite("AT", command..., this->gsmNL);
    this->stream.flush();
  };

 private:
  template <typename T>
  inline void streamWrite(T last) {
    this->stream.print(last);
  }

  template <typename T, typename... Args>
  inline void streamWrite(T head, Args... tail) {
    this->stream.print(head);
    streamWrite(tail...);
  }
};

#endif
