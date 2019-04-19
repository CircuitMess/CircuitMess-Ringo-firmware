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

int8_t notificationsAudioMenu(String* items, uint8_t length);
void notificationsDrawBox(String text, uint8_t i, int32_t y);
void notificationsDrawCursor(uint8_t i, int32_t y);

extern String settingsItems[6];
extern uint16_t pinNumber;
extern uint8_t timesRemaining;
extern bool pinLock;

#endif