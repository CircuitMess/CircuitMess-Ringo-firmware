#include "contactsApp.h"
//Contacts app
uint8_t deleteContact(String contact, String number, String id)
{
	unsigned long elapsedMillis = millis();
	bool blinkState = 1;
	while (1)
	{
		mp.display.fillScreen(TFT_BLACK);
        mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
        mp.display.setTextFont(2);
        mp.display.setCursor(0,-2);
        mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Delete contact");

		if (millis() - elapsedMillis >= multi_tap_threshold) {
		elapsedMillis = millis();
		blinkState = !blinkState;
		}

		mp.display.setTextColor(TFT_WHITE);
		mp.display.setCursor(4, 17);
		mp.display.print("Are you sure?");
		mp.display.setCursor(4, 33);
		mp.display.print(contact);
		mp.display.setCursor(4, 49);
		mp.display.print(number);

		if (blinkState){
			mp.display.drawRect(mp.display.width() / 2 - 29, 102, 30*2, 9*2, TFT_RED);
			mp.display.setTextColor(TFT_RED);
			mp.display.setCursor(28*2, 103);
			mp.display.printCenter("DELETE");
		}
		else {
			mp.display.fillRect(mp.display.width() / 2 - 29, 102, 30*2, 9*2, TFT_RED);
			mp.display.setTextColor(TFT_WHITE);
			mp.display.setCursor(28*2, 103);
			mp.display.print("DELETE");
		}
		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			Serial.println("Go back");
			mp.update();
			break;
		}
		if (mp.buttons.released(BTN_A)) // DELETE
		{
			Serial.println("DElete");
			mp.display.fillScreen(TFT_BLACK);
			mp.display.setTextFont(2);
			mp.display.setCursor(34, mp.display.height()/2 -16);
			mp.display.printCenter("Deleting contact...");
			mp.update();

			Serial1.print("AT+CPBW=");
			Serial1.println(id);
			Serial.print("AT+CPBW=");
			Serial.println(id);

			while (Serial1.readString().indexOf("OK") != -1);
			mp.display.fillScreen(TFT_BLACK);
			mp.display.printCenter("Contact deleted!");
			Serial.println("Contact deleted");
			mp.update();
			delay(1000);
			return 1;
		}

		mp.update();
	}
	return 0;
}

