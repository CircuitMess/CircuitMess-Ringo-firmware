#ifndef phoneApp_h
#define phoneApp_h

#include "main.h"
void phoneApp();

// Call log
void callLog();
int callLogMenu(JsonArray *call_log);
void showCall(int call_id);
void deleteCall(int call_id);
void callLogMenuDrawCursor(uint8_t i, int32_t y);
void callLogDrawBoxSD(String number, String dateTime, uint8_t i, int32_t y);
#endif
