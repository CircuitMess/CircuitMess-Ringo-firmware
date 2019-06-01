#include "phoneApp.h"
void phoneApp() {
	mp.dataRefreshFlag = 0;
	String callBuffer = "";
	char key = NO_KEY;
	mp.display.setTextWrap(0);
	mp.display.setTextFont(2);

	while (1)
	{
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.setTextSize(1);
		mp.display.fillRect(0, 79, mp.display.width(), 26, TFT_DARKGREY);
		mp.display.drawRect(108, 110, 52, 17, TFT_WHITE);
		mp.display.setCursor(110, 110);
		mp.display.print("Call log");
		mp.display.setCursor(2, 110);
		mp.display.print("Press A to call");
		mp.display.setCursor(2, -1);
		mp.display.setTextColor(TFT_LIGHTGREY);
		mp.display.print("Dialer");
		mp.display.setTextColor(TFT_WHITE);

		key = NO_KEY;
		for(int i = 0; i < 11; i++) {
			if(mp.buttons.released(i) && i != 9) {
				Serial.println(i);
				if(i == 10) {
					key = '0';
				} else {
					key = '0' + i + 1;
				}
				break;
			}
		}

		if (mp.buttons.released(BTN_FUN_LEFT))
			callBuffer.remove(callBuffer.length()-1);
		else if (mp.buttons.released(BTN_FUN_RIGHT))
			callLog();
		else if (key != NO_KEY && isdigit(key))
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
		mp.display.setCursor(0, 76);
		mp.display.setTextSize(2);
		mp.display.print(callBuffer);

		if (mp.display.cursor_x + 4  >= mp.display.width())
		{
			mp.display.fillRect(0, 79, mp.display.width(), 26, TFT_DARKGREY);
			mp.display.setCursor(mp.display.width() - mp.display.cursor_x - 14, 76);
			mp.display.print(callBuffer);
		}

		if (mp.buttons.released(BTN_A))//initate call
		{
			callNumber(callBuffer);
			while (!mp.update());
			callBuffer = "";
		}
		if (mp.buttons.released(BTN_B)) //BACK BUTTON
			break;

		while (!mp.update());
	}
	while(!mp.update());
}

void addCall(String number, String dateTime, int duration){
	SDAudioFile file = mp.SD.open("/call_log.json", "r");

	if(file.size() < 2){
		Serial.println("Override");
		file.close();
		JsonArray& jarr = jb.parseArray("[]");
		delay(10);
		SDAudioFile file1 = mp.SD.open("/call_log.json", "w");
		jarr.prettyPrintTo(file1);
		file1.close();
		file = mp.SD.open("/call_log.json", "r");
		while(!file)
			Serial.println("CONTACTS ERROR");
	}

	JsonArray& jarr = jb.parseArray(file);
	file.close();

	JsonObject& new_item = jb.createObject();
	new_item["number"] = number;
	new_item["dateTime"] = dateTime;
	new_item["duration"] = duration;
	jarr.add(new_item);

	SDAudioFile file1 = mp.SD.open("/call_log.json", "w");
	jarr.prettyPrintTo(file1);
	file1.close();
}

void callLog() {
	mp.dataRefreshFlag = 0;
	mp.display.setTextWrap(0);
    mp.display.setTextFont(2);

	SDAudioFile file = mp.SD.open("/call_log.json", "r");

	if(file.size() < 2){ // empty -> FILL
		Serial.println("Override");
		file.close();
		JsonArray& jarr = jb.parseArray("[{\"dateTime\":\"2019-04-18 12:00:00\", \"number\":\"911\", \"duration\":\"124\"}]");
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
		while (!mp.buttons.released(BTN_B))//BUTTON BACK
		while (!mp.update());
	}
	else
	{
		// add("5555", "2019-04-18 13:00:00", "342", &jarr);

		while (1)
		{
			int menuChoice = -1;
			menuChoice = callLogMenu(&jarr);

			mp.update();
			if (menuChoice != -2)
			{
				if(menuChoice >= 0)
				{
					if(showCall(menuChoice, jarr[menuChoice]["number"], jarr[menuChoice]["dateTime"], jarr[menuChoice]["duration"])){
						jarr.remove(menuChoice);
						SDAudioFile file = mp.SD.open("/call_log.json", "w");
						jarr.prettyPrintTo(file);
						file.close();
					}
				}
				while(!mp.update());
			}
			else
			{
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
		mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
		mp.display.setTextFont(2);
		mp.display.setCursor(0,-2);
		mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Call log");

		if(call_log->size() == 0){
			mp.display.setTextSize(2);
			mp.display.setCursor(0, 16);
			mp.display.setTextColor(TFT_WHITE);
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

			if (mp.buttons.released(BTN_A)) {
				while (!mp.update());
				break;
			}
			if (mp.buttons.released(BTN_FUN_LEFT)) //BUTTON BACK
			{
				while (!mp.update());
				call_log->remove(cursor);
				SDAudioFile file = mp.SD.open("/call_log.json", "w");
				call_log->prettyPrintTo(file);
				file.close();
				return -10;
			}

			if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
				while (!mp.update());
				if (cursor == 0) {
					cursor = length - 1;
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
				while (!mp.update());
				cursor++;
				if ((cursor * (boxHeight+1) + cameraY + offset) > 48) {
					cameraY -= (boxHeight+1);
				}
				if (cursor >= length) {
					cursor = 0;
					cameraY = 0;
				}
			}
		}

		if (mp.buttons.released(BTN_B)) //BUTTON BACK
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

uint8_t showCall(int id, String number, String dateTime, String duration)
{
	while (1)
	{
		mp.display.fillScreen(TFT_BLACK);
        mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
        mp.display.setTextFont(2);
        mp.display.setCursor(2,-2);
        mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Call log");

		mp.display.setCursor(0, 18);
		mp.display.print(" ");
		mp.display.println(dateTime);
		mp.display.print(" ");
		mp.display.println(number);

		int seconds = duration.toInt() % 60;
		int minutes = duration.toInt() / 60;

		mp.display.print(" ");
		if(minutes < 10)
			mp.display.print("0");
		mp.display.print(minutes);
		mp.display.print(":");
		if(seconds < 10)
			mp.display.print("0");
		mp.display.print(seconds);

		mp.display.fillRect(105, 102, 30*2, 9*2, TFT_GREENYELLOW);
		mp.display.fillRect(5, 102, 30*2, 9*2, TFT_RED);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.setCursor(10, 103);
		mp.display.print("C Delete");
		mp.display.setCursor(115, 103);
		mp.display.print("A Call");

		if (mp.buttons.released(BTN_FUN_LEFT)) //BUTTON BACK
		{
			while (!mp.update());
			return 1;
		}
		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			Serial.println("Go back");
			while (!mp.update());
			break;
		}
		if (mp.buttons.released(BTN_A)) // Call
		{
			callNumber(number);
			while(!mp.update());
			return 0;
		}

		mp.update();
	}
	return 0;
}