uint8_t newContact()
{
	textPointer = 0;
	uint8_t y = 20;  //Beggining point
	String content = "";
	String contact = "";
	String prevContent = "";
	char key = NO_KEY;
	bool cursor = 0; //editing contacts or text content
	unsigned long elapsedMillis = millis();
	bool blinkState = 1;
	while (1)
	{
		mp.display.fillScreen(TFT_BLACK);
        mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
        mp.display.setTextFont(2);
        mp.display.setCursor(0,-2);
        mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Contacts");
		if (millis() - elapsedMillis >= multi_tap_threshold) //cursor blinking routine
		{
		elapsedMillis = millis();
		blinkState = !blinkState;
		}
		if (cursor == 0) //inputting the contact number
		{
			key = mp.buttons.getKey();
			if (mp.buttons.pressed(BTN_FUN_RIGHT)) //clear number
				contact = "";
			else if (mp.buttons.pressed(BTN_FUN_LEFT))
				contact.remove(contact.length() - 1);
			if (key != NO_KEY && isdigit(key) && contact.length() < 14)
				contact += key;
			mp.display.setTextWrap(1);
			mp.display.setCursor(4, 20);
			mp.display.setTextFont(2);
			if (content == "")
			{
				mp.display.setTextColor(TFT_LIGHTGREY);
				mp.display.print(F("Name"));
				mp.display.setTextColor(TFT_WHITE);
			}
			else
				mp.display.print(content);
			mp.display.setTextFont(2);
			mp.display.setCursor(4, 38);
			mp.display.print("Num: ");
			mp.display.print(contact);
			if (blinkState == 1)
				mp.display.drawFastVLine(mp.display.getCursorX() + 1, mp.display.getCursorY() + 3, 11, TFT_WHITE);
		}
		else //inputting contact name
		{
			mp.display.setTextColor(TFT_WHITE);
			mp.display.setCursor(2*2, 38);
			mp.display.print("Num: ");
			if (contact == "")
			{
				mp.display.setTextColor(TFT_LIGHTGREY);
				mp.display.print(F("xxxxxxxx"));
				mp.display.setTextColor(TFT_WHITE);
			}
			else
				mp.display.print(contact);
			prevContent = content;
			content = mp.textInput(content, 12);
			if (prevContent != content)
			{
				blinkState = 1;
				elapsedMillis = millis();
			}
			mp.display.setTextColor(TFT_LIGHTGREY);
			mp.display.setTextWrap(1);
			mp.display.setCursor(2*2, 10*2);
			mp.display.print(content);
			mp.display.setTextColor(TFT_WHITE);
			if(blinkState == 1)
				mp.display.drawFastVLine(mp.display.getCursorX() + 1, mp.display.getCursorY() + 3, 11, TFT_WHITE);
		}

		mp.display.fillRect(mp.display.width() / 2 - 29, 102, 30*2, 9*2, TFT_GREENYELLOW);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.setCursor(31*2, 103);
		mp.display.printCenter("SAVE");

		if (mp.buttons.released(BTN_DOWN) && cursor == 1) { //BUTTON UP
		mp.update();
		cursor = 0;
		}

		if (mp.buttons.released(BTN_UP) && cursor == 0) { //BUTTON DOWN
		mp.update();
		cursor = 1;
		}

		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
		mp.update();
		break;
		}
		if (mp.buttons.released(BTN_A)) // SAVE CONTACT
		{
			if(contact != "" && content != "")
			{
				// international numbers ?
				// AT+CPBW=,”6187759088″,129,”Adam”

				mp.display.fillScreen(TFT_BLACK);
				mp.display.setCursor(34, mp.display.height()/2 -16);
				mp.display.printCenter("Inserting contact");
				mp.update();

				Serial1.print("AT+CPBW=,\"");
				Serial1.print(contact);
				Serial1.print("\",129,\"");
				Serial1.print(content);
				Serial1.println("\"");

				while (Serial1.readString().indexOf("OK") != -1);
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setCursor(34, mp.display.height()/2 -16);
				mp.display.printCenter("Contact inserted");
				mp.update();
				delay(1000);
				return 1;
			}
		}
		mp.update();
	}
	return 0;

}


