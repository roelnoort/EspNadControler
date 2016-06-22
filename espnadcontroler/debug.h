#include "Arduino.h"

//#define DEBUG_TO_SERIAL

#ifdef DEBUG_TO_SERIAL
#define DEBUGLOG(x) Serial.println(x)
#else
#define DEBUGLOG(x)
#endif
