#ifndef contacts_h
#define contacts_h
#include "main.h"
//Contacts app
void contactsMenuDrawBox(String contact, String number, uint8_t i, int32_t y);
void contactsMenuDrawBoxSD(String name, String number, uint8_t i, int32_t y);
uint8_t deleteContact(String contact, String number, String id);
uint8_t deleteContactSD(String name, String number);
uint8_t newContact();
uint8_t newContactSD(String *name, String *number);
void parse_contacts();
void contactsMenuNewBox(uint8_t i, int32_t y);
void contactsMenuDrawCursor(uint8_t i, int32_t y);
void contactsMenuNewBoxCursor(uint8_t i, int32_t y);
int contactsMenu(const char* title, String* contact, String *number, uint8_t length);
int contactsMenuSD(JsonArray *contacts);
void contactsApp();
void contactsAppSD();
String readAllContacts();
void callNumber(String number);
extern int textPointer;
#endif