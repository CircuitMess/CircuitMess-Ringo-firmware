#include "messagesApp.h"
int16_t y;
String smsContent[smsNumber];
String phoneNumber[smsNumber];
String tempDate[smsNumber];
uint16_t smsYear[smsNumber];
uint8_t smsDay[smsNumber];
uint8_t smsMonth[smsNumber];
uint8_t smsMinute[smsNumber];
uint8_t smsSecond[smsNumber];
uint8_t smsHour[smsNumber];
uint32_t start = 0;
uint32_t end = 0;
String input;
String buffer;
int textPointer = 0;

//Messages app
void messagesApp() {
	
	mp.dataRefreshFlag = 0;
	//sim800.println("AT+CMGL=\"REC READ\"");
	//sim800.flush();
	/*while (sim800.available())
	input += (char)sim800.read();*/
	while(!mp.simReady)
		mp.update();
	input = readAllSms();
	while (input == "")
	{
		input = readSerial();
	}
	
	if (input == "ERROR")
	{
		mp.display.fillScreen(TFT_BLACK);
        mp.display.setCursor(0, mp.display.height()/2 - 16);
        mp.display.setTextFont(2);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.printCenter("Error loading SMS!");
		while (!mp.update());
		while (!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
			mp.update();
	}
	else if (input == "OK")
	{
		mp.display.setFreeFont(TT1);
		mp.display.fillScreen(TFT_BLACK);
        mp.display.setCursor(0, mp.display.height()/2 - 16);
        mp.display.setTextFont(2);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.printCenter("No messages :(");
		while (!mp.update());
		while (!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
			mp.update();
	}
	else
	{
		Serial.println(input);
		delay(5);


		/////////////////////////////////////////////////////
		//parsing the raw input data for contact number,
		//date and text content
		////////////////////////////////////////////////////
		uint16_t numberOfTexts = countSubstring(input, "+CMGL:");
		for (uint8_t i = 0; i < numberOfTexts; i++)
		{
			start = input.indexOf("\n", input.indexOf("CMGL:", end));
			end = input.indexOf("\n", start + 1);
			smsContent[i] = input.substring(start + 1, end);
		}


		end = 0;
		for (uint8_t i = 0; i < numberOfTexts; i++)
		{
			start = input.indexOf("D\",\"", input.indexOf("CMGL:", end));
			end = input.indexOf("\"", start + 4);
			phoneNumber[i] = input.substring(start + 4, end);
		}
		//////////////////////////////////////////
		//Parsing for date and time of sending
		////////////////////////////
		end = 0;
		for (uint8_t i = 0; i < numberOfTexts; i++)
		{
			start = input.indexOf("\"\",\"", input.indexOf("CMGL:", end));
			end = input.indexOf("\"", start + 4);
			tempDate[i] = input.substring(start + 4, end);
		}

		int8_t index = -8;
		for (uint8_t i = 0; i < numberOfTexts; i++)
		{

			char c1, c2; //buffer for saving date and time numerals in form of characters
			c1 = tempDate[i].charAt(index + 8);
			c2 = tempDate[i].charAt(index + 9);
			smsYear[i] = 2000 + ((c1 - '0') * 10) + (c2 - '0');

			c1 = tempDate[i].charAt(index + 11);
			c2 = tempDate[i].charAt(index + 12);
			smsMonth[i] = ((c1 - '0') * 10) + (c2 - '0');

			c1 = tempDate[i].charAt(index + 14);
			c2 = tempDate[i].charAt(index + 15);
			smsDay[i] = ((c1 - '0') * 10) + (c2 - '0');

			c1 = tempDate[i].charAt(index + 17);
			c2 = tempDate[i].charAt(index + 18);
			smsHour[i] = ((c1 - '0') * 10) + (c2 - '0');

			c1 = tempDate[i].charAt(index + 20);
			c2 = tempDate[i].charAt(index + 21);
			smsMinute[i] = ((c1 - '0') * 10) + (c2 - '0');

			c1 = tempDate[i].charAt(index + 23);
			c2 = tempDate[i].charAt(index + 24);
			smsSecond[i] = ((c1 - '0') * 10) + (c2 - '0');
		}

		while (1)
		{
			int16_t menuChoice = smsMenu("Messages", phoneNumber, tempDate, smsContent, numberOfTexts);
			Serial.println(menuChoice);
			if (menuChoice == -1)
				break;
			else if (menuChoice == 0)
				composeSMS();
			else
				viewSms(smsContent[menuChoice-1], phoneNumber[menuChoice-1], tempDate[menuChoice-1]);
		}
	}
}
uint16_t countSubstring(String string, String substring) {
	if (substring.length() == 0) return 0;
	int count = 0;
	for (size_t offset = string.indexOf(substring); offset != -1;
		offset = string.indexOf(substring, offset + substring.length()))
	{
		count++;
	}
	return count;
}
String readSms(uint8_t index) {
	String buffer;
	Serial1.print(F("AT+CMGF=1\r"));
	if ((readSerial().indexOf("ER")) == -1) {
		Serial1.print(F("AT+CMGR="));
		Serial1.print(index);
		Serial1.print("\r");
		buffer = readSerial();
		if (buffer.indexOf("CMGR:") != -1) {
			return buffer;
		}
		else return "";
	}
	else
		return "";
}
String readAllSms() {
	
	Serial1.print(F("AT+CMGL=\"ALL\"\r"));
	buffer = readSerial();
	delay(10);
	if (buffer.indexOf("CMGL:") != -1) {
		return buffer;
	}
	else if (buffer.indexOf("ERROR", buffer.indexOf("AT+CMGL")) != -1)
		return "ERROR";
	else if (buffer.indexOf("CMGL:") == -1 && buffer.indexOf("OK", buffer.indexOf("AT+CMGL")) != -1)
		return "OK";
	else
		return "";


}
void viewSms(String content, String contact, String date) {
	y = 14;  //Beggining point
	unsigned long buttonHeld = millis();
	unsigned long elapsedMillis = millis();
	bool blinkState = 1;
	while (1)
	{
		mp.display.fillScreen(TFT_DARKGREY);
		mp.display.setTextWrap(1);
		//while (kpd.pin_read(3))
		//{

		mp.display.setCursor(1, y);
		mp.display.print(content);
		if (mp.buttons.kpd.pin_read(BTN_DOWN) == 0) { //BUTTON DOWN
			Serial.println(mp.display.cursor_y);
			if (mp.display.cursor_y >= 128)
			{
				buttonHeld = millis();

				if (mp.buttons.kpd.pin_read(BTN_DOWN) == 1)
				{
					y -= 4;
					break;
				}

				while (mp.buttons.kpd.pin_read(BTN_DOWN) == 0)
				{
					if (millis() - buttonHeld > 100) {
						y -= 4;
						break;
					}
				}
			}
		}

		if (mp.buttons.kpd.pin_read(BTN_UP) == 0) { //BUTTON UP
			if (y < 14)
			{
				buttonHeld = millis();

				if (mp.buttons.kpd.pin_read(BTN_UP) == 1)
				{
					y += 4;
					break;
				}

				while (mp.buttons.kpd.pin_read(BTN_UP) == 0)
				{
					if (millis() - buttonHeld > 100) {
						y += 4;
						break;
					}
				}
			}

		}

		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			while (!mp.update());
			break;
		}
		//}
		// last draw the top entry thing

		if (millis() - elapsedMillis >= 1000) {
			elapsedMillis = millis();
			blinkState = !blinkState;
		}

		if (blinkState == 1)
		{
			mp.display.setTextColor(TFT_WHITE);
            mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
            mp.display.setTextFont(2);
            mp.display.setCursor(2,-1);
            mp.display.drawFastHLine(0, 14, mp.display.width(), TFT_WHITE);
			mp.display.print("From: ");
			mp.display.print(contact);
		}
		else
		{
			mp.display.setTextColor(TFT_WHITE);
            mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
            mp.display.setTextFont(2);
            mp.display.setCursor(2,-1);
            mp.display.drawFastHLine(0, 14, mp.display.width(), TFT_WHITE);
			mp.display.print(date);
		}

		mp.update();
	}
}
void smsMenuDrawBox(String contact, String date, String content, uint8_t i, int32_t y) {
	uint8_t scale;
	uint8_t offset;
	uint8_t boxHeight;
	uint8_t composeHeight;
	mp.display.setTextSize(1);
    scale = 2;
    offset = 19;
    composeHeight=21;
    boxHeight = 30;
    mp.display.setTextFont(2);
	y += (i-1) * (boxHeight-1) + composeHeight + offset;
	if (y < 0 || y > mp.display.height()) {
		return;
	}
		String monthsList[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
		mp.display.fillRect(1, y + 1, mp.display.width() - 2, boxHeight-2, TFT_DARKGREY);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.setCursor(4, y + 2);
		mp.display.drawString(contact, 3, y-1);

		//mp.display.drawString(date, 3, y + 10);
		mp.display.drawString(content, 3, y + 13);
		mp.display.setTextFont(1);
		mp.display.setCursor(124, y+3);
		mp.display.print(monthsList[smsMonth[i-1]-1]);
		mp.display.print(" ");
		mp.display.print(smsDay[i - 1]);
}
void smsMenuDrawCursor(uint8_t i, int32_t y) {
	uint8_t offset;
	uint8_t boxHeight;
	uint8_t composeHeight;
    offset = 19;
    composeHeight=21;
    boxHeight = 30;
    mp.display.setTextSize(2);
	if (millis() % 500 <= 250) {
		return;
	}
	y += (i-1) * (boxHeight-1) + composeHeight + offset;
	mp.display.drawRect(0, y, mp.display.width(), boxHeight, TFT_RED);
}
int16_t smsMenu(const char* title, String* contact, String *date, String *content, uint8_t length) {
	uint8_t cursor = 0;
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	uint8_t scale;
	uint8_t offset;
	uint8_t boxHeight;
    scale = 2;
    offset = 19;
    boxHeight = 30;
	while (1) {
		while (!mp.update());
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, 0);
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
		}

		for (uint8_t i = 0; i < length+1; i++) {
			if (i == 0)
				smsMenuComposeBox(i, cameraY_actual);
			else
				smsMenuDrawBox(contact[i-1], date[i-1], content[i-1], i, cameraY_actual);
		}
		if (cursor == 0)
			smsMenuComposeBoxCursor(cursor, cameraY_actual);
		else
			smsMenuDrawCursor(cursor, cameraY_actual);

        mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
        mp.display.setTextFont(2);
        mp.display.setCursor(1,-2);
        mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print(title);

		if (mp.buttons.released(BTN_A)) {   //BUTTON CONFIRM
			mp.gui.osc->note(75, 0.05);
			mp.gui.osc->play();
			while (!mp.update());// Exit when pressed
			break;
		}

		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
			mp.gui.osc->note(75, 0.05);
			mp.gui.osc->play();

			if (cursor == 0) {
				cursor = length - 1;
				if (length > 2) {
					cameraY = -(cursor - 1) * (boxHeight-1);
				}
			}
			else {
				cursor--;
				if (cursor > 0 && (cursor * (boxHeight-1) + cameraY + offset) < 14*scale) {
					cameraY += (boxHeight-1);
				}
			}
		}

		if (mp.buttons.released(BTN_DOWN)) { //BUTTON DOWN
			mp.gui.osc->note(75, 0.05);
			mp.gui.osc->play();

			cursor++;
			if (cursor > 0)
			{
				if (((cursor - 1) * (boxHeight-1) + composeBoxHeight + cameraY + offset) > 40*scale) {
					cameraY -= boxHeight-1;
				}
			}
			else
			{
				if ((cursor * (boxHeight-1) + cameraY + offset) > 40*scale) {
					cameraY -= boxHeight-1;
				}
			}
			if (cursor > length) {
				cursor = 0;
				cameraY = 0;

			}

		}

		if (mp.buttons.released(BTN_B) == 1) //BUTTON BACK
		{
			return -1;
		}
	}
	return cursor;

}
void smsMenuComposeBoxCursor(uint8_t i, int32_t y) {
	uint8_t offset;
	uint8_t boxHeight;
		offset = 19;
		boxHeight=21;
	if (millis() % 500 <= 250) {
		return;
	}
	y += offset;
	mp.display.drawRect(0, y, mp.display.width(), boxHeight+1, TFT_RED);
}
void smsMenuComposeBox(uint8_t i, int32_t y) {
	uint8_t scale;
	uint8_t offset;
	uint8_t boxHeight;
	mp.display.setTextSize(1);
    scale = 2;
    offset = 19;
    boxHeight=21;
    mp.display.setTextFont(2);
	y += offset;
	if (y < 0 || y > mp.display.height()) {
		return;
	}
	mp.display.fillRect(1, y + 1, mp.display.width() - 2, boxHeight-1, TFT_DARKGREY);
	mp.display.drawBitmap(2*scale, y + 2, composeIcon, TFT_WHITE, scale);
	mp.display.setTextColor(TFT_WHITE);
	mp.display.drawString("New SMS", 20*scale, y+3);
	mp.display.setFreeFont(TT1);

}
void composeSMS()
{
	textPointer = 0;
    y = 16; //beginning point
	String content = "";
	String contact = "";
	String prevContent = "";
	char key = NO_KEY;
	bool cursor = 0; //editing contacts or text content
	unsigned long elapsedMillis = millis();
	bool blinkState = 1;
	uint8_t scale;

	while (1)
	{


		mp.display.fillScreen(TFT_DARKGREY);

		mp.display.setTextColor(TFT_WHITE);
        scale = 2;
        mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
        mp.display.setTextFont(2);
        mp.display.setCursor(2,-1);
        mp.display.drawFastHLine(0, 14, mp.display.width(), TFT_WHITE);
		mp.display.print("To: ");

		if (millis() - elapsedMillis >= multi_tap_threshold) //cursor blinking routine
		{
			elapsedMillis = millis();
			blinkState = !blinkState;
		}
		if (cursor == 0) //inputting the contact number
		{
			key = mp.buttons.kpdNum.getKey();
			if (key == 'A') //clear number
				contact = "";
			else if (key == 'C')
				contact.remove(contact.length() - 1);
			if (key != NO_KEY && isdigit(key))
				contact += key;
			mp.display.setTextWrap(1);
			mp.display.setCursor(1*scale, y);
			if (content == "")
			{
				mp.display.setTextColor(TFT_LIGHTGREY);
				mp.display.print(F("Compose..."));
				mp.display.setTextColor(TFT_WHITE);
			}
			else
				mp.display.print(content);
            mp.display.setCursor(27, -1);

			mp.display.print(contact);
			if (blinkState == 1)
                mp.display.drawFastVLine(mp.display.getCursorX(), mp.display.getCursorY()+3, 10, TFT_WHITE);
		}
		else //inputting the text content
		{
			mp.display.setTextColor(TFT_WHITE);
            mp.display.setCursor(2, -1);
			mp.display.print("To: ");
			mp.display.print(contact);
			prevContent = content;
			content = mp.textInput(content, -1);
			if (prevContent != content)
			{
				blinkState = 1;
				elapsedMillis = millis();
			}
			mp.display.setTextWrap(1);
			mp.display.setCursor(1*scale, y);
			mp.display.print(content);
			if(blinkState == 1)
                mp.display.drawFastVLine(mp.display.getCursorX(), mp.display.getCursorY()+3, 10, TFT_WHITE);

			/*if (blinkState == 1)
			{
				mp.display.fillRect(0, 0, mp.display.width(), 7, TFT_DARKGREY);
				mp.display.setTextColor(TFT_WHITE);
				mp.display.setCursor(1, 6);
				mp.display.print("From: ");
				mp.display.print(contact);
				mp.display.drawFastHLine(0, 7, LCDWIDTH, TFT_BLACK);
			}
			else
			{
				mp.display.setTextColor(TFT_WHITE);
				mp.display.setCursor(1, 6);
				mp.display.drawFastHLine(0, 7, LCDWIDTH, TFT_BLACK);
			}*/
		}
		if (mp.buttons.kpd.pin_read(BTN_UP) == 0 && cursor == 1) { //BUTTON UP
			/*Serial.println(mp.display.cursor_y);
			if (mp.display.cursor_y >= 64)
			{
				buttonHeld = millis();

				if (mp.buttons.kpd.pin_read(BTN_UP) == 1)
				{
					y -= 2;
					break;
				}

				while (mp.buttons.kpd.pin_read(BTN_UP) == 0)
				{
					if (millis() - buttonHeld > 100) {
						y -= 2;
						break;
					}
				}
			}*/
			cursor = 0;
		}

		if (mp.buttons.kpd.pin_read(BTN_DOWN) == 0 && cursor == 0) { //BUTTON DOWN
			/*if (y < 14)
			{


				buttonHeld = millis();

				if (mp.buttons.kpd.pin_read(BTN_DOWN) == 1)
				{
					y += 2;
					break;
				}

				while (mp.buttons.kpd.pin_read(BTN_DOWN) == 0)
				{
					if (millis() - buttonHeld > 100) {
						y += 2;
						break;
					}
				}
			}*/
			cursor = 1;

		}

		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			while (!mp.update());
			break;
		}
		if (mp.buttons.released(BTN_A) && contact != "" && content != "") // SEND SMS
		{
			mp.display.fillScreen(TFT_BLACK);
            mp.display.setCursor(0, mp.display.height()/2 - 16);
            mp.display.setTextFont(2);
			mp.display.printCenter("Sending text...");
			while (!mp.update());

			Serial1.print("AT+CMGS=\"");
			Serial1.print(contact);
			Serial1.println("\"");
			while (!Serial1.available());
			Serial.println(Serial1.readString());
			delay(5);
			Serial1.print(content);
			while (!Serial1.available());
			Serial.println(Serial1.readString());
			delay(5);
			Serial1.println((char)26);
			while (Serial1.readString().indexOf("OK") != -1);
			mp.display.fillScreen(TFT_BLACK);
			mp.display.printCenter("Text sent!");
			while (!mp.update());
			delay(1000);
			break;
		}




		mp.update();
	}
}