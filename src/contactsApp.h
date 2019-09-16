#ifndef contacts_h
#define contacts_h
#include "main.h"
//Contacts app
void contactsMenuDrawBoxSD(String name, String number, uint8_t i, int32_t y);
uint8_t deleteContactSD(String name, String number);
uint8_t newContact(String *name, String *number);
void parse_contacts();
void contactsMenuNewBox(uint8_t i, int32_t y);
void contactsMenuDrawCursor(uint8_t i, int32_t y);
void contactsMenuNewBoxCursor(uint8_t i, int32_t y);
int contactsMenuSD(JsonArray *contacts, bool smsFlag);
void contactsAppSD(bool smsFlag);
String readAllContacts();
void callNumber(String number);
int8_t viewContact(JsonObject &object);
extern int textPointer;
String searchContacts(String input);
void contactsMenuSearchBox(uint8_t i, int32_t y, String input); 
void contactsMenuSearchBoxCursor(uint8_t i, int32_t y);
#endif