void contactsMenuDrawBox(String contact, String number, uint8_t i, int32_t y) {
    uint8_t offset = 19;
    uint8_t boxHeight = 28;
	y += i * boxHeight + offset;
	if (y < 0 || y > mp.display.height()) {
		return;
	}
    mp.display.setTextSize(1);
    mp.display.setTextFont(2);
    mp.display.fillRect(1, y + 1, mp.display.width() - 2, boxHeight-1, TFT_DARKGREY);
    mp.display.setTextColor(TFT_WHITE);
    mp.display.setCursor(2, y + 2);
    mp.display.drawString(contact, 4, y);
    mp.display.drawString(number, 4, y + 12);

}
void contactsMenuDrawCursor(uint8_t i, int32_t y) {
    uint8_t offset = 19;
    uint8_t boxHeight = 28;
	if (millis() % 500 <= 250) {
		return;
	}
	y += i * boxHeight + offset;
	mp.display.drawRect(0, y, mp.display.width(), boxHeight + 1, TFT_RED);
}
void contactsMenuNewBoxCursor(uint8_t i, int32_t y) {
	uint8_t offset = 19;
    uint8_t boxHeight = 28;
	if (millis() % 500 <= 250) {
		return;
	}
	y += offset + 1;
	mp.display.drawRect(0, y, mp.display.width(), boxHeight, TFT_RED);
}
void contactsMenuNewBox(uint8_t i, int32_t y) {
	uint8_t offset = 19;
    uint8_t boxHeight = 28;
	y += offset + 1;
	if (y < 0 || y > mp.display.height()) {
		return;
	}
    mp.display.fillRect(1, y + 1, mp.display.width() - 2, boxHeight - 2, TFT_DARKGREY);
    mp.display.drawBitmap(0, y + 2, newContactIcon, TFT_WHITE);
    mp.display.setTextColor(TFT_WHITE);
    mp.display.setCursor(12, y + 3);
    mp.display.setTextFont(1);
    mp.display.print("New contact");
    mp.display.setFreeFont(TT1);
    mp.display.setTextSize(1);
    mp.display.fillRect(1, y + 1, mp.display.width() - 2, boxHeight - 2, TFT_DARKGREY);
    mp.display.drawBitmap(2, y + 4, newContactIcon, TFT_WHITE, 2);
    mp.display.setTextColor(TFT_WHITE);
    mp.display.setCursor(32, y + 6);
    mp.display.setTextFont(2);
    mp.display.print("New contact");

}
int contactsMenu(const char* title, String* contact, String *number, uint8_t length) {
	Serial.println("contactsMenu()");
	uint8_t cursor = 0;
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	uint8_t offset = 19;
    uint8_t boxHeight = 28;
	while (1) {
		mp.update();
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, 0);
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) {
		cameraY_actual = cameraY;
		}

		for (uint8_t i = 0; i < length + 1; i++) {
		if(i == 0){
			contactsMenuNewBox(i, cameraY_actual);
		} else {
			contactsMenuDrawBox(contact[i-1], number[i-1], i, cameraY_actual);
		}
		}
		if(cursor == 0){
		contactsMenuNewBoxCursor(cursor, cameraY_actual);
		} else {
		contactsMenuDrawCursor(cursor, cameraY_actual);
		}

        mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
        mp.display.setTextFont(2);
        mp.display.setCursor(0,-2);
        mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print(title);

		if (mp.buttons.released(BTN_A)) {   //BUTTON CONFIRM
		mp.update();// Exit when pressed
		break;
		}
		if (mp.buttons.released(BTN_LEFT) && cursor != 0) {
		mp.update(); // Delete
		return -1000 + cursor;
		}
		if (mp.buttons.released(BTN_RIGHT) && cursor != 0) {
		mp.update(); // Edit contact
		return -3000 + cursor;
		}

		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
		mp.update();
		if (cursor == 0) {
			cursor = length;
			if (length > 2) {
			cameraY = -(cursor - 2) * (boxHeight+1);
			}
		}
		else {
			cursor--;
			if (cursor > 0 && (cursor * (boxHeight+1) + cameraY + offset) < (boxHeight+1)) {
			cameraY += (boxHeight+1);
			}
		}
		}

		if (mp.buttons.released(BTN_DOWN)) { //BUTTON DOWN
		mp.update();
		cursor++;
		if ((cursor * (boxHeight+1) + cameraY + offset) > 48) {
			cameraY -= (boxHeight+1);
		}
		if (cursor >= length + 1) {
			cursor = 0;
			cameraY = 0;

		}
		}
		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
		mp.update();
		return -2;
		}
	}
	return cursor;
}
void contactsApp() {
	delay(5);
	Serial.println("Loaded ?");
	int change = 0;
	String input = readAllContacts();
	int count_try = 0;
	while (input == "") {
		if(count_try > 0) delay(1000);
		if(count_try > 4) return;
		Serial.println("try again");
		input = readAllContacts();
		count_try++;
	}
	if (input.indexOf("CPBR:") == -1)
	{
		mp.display.fillScreen(TFT_BLACK);
        mp.display.setCursor(0, mp.display.height()/2 - 16);
        mp.display.setTextFont(2);
		mp.display.printCenter("No contacts  :(");
		while (mp.buttons.released(BTN_B) == 0)//BUTTON BACK
		mp.update();
		mp.update();
	}
	else
	{
		uint8_t contactNumber = countSubstring(input, "CPBR:");
		Serial.println(contactNumber);

		/////////////////////////////////
		//Variables for contact parsing
		////////////////////////////////
		String phoneNumber[contactNumber];
		String contactName[contactNumber];
		String contact_id[contactNumber];
		uint16_t start;
		uint16_t end = 0;
		uint16_t foo = 0;
		uint16_t bar = 0;
		/////////////////////////////////////////////////////
		//parsing the raw data input for contact number,
		//date and text content
		////////////////////////////////////////////////////
		Serial.println(input);
		for (uint8_t i = 0; i < contactNumber; i++)
		{
			foo = input.indexOf(" ", input.indexOf("CPBR:", end));
			bar = input.indexOf("\"", input.indexOf("CPBR:", end));
			contact_id[i] = input.substring(foo+1, bar-1);

			start = input.indexOf("\"", input.indexOf("CPBR:", end));
			end = input.indexOf("\"", start + 1);
			phoneNumber[i] = input.substring(start + 1, end);

			start = input.indexOf("\"", end + 1);
			end = input.indexOf("\"", start + 1);
			contactName[i] = input.substring(start + 1, end);
		}

		while(1){
			int menuChoice = -1;
			if(change == 1 || change == -10){
				if(change == 1)
				{
					mp.display.fillScreen(TFT_BLACK);
					mp.display.setTextFont(2);
					mp.display.setCursor(34, mp.display.height()/2 - 16);
					mp.display.printCenter("Reloading data...");
					mp.update();

					delay(1000);

					input = "";
					int count_try = 0;
					bool flag = 0;
					while (input == "") {
						if(count_try > 4) { flag = 1; break; }
						delay(1000);
						Serial.println("try again");
						input = readAllContacts();
						count_try++;
					}

					if (input.indexOf("CPBR:") == -1 || flag) {
						mp.display.fillScreen(TFT_BLACK);
						mp.display.setCursor(16, 35);
						mp.display.setFreeFont(TT1);
						mp.display.print("No contacts  :(");
						while (mp.buttons.released(BTN_B) == 0) //BUTTON BACK
						mp.update();
						mp.update();
						break;
					}
				}

				uint8_t contactNumber = countSubstring(input, "CPBR:");
				Serial.println(contactNumber);

				/////////////////////////////////
				//Variables for contact parsing
				////////////////////////////////
				// String phoneNumber[contactNumber];
				// String contactName[contactNumber];
				// String contact_id[contactNumber];
				uint16_t start;
				uint16_t end = 0;
				uint16_t foo = 0;
				uint16_t bar = 0;
				/////////////////////////////////////////////////////
				//parsing the raw data input for contact number,
				//date and text content
				////////////////////////////////////////////////////
				Serial.println(input);
				for (uint8_t i = 0; i < contactNumber; i++)
				{
					foo = input.indexOf(" ", input.indexOf("CPBR:", end));
					bar = input.indexOf("\"", input.indexOf("CPBR:", end));
					contact_id[i] = input.substring(foo+1, bar-1);

					start = input.indexOf("\"", input.indexOf("CPBR:", end));
					end = input.indexOf("\"", start + 1);
					phoneNumber[i] = input.substring(start + 1, end);

					start = input.indexOf("\"", end + 1);
					end = input.indexOf("\"", start + 1);
					contactName[i] = input.substring(start + 1, end);
				}

				Serial.print("done parsing data\ncontact number:");
				Serial.println(contactNumber);
				Serial.println(sizeof(contactName));
				for(int i = 0; i<contactNumber; i++){
					Serial.println(contactName[i]);
					delay(5);
				}

				menuChoice = contactsMenu("Contacts", contactName, phoneNumber, contactNumber);
				change = -10;
			} else menuChoice = contactsMenu("Contacts", contactName, phoneNumber, contactNumber);

			mp.update();
			if (menuChoice != -2)
			{
				Serial.println(menuChoice);
				if (menuChoice == 0){
				if(newContact()){
					change = 1;
				}
				} else if (menuChoice < -1000){
					Serial.println("Edit this concat");
				} else if (menuChoice < -10){
					int id = menuChoice + 1000 - 1;
					if(deleteContact(contactName[id], phoneNumber[id], contact_id[id])){
						change = 1;
					}
				} else {
					callNumber(phoneNumber[menuChoice - 1]);
					mp.update();
				}
			} else {
				break;
			}
		}
	}
}

