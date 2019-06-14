#ifndef phoneApp_h
#define phoneApp_h

#include "main.h"
void phoneApp();

// Call log
void callLog();
int callLogMenu(JsonArray &call_log, int prevCursor);
uint8_t showCall(int id, String number, uint32_t dateTime, String duration, uint8_t direction);
void callLogMenuDrawCursor(uint8_t i, int32_t y);
void callLogDrawBoxSD(JsonObject& object, uint8_t i, int32_t y);
#endif
