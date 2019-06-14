#ifndef clockApp_h
#define clockApp_h

#include "main.h"
void clockApp();
void clockStopwatch();
int8_t clockMenu(String* title, uint8_t length, int8_t prevCursor);
void clockMenuDrawBox(String title, uint8_t i, int32_t y);
void clockAlarm();
int8_t clockAlarmMenu(uint8_t* alarmsArray, uint8_t length);
void clockAlarmMenuDrawBox(uint8_t alarmIndex, uint8_t i, int32_t y);
void clockAlarmEdit(uint8_t index);
extern String alarmTrack[5];
void clockTimer();
extern uint8_t alarmHours[5];
extern uint8_t alarmMins[5];
extern uint8_t alarmEnabled[5];
extern bool alarmRepeat[5];
extern bool alarmRepeatDays[5][7];
#endif