uint8_t deleteContactSD(String name, String number)
{
	unsigned long elapsedMillis = millis();
	bool blinkState = 1;
	while (1)
	{
		mp.display.fillScreen(TFT_BLACK);
        mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
        mp.display.setTextFont(2);
        mp.display.setCursor(0,-2);
        mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Delete contact");

		if (millis() - elapsedMillis >= multi_tap_threshold) {
		elapsedMillis = millis();
		blinkState = !blinkState;
		}

		mp.display.setTextColor(TFT_WHITE);
		mp.display.setCursor(4, 17);
		mp.display.print("Are you sure?");
		mp.display.setCursor(4, 33);
		mp.display.print(name);
		mp.display.setCursor(4, 49);
		mp.display.print(number);

		if (blinkState){
			mp.display.drawRect(mp.display.width() / 2 - 29, 102, 30*2, 9*2, TFT_RED);
			mp.display.setTextColor(TFT_RED);
			mp.display.setCursor(28*2, 103);
			mp.display.printCenter("DELETE");
		}
		else {
			mp.display.fillRect(mp.display.width() / 2 - 29, 102, 30*2, 9*2, TFT_RED);
			mp.display.setTextColor(TFT_WHITE);
			mp.display.setCursor(28*2, 103);
			mp.display.print("DELETE");
		}

		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			Serial.println("Go back");
			mp.update();
			break;
		}
		if (mp.buttons.released(BTN_A)) // DELETE
		{
			Serial.println("DElete");
			mp.display.fillScreen(TFT_BLACK);
			mp.display.setTextFont(2);
			mp.display.setCursor(34, mp.display.height()/2 -16);
			mp.display.printCenter("Deleting contact...");
			mp.update();
			return 1;
		}
		mp.update();
	}
	return 0;
}

