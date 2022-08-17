#ifndef Advanced_GsmLog_h
#define Advanced_GsmLog_h

#include "../api/Shared.h"
#include <Print.h>

#define ADVGSM_LOG_ENABLED

#ifdef ADVGSM_LOG_ENABLED
class GsmLog {
 public:
  Print* Log = nullptr;
};

extern GsmLog AdvancedGsmLog;

#define ADVGSM_LOG( severity, app_name, format, ... ) if (AdvancedGsmLog.Log != nullptr) { \
    AdvancedGsmLog.Log->printf("<%d>1 %d - %s - - - ", 16 * 8 + severity, millis(), app_name); \
    AdvancedGsmLog.Log->printf(format __VA_OPT__(,) __VA_ARGS__); \
    AdvancedGsmLog.Log->print("\n"); \
}
#else
#define ADVGSM_LOG( severity, app_name, format, ... ) ;
#endif

#endif