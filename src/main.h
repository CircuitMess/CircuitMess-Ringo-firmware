#ifndef Main_h
#define Main_h
#include <MAKERphone.h>
extern MAKERphone mp;
extern int backgroundColors[7];
extern String backgroundColorsNames[7];
extern String titles[10];
extern int32_t cameraY;
extern int32_t cameraY_actual;
extern uint16_t cursor;
void menuDrawBox(String text, uint8_t i, int32_t y);
int8_t menu(const char* title, String* items, uint8_t length);
void menuDrawCursor(uint8_t i, int32_t y);
#endif