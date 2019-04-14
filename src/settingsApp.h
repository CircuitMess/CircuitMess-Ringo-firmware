#ifndef settings_h
#define settings_h

#include "main.h"
bool settingsApp();
int8_t settingsMenu(String* title, uint8_t length);
void settingsMenuDrawBox(String title, uint8_t i, int32_t y);
void settingsMenuDrawCursor(uint8_t i, int32_t y,  bool pressed);

void networkMenu();
void displayMenu();
void soundMenu();
void securityMenu();
void timeMenu();
bool updateMenu();
void wifiMenu();

void listRingtones(const char * dirname, uint8_t levels);
void listNotifications(const char * dirname, uint8_t levels);

extern String settingsItems[6];
extern uint8_t ringtoneCount;
extern uint8_t notificationCount;
extern String ringtoneFiles[100];
extern String notificationFiles[100];
extern uint16_t pinNumber;
extern uint8_t timesRemaining;
extern bool pinLock;

#endif