#ifndef media_h
#define media_h

#include "main.h"
// extern uint8_t cursor;
// extern int32_t cameraY;
// extern int32_t cameraY_actual;
extern String audioFiles[255];
extern uint8_t audioCount;
extern String mediaItems[3];
extern String photoFiles[255];
extern uint8_t photoCount;

void mediaApp();
int8_t mediaMenu(String* title, uint8_t length);
void mediaMenuDrawBox(String title, uint8_t i, int32_t y);
void mediaMenuDrawCursor(uint8_t i, int32_t y,  bool pressed);
int16_t audioPlayerMenu(const char* title, String* items, uint16_t length, uint16_t index = 0);
void listAudio(const char * dirname, uint8_t levels);
void audioPlayer(uint16_t index);
void drawJpeg(String filename, int xpos, int ypos);
void listPhotos(const char *dirname, uint8_t levels);
void jpegRender(int xpos, int ypos);
void jpegInfo();

#endif