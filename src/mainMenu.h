#ifndef mainMenu_h
#define mainMenu_h

#include "main.h"
#include "messagesApp.h"
#include "mediaApp.h"
#include "contactsApp.h"
extern uint16_t directoryCount;
extern String directories[255];
extern String BinaryFiles[255];
extern uint16_t binaryCount;//Number of binary files available for loading

void listDirectories(const char * dirname);
void listBinaries(const char * dirname, uint8_t levels);
int16_t scrollingMainMenu();
void mainMenu();


#endif