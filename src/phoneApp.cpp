#include "phoneApp.h"
void phoneApp() {
	mp.dataRefreshFlag = 0;
	String callBuffer = "";
	char key = NO_KEY;
	mp.display.setTextWrap(0);
	mp.display.setTextFont(2);

	while (1)
	{ // za 20 prema gore
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setTextSize(1);
		mp.display.fillRect(0, 59, mp.display.width(), 26, TFT_DARKGREY);
		mp.display.setCursor(2, 100);
		mp.display.print("Press D to open call-log");
		mp.display.setCursor(2, 112);
		mp.display.print("Press A to call");
		mp.display.setCursor(2, -1);
		mp.display.setTextColor(TFT_LIGHTGREY);
		mp.display.print("Dialer");
		mp.display.setTextColor(TFT_WHITE);


		key = mp.buttons.kpdNum.getKey();
		if (key == 'A') //clear number
			callBuffer = "";
		else if (key == 'C')
			callBuffer.remove(callBuffer.length()-1);
		else if (key == 'D')
			callLog();
		else if (key != NO_KEY && key!= 'A' && key != 'C' && key != 'B' && key != 'D')
		{
			switch (key)
			{
				case '1':
					osc->note(C5,0.05);
					osc->play();
					break;
				case '2':
					osc->note(D5,0.05);
					osc->play();
					break;
				case '3':
					osc->note(E5,0.05);
					osc->play();
					break;
				case '4':
					osc->note(F5,0.05);
					osc->play();
					break;
				case '5':
					osc->note(G5,0.05);
					osc->play();
					break;
				case '6':
					osc->note(A5,0.05);
					osc->play();
					break;
				case '7':
					osc->note(B5,0.05);
					osc->play();
					break;
				case '8':
					osc->note(C6,0.05);
					osc->play();
					break;
				case '9':
					osc->note(D6,0.05);
					osc->play();
					break;
				case '*':
					osc->note(E6,0.05);
					osc->play();
					break;
				case '0':
					osc->note(F6,0.05);
					osc->play();
					break;
				case '#':
					osc->note(G6,0.05);
					osc->play();
					break;

				default:
					break;
			}
			callBuffer += key;
		}
        mp.display.setCursor(0, 56);
		mp.display.setTextSize(2);
		mp.display.print(callBuffer);

		if (mp.display.cursor_x + 4  >= mp.display.width())
		{
			mp.display.fillRect(0, 59, mp.display.width(), 26, TFT_DARKGREY);
			mp.display.setCursor(mp.display.width() - mp.display.cursor_x - 14, 56);
			mp.display.print(callBuffer);
		}


		if (mp.buttons.pressed(BTN_A))//initate call
		{
			callNumber(callBuffer);
			while (!mp.update());
			callBuffer = "";
		}
		if (mp.buttons.released(BTN_B)) //BACK BUTTON
			break;
		if (mp.buttons.released(BTN_B)) //BACK BUTTON
			break;

		mp.update();
	}
}

void callLog() {
	mp.dataRefreshFlag = 0;
	mp.display.setTextWrap(0);
    mp.display.setTextFont(2);

	uint16_t curr_id = 0;

	SDAudioFile file = mp.SD.open("/call_log.json", "r");

	if(file.size() < 2){ // empty -> FILL
		Serial.println("Override");
		file.close();
		JsonArray& jarr = jb.parseArray("[{\"dateTime\":\"2019-04-18 12:00:00\", \"number\":\"+385992010102\", \"duration\":\"124\"}]");
		delay(10);
		SDAudioFile file1 = mp.SD.open("/call_log.json", "w");
		jarr.prettyPrintTo(file1);
		file1.close();
		file = mp.SD.open("/call_log.json", "r");
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
		mp.display.printCenter("Error: Call log - loading data");
		while (mp.buttons.released(BTN_B) == 0)//BUTTON BACK
		while (!mp.update());
	}
	else
	{
		// Maybe later read from core.json
		// uint8_t contactNumber = countSubstring(input, "CPBR:");
		// Serial.println(contactNumber);

		// index(&jarr);
		// add("5555", "2019-04-18 13:00:00", "342", &jarr);
		// index(&jarr);
		// remove(1, &jarr);
		// index(&jarr);

		while (1)
		{
			int menuChoice = -1;
			menuChoice = callLogMenu(&jarr);

			mp.update();
			if (menuChoice != -2)
			{
				if(menuChoice >= 0){
					Serial.print("OPEN ");
					Serial.println(menuChoice);
				} else {
					Serial.println("reaload");
				}
				while(!mp.update());
			} else {
				break;
			}
		}
	}
}

int callLogMenu(JsonArray *call_log){
	uint8_t cursor = 0;
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	uint8_t length = call_log->size();
    uint8_t offset = 19;
    uint8_t boxHeight = 28;

	while (1) {
		while (!mp.update());
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, 0);
		if(call_log->size() == 0){
			mp.display.printCenter("No calls");
		} else {
			cameraY_actual = (cameraY_actual + cameraY) / 2;
			if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
			}

			int i = 0;
			for (JsonObject& elem : *call_log) {
				callLogDrawBoxSD(elem["number"].as<char*>(), elem["dateTime"].as<char*>(), i, cameraY_actual);
				i++;
			}
			callLogMenuDrawCursor(cursor, cameraY_actual);

			// last draw the top entry thing
			mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
			mp.display.setTextFont(2);
			mp.display.setCursor(0,-2);
			mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
			mp.display.setTextSize(1);
			mp.display.setTextColor(TFT_WHITE);
			mp.display.print("Call log");

			if (mp.buttons.kpd.pin_read(BTN_A) == 0) {   //BUTTON CONFIRM
				while (mp.buttons.kpd.pin_read(BTN_A) == 0); // Exit when pressed
				break;
			}
			if (mp.buttons.kpd.pin_read(BTN_LEFT) == 0) {
				while (mp.buttons.kpd.pin_read(BTN_LEFT) == 0); // Delete
				call_log->remove(cursor);
				// TODO: SAVE
				return -10;
			}

			if (mp.buttons.kpd.pin_read(BTN_UP) == 0) {  //BUTTON UP
				while (mp.buttons.kpd.pin_read(BTN_UP) == 0);
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
			if (mp.buttons.kpd.pin_read(BTN_DOWN) == 0) { //BUTTON DOWN
				while (mp.buttons.kpd.pin_read(BTN_DOWN) == 0);

				cursor++;
				if ((cursor * (boxHeight+1) + cameraY + offset) > 48) {
					cameraY -= (boxHeight+1);
				}
				if (cursor >= length + 1) {
					cursor = 0;
					cameraY = 0;

				}
			}
		}

		if (mp.buttons.released(BTN_B) == 1) //BUTTON BACK
		{
			while (!mp.update());
			return -2;
		}
	}
	return cursor;
}

void callLogDrawBoxSD(String number, String dateTime, uint8_t i, int32_t y) {
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
    mp.display.drawString(dateTime, 4, y);
    mp.display.drawString(number, 4, y + 12);
}

void callLogMenuDrawCursor(uint8_t i, int32_t y) {
    uint8_t offset = 19;
    uint8_t boxHeight = 28;
	if (millis() % 500 <= 250) {
		return;
	}
	y += i * boxHeight + offset;
	mp.display.drawRect(0, y, mp.display.width(), boxHeight + 1, TFT_RED);
}
