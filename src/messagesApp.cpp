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

/*
[
	{
		number: "123123",
		dateTime: "2018-12-12 12:12:12",
		text: "asd asd asd asd"
	}
]
*/

//Messages app
void messagesApp() {
	mp.dataRefreshFlag = 0;
	SDAudioFile file = mp.SD.open("/messages.json", "r");

	if(file.size() < 2){ // empty -> FILL
		Serial.println("Override");
		file.close();
		// JsonArray& jarr = jb.parseArray("[{\"number\":\"123123\", \"dateTime\":\"2018-12-12 12:12:12\", \"text\":\"asd asd asd asd\"}]");
		// JsonArray& jarr = jb.parseArray("[{\"number\":\"123123\", \"dateTime\":\"2018-12-12 12:12:12\", \"text\":\"asd asd asd asd\"}, {\"number\":\"09123\", \"dateTime\":\"2018-12-12 12:12:12\", \"text\":\"Some other text\"}, {\"number\":\"911\", \"dateTime\":\"2018-03-12 12:12:12\", \"text\":\"Help\"}]");
		JsonArray& jarr = jb.createArray();
		delay(10);
		SDAudioFile file1 = mp.SD.open("/messages.json", "w");
		jarr.prettyPrintTo(file1);
		file1.close();
		file = mp.SD.open("/messages.json", "r");
		while(!file)
			Serial.println("Messages ERROR");
	}

	JsonArray& jarr = jb.parseArray(file);

	if(!jarr.success())
	{
		Serial.println("Error");
		mp.display.fillScreen(TFT_BLACK);
        mp.display.setCursor(0, mp.display.height()/2 - 16);
        mp.display.setTextFont(2);
		mp.display.printCenter("Error: Messages - loading data");
		while (mp.buttons.released(BTN_B) == 0)//BUTTON BACK
		mp.update();
	}
	else
	{
		while (1)
		{
			int16_t menuChoice = smsMenu(&jarr);
			Serial.println(menuChoice);
			if (menuChoice == -1)
				break;
			else if (menuChoice == 0)
				composeSMS(&jarr);
			else
				viewSms(jarr[menuChoice-1]["text"].as<char*>(), jarr[menuChoice-1]["number"].as<char*>(), jarr[menuChoice-1]["dateTime"].as<char*>());
		}
	}
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

		mp.display.setCursor(1, y);
		mp.display.print(content);
		if (mp.buttons.pressed(BTN_DOWN)) { //BUTTON DOWN
			Serial.println(mp.display.cursor_y);
			if (mp.display.cursor_y >= 128)
			{
				buttonHeld = millis();

				if (!mp.buttons.released(BTN_DOWN))
				{
					y -= 4;
					break;
				}
				while (!mp.buttons.released(BTN_DOWN))
				{
					if (millis() - buttonHeld > 100) {
						y -= 4;
						break;
					}
					mp.update();
				}
			}
			mp.update();
		}

		if (mp.buttons.pressed(BTN_UP)) { //BUTTON UP
			if (y < 14)
			{
				buttonHeld = millis();

				if (!mp.buttons.released(BTN_UP))
				{
					y += 4;
					break;
				}
				while (!mp.buttons.released(BTN_UP))
				{
					if (millis() - buttonHeld > 100) {
						y += 4;
						break;
					}
					mp.update();
				}
			}
			mp.update();
		}

		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			mp.update();
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
void smsMenuDrawBox(String contact, String date, String content, uint8_t sms_day, uint8_t sms_month, uint8_t i, int32_t y) {
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
		mp.display.print(monthsList[sms_month-1]);
		mp.display.print(" ");
		mp.display.print(sms_day);
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
int16_t smsMenu(JsonArray *messages) {
	uint8_t cursor = 0;
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	uint8_t scale;
	uint8_t offset;
	uint8_t boxHeight;
	int length = messages->size();
    scale = 2;
    offset = 19;
    boxHeight = 30;
	while (1) {
		mp.update();
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, 0);
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
		}

		uint8_t i = 0;

		smsMenuComposeBox(i, cameraY_actual);
		for (JsonObject& elem : *messages) {
			String date = elem["dateTime"];
			int day = date.substring(8, 10).toInt();
			int month = date.substring(5, 7).toInt();
			smsMenuDrawBox(elem["number"].as<char*>(), elem["dateTime"].as<char*>(), elem["text"].as<char*>(), day, month, i+1, cameraY_actual);
			i++;
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
		mp.display.print("Messages");

		if (mp.buttons.released(BTN_A)) {   //BUTTON CONFIRM
			osc->note(75, 0.05);
			osc->play();
			mp.update();// Exit when pressed
			break;
		}

		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
			osc->note(75, 0.05);
			osc->play();

			if (cursor == 0) {
				cursor = length;
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
			osc->note(75, 0.05);
			osc->play();

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
void composeSMS(JsonArray *messages)
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
			key = mp.buttons.getKey();
			if (mp.buttons.released(BTN_FUN_RIGHT)) //clear number
				contact = "";
			else if (mp.buttons.released(BTN_FUN_LEFT))
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

		}
		if (mp.buttons.released(BTN_UP) && cursor == 1) { //BUTTON UP
			cursor = 0;
		}

		if (mp.buttons.released(BTN_DOWN) && cursor == 0) { //BUTTON DOWN
			cursor = 1;
		}

		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			mp.update();
			break;
		}
		if (mp.buttons.released(BTN_A) && contact != "" && content != "") // SEND SMS
		{
			mp.display.fillScreen(TFT_BLACK);
            mp.display.setCursor(0, mp.display.height()/2 - 16);
            mp.display.setTextFont(2);
			mp.display.printCenter("Sending text...");
			mp.update();

			saveMessage(content, contact, messages);

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
			mp.update();
			delay(1000);
			break;
		}




		mp.update();
	}
}

void saveMessage(String text, String number, JsonArray *messages){
	JsonObject& new_item = jb.createObject();

	new_item["number"] = number;
	new_item["text"] = text;
	new_item["dateTime"] = mp.currentDateTime();

	messages->add(new_item);
	SDAudioFile file1 = mp.SD.open("/messages.json", "w");
	messages->prettyPrintTo(file1);
	file1.close();
}