void contactsAppSD(){
	Serial.println("");
	Serial.println("Begin contacts");
	SDAudioFile file = mp.SD.open("/.core/contacts.json", "r");

	if(file.size() < 2){ // empty -> FILL
		Serial.println("Override");
		file.close();
		// JsonArray& jarr = jb.parseArray("[{\"name\":\"foo\", \"number\":\"099\"}]");
		JsonArray& jarr = jb.createArray();
		delay(10);
		SDAudioFile file1 = mp.SD.open("/.core/contacts.json", "w");
		jarr.prettyPrintTo(file1);
		file1.close();
		file = mp.SD.open("/.core/contacts.json", "r");
		while(!file)
			Serial.println("CONTACTS ERROR");
	}

	JsonArray& jarr = jb.parseArray(file);

	if(!jarr.success())
	{
		Serial.println("Error");
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, mp.display.height()/2 - 16);
		mp.display.setTextFont(2);
		mp.display.printCenter("Error loading contacts");
		while (mp.buttons.released(BTN_B) == 0)//BUTTON BACK
		mp.update();
		mp.update();
	}
	else
	{
		// Maybe later read from core.json
		// uint8_t contactNumber = countSubstring(input, "CPBR:");
		// Serial.println(contactNumber);

		Serial.println("Read from .json");
		for (JsonObject& elem : jarr) {
			Serial.println(elem["name"].as<char*>());
			Serial.println(elem["number"].as<char*>());
		}

		while(1){
			int menuChoice = -1;
			menuChoice = contactsMenuSD(&jarr);

			mp.update();
			if (menuChoice != -2) //creating new contact
			{
				if (menuChoice == 0){
					String name, number;
					if(newContactSD(&name, &number)){
						JsonObject& newContact = jb.createObject();
						newContact["name"] = name;
						newContact["number"] = number;
						jarr.add(newContact);
						SDAudioFile file = mp.SD.open("/.core/contacts.json", "w");
						jarr.prettyPrintTo(file);
						jarr.prettyPrintTo(Serial);
						file.close();
					}
				}
				else if(menuChoice < -2000) // editing contact
				{
					int id = menuChoice + 3000 - 1;
					String name, number;
					name = jarr[id]["name"].as<String>();
					number = jarr[id]["number"].as<String>();

					if(newContactSD(&name, &number)){
						JsonObject& newContact = jb.createObject();
						newContact["name"] = name;
						newContact["number"] = number;
						jarr[id] = newContact;
						SDAudioFile file = mp.SD.open("/.core/contacts.json", "w");
						jarr.prettyPrintTo(file);
						jarr.prettyPrintTo(Serial);
						file.close();
					}
					// if(deleteContactSD(jarr[id]["name"], jarr[id]["number"])){
					// 	jarr.remove(id);
					// 	SDAudioFile file = mp.SD.open("/.core/contacts.json", "w");
					// 	jarr.prettyPrintTo(file);
					// 	file.close();
					// }
				}
				else if (menuChoice < -10){
					int id = menuChoice + 1000 - 1;
					if(deleteContactSD(jarr[id]["name"], jarr[id]["number"])){
						jarr.remove(id);
						SDAudioFile file = mp.SD.open("/.core/contacts.json", "w");
						jarr.prettyPrintTo(file);
						file.close();
					}
				// EDIT
				} else {
					if(mp.simInserted)
						callNumber(jarr[menuChoice - 1]["number"].as<char*>());
					else
					{
						mp.display.setTextColor(TFT_BLACK);
						mp.display.setTextSize(1);
						mp.display.setTextFont(2);
						mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
						mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
						mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
						mp.display.setCursor(47, 55);
						mp.display.printCenter(F("SIM card missing!"));
						uint32_t tempMillis = millis();
						while(millis() < tempMillis + 2000)
						{
							mp.update();
							if(mp.buttons.pressed(BTN_A) || mp.buttons.pressed(BTN_B))
							{
								while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
									mp.update();
								break;
							}
						}
						mp.update();
					}
					
					mp.update();
				}
			} else {
				break;
			}
		}
	}
}

