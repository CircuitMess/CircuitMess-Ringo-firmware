#ifndef phoneApp_h
#define phoneApp_h

#include "main.h"
void phoneApp();

// Call log
void callLog();
int callLogMenu(JsonArray &call_log, int prevCursor);
uint8_t showCall(int id, String number, uint32_t dateTime, String contact, String duration, uint8_t direction);
void callLogMenuDrawCursor(uint8_t i, int32_t y);
void callLogDrawBoxSD(JsonObject& object, uint8_t i, int32_t y);
void sendMMI(String code);
struct contactsX
{
	//uint8_t id;
	String name;
	String number;
    uint32_t score;
	bool initd = false;
};
String sortAndGetFav();
void phoneMenuDrawBox(String name, String number, uint8_t i, int32_t y, bool blState, int32_t cPos, bool inputFlag);
void phoneMenuDrawCursor(uint8_t i, int32_t y);
void drawStuff();
String getContact(String data, int no);
#endif
