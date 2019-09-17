#ifndef contacts_h
#define contacts_h
#include "main.h"
//Contacts app
void contactsMenuDrawBox(String name, String number, uint8_t i, int32_t y, bool smsflag);
uint8_t deleteContactSD(String name, String number);
uint8_t newContact(String *name, String *number);
void parse_contacts();
void contactsMenuNewBox(uint8_t i, int32_t y);
void contactsMenuDrawCursor(uint8_t i, int32_t y, bool smsflag);
void contactsMenuNewBoxCursor(uint8_t i, int32_t y);
int contactsMenu(JsonArray *contacts, bool smsFlag);
void contactsApp(bool smsFlag);
String readAllContacts();
void callNumber(String number);
int8_t viewContact(JsonObject &object);
extern int textPointer;
String searchContacts(String input);
void contactsMenuSearchBox(uint8_t i, int32_t y, String input, bool blinkState); 
void contactsMenuSearchBoxCursor(uint8_t i, int32_t y);
#endif