uint8_t newContactSD(String *name, String *number)
{
	mp.textInput("");
	mp.textPointer = 0;
	String content = *name;
	String contact = *number;
	String prevContent = "";
	char key = NO_KEY;
	bool cursor = 0; //editing contacts or text content
	unsigned long elapsedMillis = millis();
	bool blinkState = 1;
	while (1) {

		mp.display.fillScreen(TFT_BLACK);
		mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
		mp.display.setTextFont(2);
		mp.display.setCursor(0,-2);
		mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Contacts");
		if (millis() - elapsedMillis >= multi_tap_threshold) //cursor blinking routine
		{
			elapsedMillis = millis();
			blinkState = !blinkState;
		}
		if (cursor == 0) //inputting the contact number
		{
			key = mp.buttons.getKey();
			if (mp.buttons.released(BTN_FUN_RIGHT)) //clear number
				contact = "";
			else if (mp.buttons.released(BTN_FUN_LEFT))
				contact.remove(contact.length() - 1);
			if (key != NO_KEY && isdigit(key) && contact.length() < 14)
				contact += key;
			mp.display.setTextWrap(1);
			mp.display.setCursor(4, 20);
			mp.display.setTextFont(2);
			if (content == "")
			{
				mp.display.setTextColor(TFT_LIGHTGREY);
				mp.display.print(F("Name"));
				mp.display.setTextColor(TFT_WHITE);
			}
			else
				mp.display.print(content);
			mp.display.setTextFont(2);
			mp.display.setCursor(4, 38);
			mp.display.print("Num: ");
			mp.display.print(contact);
			if (blinkState == 1)
				mp.display.drawFastVLine(mp.display.getCursorX() + 1, mp.display.getCursorY() + 3, 11, TFT_WHITE);
		}
		else //inputting contact name
		{
			mp.display.setTextColor(TFT_WHITE);
			mp.display.setCursor(2*2, 38);
			mp.display.print("Num: ");
			if (contact == "")
			{
				mp.display.setTextColor(TFT_LIGHTGREY);
				mp.display.print(F("xxxxxxxx"));
				mp.display.setTextColor(TFT_WHITE);
			}
			else
				mp.display.print(contact);
			prevContent = content;
			content = mp.textInput(content, 12);
			if (prevContent != content)
			{
				blinkState = 1;
				elapsedMillis = millis();
			}
			mp.display.setTextColor(TFT_LIGHTGREY);
			mp.display.setTextWrap(1);
			mp.display.setCursor(2*2, 10*2);
			mp.display.print(content);
			mp.display.setTextColor(TFT_WHITE);
			if(blinkState == 1)
				mp.display.drawFastVLine(mp.display.getCursorX() + 1, mp.display.getCursorY() + 3, 11, TFT_WHITE);
		}

		mp.display.fillRect(mp.display.width() / 2 - 29, 102, 30*2, 9*2, TFT_GREENYELLOW);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.setCursor(31*2, 103);
		mp.display.printCenter("SAVE");

		if (mp.buttons.released(BTN_DOWN) && cursor == 1) { //BUTTON UP
			mp.update();
			cursor = 0;
		}

		if (mp.buttons.released(BTN_UP) && cursor == 0) { //BUTTON DOWN
			mp.update();
			cursor = 1;
		}

		if (mp.buttons.released(BTN_B)) { //BUTTON BACK
			mp.update();
			break;
		}
		if (mp.buttons.released(BTN_A)) // SAVE CONTACT
		{
			mp.update();
			if(contact != "" && content != "")
			{
				*name = contact;
				*number = content;
				return 1;
			}
		}
		mp.update();
	}
	return 0;
}

