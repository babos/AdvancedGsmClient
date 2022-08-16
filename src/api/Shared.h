#ifndef Advanced_Shared_h
#define Advanced_Shared_h

#include <Arduino.h>

// #if defined(SPARK) || defined(PARTICLE)
// #include "Particle.h"
// #elif defined(ARDUINO)
// #if ARDUINO >= 100
// #include "Arduino.h"
// #else
// #include "WProgram.h"
// #endif
// #endif


// Log level: 1 = error, 2 = warn, 3 = info, 4 = debug, 5 = verbose
#ifdef ADVGSM_LOG_DESTINATION
#ifndef ADVGSM_LOG_LEVEL
#define ADVGSM_LOG_LEVEL ADVGSM_LOG_LEVEL_INFO
#endif
#endif

#define ADVGSM_LOG( level, tag, format, ... ) ADVGSM_LOG_DESTINATION.printf("[%6d][%s]: [%s] ", millis(), level, tag); \
                                              ADVGSM_LOG_DESTINATION.printf(format __VA_OPT__(,) __VA_ARGS__); \
                                              ADVGSM_LOG_DESTINATION.print("\n");

// 0 = None, 1 = Error, 2 = Warn, 3 = Info, 4 = Debug, 5 = Verbose
#if ADVGSM_LOG_LEVEL > 0
#define ADVGSM_LOGE( tag, format, ... ) ADVGSM_LOG("E", tag, format __VA_OPT__(,) __VA_ARGS__)
#else
#define ADVGSM_LOGE( tag, format, ... ) ;
#endif

#if ADVGSM_LOG_LEVEL > 1
#define ADVGSM_LOGW( tag, format, ... ) ADVGSM_LOG("W", tag, format __VA_OPT__(,) __VA_ARGS__)
#else
#define ADVGSM_LOGW( tag, format, ... ) ;
#endif

#if ADVGSM_LOG_LEVEL > 2
#define ADVGSM_LOGI( tag, format, ... ) ADVGSM_LOG("I", tag, format __VA_OPT__(,) __VA_ARGS__)
#else
#define ADVGSM_LOGI( tag, format, ... ) ;
#endif

#if ADVGSM_LOG_LEVEL > 3
#define ADVGSM_LOGD( tag, format, ... ) ADVGSM_LOG("D", tag, format __VA_OPT__(,) __VA_ARGS__)
#else
#define ADVGSM_LOGD( tag, format, ... ) ;
#endif

#if ADVGSM_LOG_LEVEL > 4
#define ADVGSM_LOGV( tag, format, ... ) ADVGSM_LOG("V", tag, format __VA_OPT__(,) __VA_ARGS__)
#else
#define ADVGSM_LOGV( tag, format, ... ) ;
#endif

#ifndef TINY_GSM_YIELD_MS
#define TINY_GSM_YIELD_MS 0
#endif

#ifndef TINY_GSM_YIELD
#define TINY_GSM_YIELD() \
  { delay(TINY_GSM_YIELD_MS); }
#endif

#if defined(__AVR__) && !defined(__AVR_ATmega4809__)
#define TINY_GSM_PROGMEM PROGMEM
typedef const __FlashStringHelper* GsmConstStr;
#define GFP(x) (reinterpret_cast<GsmConstStr>(x))
#define GF(x) F(x)
#else
#define TINY_GSM_PROGMEM
typedef const char* GsmConstStr;
#define GFP(x) x
#define GF(x) x
#endif

#endif