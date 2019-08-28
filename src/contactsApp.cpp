#include "contactsApp.h"
//Contacts app
uint8_t contactsMenuCursor = 0;
bool helpPop;

void contactsMenuDrawCursor(uint8_t i, int32_t y) {
    uint8_t offset = 19;
    uint8_t boxHeight = 28;
	y += i * boxHeight + offset;
	mp.display.drawRect(0, y, mp.display.width(), boxHeight + 1, TFT_RED);
}
void contactsMenuNewBoxCursor(uint8_t i, int32_t y) {
	uint8_t offset = 19;
    uint8_t boxHeight = 28;
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

uint8_t deleteContactSD(String name, String number)
{
	// unsigned long elapsedMillis = millis();
	// bool blinkState = 1;
	while (1)
	{
		mp.display.fillScreen(TFT_BLACK);
        mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
        mp.display.setTextFont(2);
        mp.display.setCursor(0,-2);
        mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Delete contact");

		// if (millis() - elapsedMillis >= multi_tap_threshold) {
		// elapsedMillis = millis();
		// blinkState = !blinkState;
		// }

		mp.display.setTextColor(TFT_WHITE);
		mp.display.setCursor(4, 17);
		mp.display.print("Are you sure?");
		mp.display.setCursor(4, 33);
		mp.display.print(name);
		mp.display.setCursor(4, 49);
		mp.display.print(number);
		mp.display.drawFastHLine(0, 112, BUF2WIDTH, TFT_WHITE);
		mp.display.fillRect(0, 113, mp.display.width(), 30, TFT_DARKGREY);
		mp.display.setCursor(110, 113);
		mp.display.print("Confirm");
		// if (blinkState){
		// 	mp.display.drawRect(mp.display.width() / 2 - 29, 102, 30*2, 9*2, TFT_RED);
		// 	mp.display.setTextColor(TFT_RED);
		// 	mp.display.setCursor(28*2, 103);
		// 	mp.display.printCenter("DELETE");
		// }
		// else {
		// 	mp.display.fillRect(mp.display.width() / 2 - 29, 102, 30*2, 9*2, TFT_RED);
		// 	mp.display.setTextColor(TFT_WHITE);
		// 	mp.display.setCursor(28*2, 103);
		// 	mp.display.print("DELETE");
		// }

		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			Serial.println("Go back");
			while(!mp.update());
			break;
		}
		if (mp.buttons.released(BTN_A) || mp.buttons.released(BTN_FUN_RIGHT)) // DELETE
		{
			while(!mp.update());
			// Serial.println("Delete");
			// mp.display.fillScreen(TFT_BLACK);
			// mp.display.setTextFont(2);
			// mp.display.setCursor(34, mp.display.height()/2 -16);
			// mp.display.printCenter("Deleting contact...");
			// mp.update();
			return 1;
		}
		mp.update();
	}
	return 0;
}