int contactsMenuSD(JsonArray *contacts){
	uint8_t cursor = 0;
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	uint8_t length = contacts->size();
	uint8_t offset = 19;
	uint8_t boxHeight = 28;
	while (1) {
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, 0);
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
		}

		contactsMenuNewBox(0, cameraY_actual);

		int i = 0;
		for (JsonObject& elem : *contacts) {
			contactsMenuDrawBoxSD(elem["name"].as<char*>(), elem["number"].as<char*>(), i+1, cameraY_actual);
			i++;
		}
		if(cursor == 0){
			contactsMenuNewBoxCursor(cursor, cameraY_actual);
		} else {
			contactsMenuDrawCursor(cursor, cameraY_actual);
		}

		// last draw the top entry thing
		mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
		mp.display.setTextFont(2);
		mp.display.setCursor(0,-2);
		mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Contacts");
		mp.display.fillRect(0, 103, 160, 28, TFT_BLACK);
		mp.display.setCursor(110, 110);
		mp.display.printCenter("Delete              Edit");

		if (mp.buttons.released(BTN_A)) {   //BUTTON CONFIRM
			mp.update();// Exit when pressed
			break;
		}
		if (mp.buttons.released(BTN_FUN_LEFT) && cursor != 0) {
			mp.update(); // Delete
			return -1000 + cursor;
		}
		if (mp.buttons.released(BTN_FUN_RIGHT) && cursor != 0) {
			mp.update(); // Edit contact
			return -3000 + cursor;
		}

		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
			mp.update();
			if (cursor == 0) {
				cursor = length;
				if (length > 2) {
				cameraY = -(cursor - 2) * (boxHeight+1);
				}
			}
			else {
				cursor--;
				if (cursor > 0 && (cursor * (boxHeight+1) + cameraY + offset) < (boxHeight+1)) {
				cameraY += (boxHeight+1);
				}
			}
		}

		if (mp.buttons.released(BTN_DOWN)) { //BUTTON DOWN
			mp.update();

			cursor++;
			if ((cursor * (boxHeight+1) + cameraY + offset) > 48) {
				cameraY -= (boxHeight+1);
			}
			if (cursor >= length + 1) {
				cursor = 0;
				cameraY = 0;
			}
		}
		if (mp.buttons.released(BTN_B) == 1) {
			mp.update();
			return -2;
		}

		mp.update();
	}
	return cursor;
}

void contactsMenuDrawBoxSD(String name, String number, uint8_t i, int32_t y) {
    uint8_t offset = 19;
    uint8_t boxHeight = 28;
	y += i * boxHeight + offset;
	if (y < 0 || y > mp.display.height()) {
		return;
	}
    mp.display.setTextSize(1);
    mp.display.setTextFont(2);
    mp.display.fillRect(1, y + 1, mp.display.width() - 2, boxHeight-1, TFT_DARKGREY);
    mp.display.setTextColor(TFT_WHITE);
    mp.display.setCursor(2, y + 2);
    mp.display.drawString(name, 4, y);
    mp.display.drawString(number, 4, y + 12);

}

String readAllContacts() {
	String buffer;
	Serial1.print(F("AT+CPBR=1,250\r"));
	buffer = readSerial();
	delay(5);
	if (buffer.indexOf("CPBR:") != -1 || buffer.indexOf("OK") != -1) {
		return buffer;
	}
	else return "";
}

