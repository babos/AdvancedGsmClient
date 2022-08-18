#ifndef Advanced_GsmLog_h
#define Advanced_GsmLog_h

#include <Print.h>
#include "../api/Shared.h"

#define ADVGSM_LOG_ENABLED
#define ADVGSM_LOG_LEVEL 7

#ifdef ADVGSM_LOG_ENABLED
class GsmLog {
 public:
  Print* Log = nullptr;
};

extern GsmLog AdvancedGsmLog;

//     AdvancedGsmLog.Log->printf("<%d>1 %d - %s - - - ", 16 * 8 + severity, millis(), app_name); \

#define ADVGSM_LOG(severity, tag, format, ...)                              \
  if (AdvancedGsmLog.Log != nullptr && severity < ADVGSM_LOG_LEVEL) {       \
    AdvancedGsmLog.Log->printf("<%d>X %d %s ", 16 * 8 + severity, millis(), \
                               tag);                                        \
    AdvancedGsmLog.Log->printf(format __VA_OPT__(, ) __VA_ARGS__);          \
    AdvancedGsmLog.Log->print("\n");                                        \
  }
#else
#define ADVGSM_LOG(severity, app_name, format, ...) ;
#endif

#endif