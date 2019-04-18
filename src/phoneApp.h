#ifndef phoneApp_h
#define phoneApp_h

#include "main.h"
void phoneApp();

// Call log
void callLog();
int callLogMenu(JsonArray *call_log);
uint8_t showCall(int id, String number, String dateTime, String duration);
void callLogMenuDrawCursor(uint8_t i, int32_t y);
void callLogDrawBoxSD(String number, String dateTime, uint8_t i, int32_t y);
#endif
