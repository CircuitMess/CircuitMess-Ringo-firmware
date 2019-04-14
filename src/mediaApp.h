#ifndef media_h
#define media_h

#include "main.h"
// extern uint8_t cursor;
// extern int32_t cameraY;
// extern int32_t cameraY_actual;
extern String mediaItems[3];
extern String photoFiles[100];
extern uint8_t photoCount;

void mediaApp();
int8_t mediaMenu(String* title, uint8_t length);
void mediaMenuDrawBox(String title, uint8_t i, int32_t y);
void mediaMenuDrawCursor(uint8_t i, int32_t y,  bool pressed);
void audioPlayer(uint16_t index);
void listPhotos(const char *dirname, uint8_t levels);

#endif