void contactsAppSD(){
	Serial.println("");
	Serial.println("Begin contacts");
	File file = SD.open("/.core/contacts.json", "r");

	if(file.size() < 2){ // empty -> FILL
		Serial.println("Override");
		file.close();
		jb.clear();
		JsonArray& jarr = jb.createArray();
		delay(10);
		File file1 = SD.open("/.core/contacts.json", "w");
		jarr.prettyPrintTo(file1);
		file1.close();
		file = SD.open("/.core/contacts.json", "r");
		while(!file)
			Serial.println("CONTACTS ERROR");
	}

	jb.clear();
	JsonArray& jarr = jb.parseArray(file);
	file.close();

	if(!jarr.success())
	{
		Serial.println("Error");
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, mp.display.height()/2 - 16);
		mp.display.setTextFont(2);
		mp.display.printCenter("Error loading contacts");
		while (mp.buttons.released(BTN_B) == 0)//BUTTON BACK
			mp.update();
		while(!mp.update());
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
			if (menuChoice != -2) 
			{
				if (menuChoice == 0) //creating new contact
				{
					String name;
					String number = "+";
					if(newContactSD(&name, &number))
					{
						JsonObject& newContact = jb.createObject();
						newContact["name"] = name;
						newContact["number"] = number;
						jarr.add(newContact);
						File file = SD.open("/.core/contacts.json", "w");
						jarr.prettyPrintTo(file);
						jarr.prettyPrintTo(Serial);
						file.close();
					}
				}
				else if(menuChoice < -2000) // view contact
				{
					int id = menuChoice + 3000 - 1;
					int8_t info = viewContact(jarr[id]);
					if(info == -1)
					{
						String name, number;
						name = jarr[id]["name"].as<String>();
						number = jarr[id]["number"].as<String>();

						if(newContactSD(&name, &number)){
							JsonObject& newContact = jb.createObject();
							newContact["name"] = name;
							newContact["number"] = number;
							jarr[id] = newContact;
							File file = SD.open("/.core/contacts.json", "w");
							jarr.prettyPrintTo(file);
							jarr.prettyPrintTo(Serial);
							file.close();
						}
					}
					else if(info == 1) //call number
					{
						if(mp.sim_module_version == 255)
						{
							mp.display.fillScreen(TFT_BLACK);
							mp.display.setTextColor(TFT_WHITE);
							mp.display.setTextSize(1);
							mp.display.setCursor(0, mp.display.height()/2 - 20);
							mp.display.setTextFont(2);
							mp.display.printCenter(F("No network board!"));
							mp.display.setCursor(0, mp.display.height()/2);
							mp.display.printCenter(F("Insert board and reset"));
							uint32_t tempMillis = millis();
							while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
								mp.update();
							while(!mp.update());
						}
						else if(!mp.simInserted)
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
							while(!mp.update());
						}
						else
							callNumber(jarr[id]["number"].as<char*>());
					}
				}
				else if (menuChoice < -10) //deleting contact
				{
					int id = menuChoice + 1000 - 1;
					if(deleteContactSD(jarr[id]["name"], jarr[id]["number"]))
					{
						jarr.remove(id);
						File file = SD.open("/.core/contacts.json", "w");
						jarr.prettyPrintTo(file);
						file.close();
						contactsMenuCursor = 0;
					}
				}
				else
				{
					
					while(!mp.update());
				}
			}
			else
			{
				contactsMenuCursor = 0;
				break;
			}
		}
	}
}


