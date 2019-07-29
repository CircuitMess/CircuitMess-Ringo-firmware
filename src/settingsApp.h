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
void wifiConnect();
int8_t checkForUpdate();
bool fetchUpdate();

int8_t notificationsAudioMenu(String* items, uint8_t length);
void notificationsDrawBox(String text, uint8_t i, int32_t y);
void notificationsDrawCursor(uint8_t i, int32_t y);
int8_t wifiNetworksMenu(String* items, String *signals, uint8_t length);
void wifiDrawBox(String text, String signalStrength, uint8_t i, int32_t y);
void wifiDrawCursor(uint8_t i, int32_t y);
int16_t ringtoneAudioMenu(String* items, uint16_t length);

extern String settingsItems[6];
extern uint16_t pinNumber;
extern uint8_t timesRemaining;
extern bool pinLock;

#endif