uint8_t newContactSD(String *name, String *number)
{
	mp.textInput("");
	String content = *name;
	String contact = *number;
	String prevContent = "";
	mp.textPointer = name->length();
	char key = NO_KEY;
	bool cursor = 0; //editing contacts or text content
	unsigned long elapsedMillis = millis();
	bool blinkState = 1;
	// bool plusSign = 0;
	while (1) {

		mp.display.fillScreen(TFT_BLACK);
		mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
		mp.display.setTextFont(2);
		mp.display.setCursor(0,-2);
		mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Contacts");
		mp.display.fillRect(0,60,160,50, TFT_DARKGREY);
		mp.display.setCursor(4, 60);
		mp.display.print("Enter your country code");
		mp.display.setCursor(4, 76);
		mp.display.printCenter("(example: +001 for USA)");
		mp.display.setCursor(4, 92);
		mp.display.printCenter("Press A to save");


		if(mp.buttons.released(BTN_HOME)) {
			mp.exitedLockscreen = true;
			mp.lockscreen(); // Robert
		}
		if (millis() - elapsedMillis >= multi_tap_threshold) //cursor blinking routine
		{
			elapsedMillis = millis();
			blinkState = !blinkState;
		}
		if (cursor == 0) //inputting the contact number
		{
			key = mp.buttons.getKey();
			// if(mp.buttons.held(BTN_0, 20))
			// {
			// 	contact+="+";
			// 	plusSign = 1;
			// }
			// if(key == '0' && plusSign)
				// key = NO_KEY;
			// if(plusSign && mp.buttons.released(BTN_0))
				// plusSign = 0;
			if (mp.buttons.released(BTN_FUN_LEFT) && contact.length() > 1)
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
			if (contact == "+")
			{
				mp.display.setTextColor(TFT_LIGHTGREY);
				mp.display.print(F("+XXXXXXXXXXXX"));
				mp.display.setTextColor(TFT_WHITE);
			}
			else
				mp.display.print(contact);
			prevContent = content;
			content = mp.textInput(content, 18);
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
		mp.display.setTextColor(TFT_WHITE);
		mp.display.setCursor(130, 110);
		mp.display.print("Help");
/* 		mp.display.setTextColor(TFT_WHITE);
		mp.display.setCursor(90, 110);
		mp.display.print("Help"); */
		mp.display.setTextColor(TFT_WHITE);
		mp.display.setCursor(4, 110);
		mp.display.print("Erase");
		// mp.display.fillRect(mp.display.width() / 2 - 29, 102, 30*2, 9*2, TFT_GREENYELLOW);
		// mp.display.setTextColor(TFT_WHITE);
		// mp.display.setCursor(31*2, 103);
		// mp.display.printCenter("SAVE");

		if (mp.buttons.released(BTN_DOWN) && cursor == 1) { //BUTTON UP
			while(!mp.update());
			cursor = 0;
		}

		if (mp.buttons.released(BTN_UP) && cursor == 0) { //BUTTON DOWN
			while(!mp.update());
			cursor = 1;
		}
		if(mp.buttons.released(BTN_FUN_RIGHT)){
			helpPop = !helpPop;
			mp.display.drawIcon(TextHelperPopup, 0, 0, 160, 128, 1, TFT_WHITE);	
			while(!mp.update());
		}
		while (helpPop) {
			if(mp.buttons.released(BTN_FUN_RIGHT) || mp.buttons.released(BTN_B)){
				helpPop = !helpPop;
			}
			mp.update();
		}
		if (mp.buttons.released(BTN_B)) { //BUTTON BACK
			while(!mp.update());
			break;
		}
		if (mp.buttons.released(BTN_A)) // SAVE CONTACT
		{
			while(!mp.update());
			if(contact != "+" && content != "")
			{
				*name = content;
				*number = contact;
				return 1;
			}
		}
		mp.update();
	}
	return 0;
}

int contactsMenuSD(JsonArray *contacts){
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	uint8_t length = contacts->size();
	uint8_t offset = 19;
	uint8_t boxHeight = 28;
	bool blinkState = 0;
	uint32_t blinkMillis = millis();
	if (length > 2 && contactsMenuCursor > 2) {
		cameraY = -(contactsMenuCursor - 1) * (boxHeight + 1) + offset + 11 ;
	}
	while (1) {
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, 0);
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
		}
		if(millis() - blinkMillis > 350)
		{
			blinkMillis = millis();
			blinkState = !blinkState;
		}
		contactsMenuNewBox(0, cameraY_actual);

		int i = 0;
		for (JsonObject& elem : *contacts) {
			contactsMenuDrawBoxSD(elem["name"].as<char*>(), elem["number"].as<char*>(), i+1, cameraY_actual);
			i++;
		}
		if(blinkState)
		{
			if(contactsMenuCursor == 0)
				contactsMenuNewBoxCursor(contactsMenuCursor, cameraY_actual);
			else
				contactsMenuDrawCursor(contactsMenuCursor, cameraY_actual);
		}

		// last draw the top entry thing
		mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
		mp.display.setTextFont(2);
		mp.display.setCursor(0,-2);
		mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Contacts");
		mp.display.fillRect(0, 104, 160, 28, TFT_BLACK);
		mp.display.drawFastHLine(0, 112, BUF2WIDTH, TFT_WHITE);
		mp.display.fillRect(0, 113, mp.display.width(), 30, TFT_DARKGREY);
		mp.display.setCursor(110, 113);
		mp.display.printCenter("Delete               View");

		if(mp.buttons.released(BTN_HOME)) {
			mp.exitedLockscreen = true;
			mp.lockscreen(); // Robert
		}
		if (mp.buttons.released(BTN_A) && contactsMenuCursor == 0) {   //BUTTON CONFIRM
			while(!mp.update());// Exit when pressed
			break;
		}
		if (mp.buttons.released(BTN_FUN_LEFT) && contactsMenuCursor != 0) {
			while(!mp.update()); // Delete
			return -1000 + contactsMenuCursor;
		}
		if ((mp.buttons.released(BTN_FUN_RIGHT) || mp.buttons.released(BTN_A)) && contactsMenuCursor != 0) {
			while(!mp.update()); // View contact
			return -3000 + contactsMenuCursor;
		}

		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
			blinkState = 1;
			blinkMillis = millis();
			while(!mp.update());
			if (contactsMenuCursor == 0) {
				contactsMenuCursor = length;
				if (length > 2) {
				cameraY = -(contactsMenuCursor - 2) * (boxHeight+1);
				}
			}
			else {
				contactsMenuCursor--;
				if (contactsMenuCursor > 0 && (contactsMenuCursor * (boxHeight+1) + cameraY + offset) < (boxHeight+1)) {
				cameraY += (boxHeight+1);
				}
			}
		}

		if (mp.buttons.released(BTN_DOWN)) { //BUTTON DOWN
			blinkState = 1;
			blinkMillis = millis();
			while(!mp.update());

			contactsMenuCursor++;
			if ((contactsMenuCursor * (boxHeight+1) + cameraY + offset) > 80) {
				cameraY -= (boxHeight+1);
			}
			if (contactsMenuCursor >= length + 1) {
				contactsMenuCursor = 0;
				cameraY = 0;
			}
		}
		if (mp.buttons.released(BTN_B) == 1) {
			while(!mp.update());
			return -2;
		}

		mp.update();
	}
	return contactsMenuCursor;
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
int8_t viewContact(JsonObject &contact)
{
	while (1)
	{
		mp.display.fillScreen(TFT_BLACK);
        mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
        mp.display.setTextFont(2);
        mp.display.setCursor(0,-2);
        mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Contacts");

		// if (millis() - elapsedMillis >= multi_tap_threshold) {
		// elapsedMillis = millis();
		// blinkState = !blinkState;
		// }
		mp.display.setCursor(2, 18);
		mp.display.print("Name:");
		mp.display.setCursor(4, 34);
		mp.display.print(contact["name"].as<char*>());
		mp.display.setCursor(4, 54);
		mp.display.print("Number:");
		mp.display.setCursor(2, 70);
		mp.display.print(contact["number"].as<char*>());
		mp.display.drawFastHLine(0, 112, BUF2WIDTH, TFT_WHITE);
		mp.display.fillRect(0, 113, mp.display.width(), 30, TFT_DARKGREY);
		mp.display.setCursor(110, 113);
		mp.display.printCenter("Edit                 Call");
		// if (blinkState){
		// 	mp.display.drawRect(mp.display.width() / 2 - 29, 102, 30*2, 9*2, TFT_RED);
		// 	mp.display.setTextColor(TFT_RED);
		// 	mp.display.setCursor(28*2, 103);
		// 	mp.display.printCenter("DELETE");
		// }
		// else {
		// 	mp.display.fillRect(mp.display.width() / 2 - 29, 102, 30*2, 9*2, TFT_RED);
		// 	mp.display.setTextColor(TFT_WHITE);
		// 	mp.display.setCursor(28*2, 103);
		// 	mp.display.print("DELETE");
		// }

		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			Serial.println("Go back");
			while(!mp.update());
			break;
		}
		if (mp.buttons.released(BTN_FUN_RIGHT)) // call
		{
			while(!mp.update());
			// Serial.println("Delete");
			// mp.display.fillScreen(TFT_BLACK);
			// mp.display.setTextFont(2);
			// mp.display.setCursor(34, mp.display.height()/2 -16);
			// mp.display.printCenter("Deleting contact...");
			// mp.update();
			return 1;
		}
		if(mp.buttons.released(BTN_FUN_LEFT)) //edit
		{
			while(!mp.update());
			return -1;
		}
		mp.update();
	}
	return 0;
}
