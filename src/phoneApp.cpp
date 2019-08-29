#include "phoneApp.h"
void phoneApp() {
	mp.dataRefreshFlag = 0;
	String callBuffer = "";
	char key = NO_KEY;
	mp.display.setTextWrap(0);
	mp.display.setTextFont(2);
	bool plusSign = 0;
	uint32_t blinkMillis = millis();
	bool blinkState = 1;
	while (1)
	{
		if(millis() - blinkMillis >= 350)
		{
			blinkMillis = millis();
			blinkState = !blinkState;
		}
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.setTextSize(1);
		mp.display.fillRect(0, 79, mp.display.width(), 26, TFT_DARKGREY);
		mp.display.drawRect(108, 110, 52, 17, TFT_WHITE);
		mp.display.setCursor(110, 110);
		mp.display.setTextFont(2);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Call log");
		mp.display.setCursor(60, -1);
		mp.display.print("Press A to call");
		mp.display.setCursor(4, 110);
		mp.display.print("Erase");
		mp.display.setCursor(2, -1);
		mp.display.setTextColor(TFT_LIGHTGREY);
		mp.display.print("Dialer");
		mp.display.setTextColor(TFT_WHITE);

		key = NO_KEY;
		for(int i = 0; i < 12; i++) {
			if(mp.buttons.released(i)) {
				if(i == 10)
					key = '0';
				else if(i == 9)
					key = '*';
				else if(i == 11)
					key = '#';
				else
					key = '0' + i + 1;
				break;
			}
		}
		if(mp.buttons.released(BTN_HOME)) {
			mp.exitedLockscreen = true;
			mp.lockscreen(); // Robert
		}
		if(mp.buttons.held(BTN_0, 20))
		{
			callBuffer+="+";
			plusSign = 1;
		}
		if(key == '0' && plusSign)
			key = NO_KEY;
		if(plusSign && mp.buttons.released(BTN_0))
			plusSign = 0;
		if (mp.buttons.released(BTN_FUN_LEFT))
		{
			callBuffer.remove(callBuffer.length()-1);
			// while(!mp.update());
			mp.buttons.update();
		}
		if (mp.buttons.released(BTN_FUN_RIGHT))
		{
			while(!mp.update());
			if(!mp.SDinsertedFlag)
			{
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setCursor(0, mp.display.height()/2 - 20);
				mp.display.setTextFont(2);
				mp.display.printCenter(F("No SD card inserted!"));
				mp.display.setCursor(0, mp.display.height()/2);
				mp.display.printCenter(F("Can't open call log"));
				uint32_t tempMillis = millis();
				while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
					mp.update();
				while(!mp.update());
			}
			else
				callLog();
		}
		if (key != NO_KEY)
		{
			switch (key)
			{
				case '1':
					mp.osc->note(C5,0.05);
					mp.osc->play();
					break;
				case '2':
					mp.osc->note(D5,0.05);
					mp.osc->play();
					break;
				case '3':
					mp.osc->note(E5,0.05);
					mp.osc->play();
					break;
				case '4':
					mp.osc->note(F5,0.05);
					mp.osc->play();
					break;
				case '5':
					mp.osc->note(G5,0.05);
					mp.osc->play();
					break;
				case '6':
					mp.osc->note(A5,0.05);
					mp.osc->play();
					break;
				case '7':
					mp.osc->note(B5,0.05);
					mp.osc->play();
					break;
				case '8':
					mp.osc->note(C6,0.05);
					mp.osc->play();
					break;
				case '9':
					mp.osc->note(D6,0.05);
					mp.osc->play();
					break;
				case '*':
					mp.osc->note(E6,0.05);
					mp.osc->play();
					break;
				case '0':
					mp.osc->note(F6,0.05);
					mp.osc->play();
					break;
				case '#':
					mp.osc->note(G6,0.05);
					mp.osc->play();
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
		if (blinkState)
		{
			mp.display.drawFastVLine(mp.display.getCursorX() + 1, mp.display.getCursorY() + 6, 20, TFT_WHITE);
			mp.display.drawFastVLine(mp.display.getCursorX() + 2, mp.display.getCursorY() + 6, 20, TFT_WHITE);
		}

		if (mp.buttons.released(BTN_A) && callBuffer != "")//initate call
		{
			while(!mp.update());
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
			else if(!mp.simInserted && callBuffer != "112")
			{
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setTextColor(TFT_WHITE);
				mp.display.setTextSize(1);
				mp.display.setCursor(0, mp.display.height()/2 - 20);
				mp.display.setTextFont(2);
				mp.display.printCenter(F("No SIM inserted!"));
				mp.display.setCursor(0, mp.display.height()/2);
				mp.display.printCenter(F("Insert SIM and reset"));
				uint32_t tempMillis = millis();
				while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
					mp.update();
				while(!mp.update());
			}
			else if(mp.airplaneMode)
			{
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setTextColor(TFT_WHITE);
				mp.display.setTextSize(1);
				mp.display.setCursor(0, mp.display.height()/2 - 20);
				mp.display.setTextFont(2);
				mp.display.printCenter(F("Can't make calls"));
				mp.display.setCursor(0, mp.display.height()/2);
				mp.display.printCenter(F("Turn off airplane mode"));
				uint32_t tempMillis = millis();
				while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
					mp.update();
				while(!mp.update());
			}
			else
			{
				bool readyForCall = 0;
				uint32_t timeoutMillis = millis(); 
				while(Serial1.available())
					Serial1.read();
				while(!readyForCall && millis() - timeoutMillis < 10000)
				{
					Serial1.println("AT+CCALR?");
					String input = mp.waitForOK();

					uint16_t helper = input.indexOf(" ", input.indexOf("+CCALR:"));
					readyForCall = input.substring(helper + 1, helper + 2).toInt();
					Serial.println(input);
					if(!readyForCall)
					{
						mp.display.fillScreen(TFT_BLACK);
						mp.display.setTextColor(TFT_WHITE);
						mp.display.setTextSize(1);
						mp.display.setCursor(0, mp.display.height()/2 - 20);
						mp.display.setTextFont(2);
						mp.display.printCenter(F("Registering to network"));
						mp.display.setCursor(0, mp.display.height()/2);
						mp.display.printCenter(F("Please wait..."));
						while(!mp.update());
						delay(1000);
					}
				}
				mp.networkRegistered = readyForCall;
				if(readyForCall)
				{
					if(mp.signalStrength == 99)
					{
						Serial1.println("AT+CSQ");
						String buffer = "";
						uint32_t current = millis();
						while(buffer.indexOf("+CSQ:") == -1 && millis() - current >= 2000)
							buffer = Serial1.readString();
						if(buffer.indexOf("+CSQ:") != -1)
							mp.signalStrength = buffer.substring(buffer.indexOf(" ", buffer.indexOf("+CSQ:")) + 1, buffer.indexOf(",", buffer.indexOf(" ", buffer.indexOf("+CSQ:")))).toInt();
						if(mp.signalStrength == 99)
						{
							mp.display.fillScreen(TFT_BLACK);
							mp.display.setTextColor(TFT_WHITE);
							mp.display.setTextSize(1);
							mp.display.setCursor(0, mp.display.height()/2 - 20);
							mp.display.setTextFont(2);
							mp.display.printCenter(F("No signal!"));
							mp.display.setCursor(0, mp.display.height()/2);
							mp.display.printCenter(F("Check your antenna"));
							uint32_t tempMillis = millis();
							while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
								mp.update();
							while(!mp.update());
						}
						else
						{
							if(callBuffer.startsWith("*"))
							{
								sendMMI(callBuffer);
							}
							else
							{
								callNumber(callBuffer);
								while(!mp.update());
							}
							callBuffer = "";
						}
					}
					else
					{
						
						if(callBuffer.startsWith("*"))
						{
							sendMMI(callBuffer);
						}
						else
						{
							callNumber(callBuffer);
							while(!mp.update());
						}
						callBuffer = "";
					}
				}
				else
				{
					mp.display.fillScreen(TFT_BLACK);
					mp.display.setTextColor(TFT_WHITE);
					mp.display.setTextSize(1);
					mp.display.setCursor(0, mp.display.height()/2 - 20);
					mp.display.setTextFont(2);
					mp.display.printCenter(F("Network unavailable"));
					mp.display.setCursor(0, mp.display.height()/2);
					mp.display.printCenter(F("Try again later"));
					uint32_t tempMillis = millis();
					while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
						mp.update();
					while(!mp.update());
				}
			}
		}
		if (mp.buttons.released(BTN_B)) //BACK BUTTON
			break;

		mp.update();
	}
	while(!mp.update());
}

void callLog() {
	mp.dataRefreshFlag = 0;
	mp.display.setTextWrap(0);
    mp.display.setTextFont(2);
	for(int i = 0; i < sizeof(mp.notificationTypeList); i++)
	{
		if(mp.notificationTypeList[i] == 1)
		{
			mp.notificationTypeList[i] = 0;
			mp.notificationDescriptionList[i] = "";
			mp.notificationTimeList[i] = DateTime((uint32_t)0);
		}
	}
	mp.saveNotifications();
	File file = SD.open("/.core/call_log.json", "r");
	if(file.size() < 2){ // empty -> FILL
		Serial.println("Override");
		file.close();
		JsonArray& jarr = jb.createArray();
		delay(10);
		File file1 = SD.open("/.core/call_log.json", "w");
		jarr.prettyPrintTo(file1);
		file1.close();
		file = SD.open("/.core/call_log.json", "r");
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
		mp.display.printCenter("Error: Call log - loading data");
		while (!mp.buttons.released(BTN_B))//BUTTON BACK
			mp.update();
		while(!mp.update());
	}
	else
	{
		// add("5555", "2019-04-18 13:00:00", "342", &jarr);
		int menuChoice = -1;

		while (1)
		{
			menuChoice = callLogMenu(jarr, menuChoice);

			mp.buttons.update();
			if (menuChoice != -2)
			{
				if(menuChoice >= 0)
				{
					if(showCall(menuChoice, jarr[menuChoice]["number"], jarr[menuChoice]["dateTime"],
					jarr[menuChoice]["contact"], jarr[menuChoice]["duration"], jarr[menuChoice]["direction"].as<uint8_t>())){
						jarr.remove(menuChoice);
						File file = SD.open("/.core/call_log.json", "w");
						jarr.prettyPrintTo(file);
						file.close();
						menuChoice = -1;
					}
				}
				else if(menuChoice == -10)
				{
					File file = SD.open("/.core/call_log.json", "r");
					jb.clear();
					JsonArray& jarr = jb.parseArray(file);
					file.close();
				}
				while(!mp.update());
			}
			else
				break;
		}
	}
}

int callLogMenu(JsonArray& call_log, int prevCursor){
	uint8_t cursor = 0;
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	uint8_t length = call_log.size();
    uint8_t offset = 20;
    uint8_t boxHeight = 28;
	uint16_t sortingArray[length];
	bool blinkState = 0;
	uint32_t blinkMillis = millis();
	for(int i = 0; i < length; i++)
		sortingArray[i] = i;
	uint16_t min;
	for(int i = 0; i < length - 1; i++)
	{
		min = i;
		for(int j = i + 1; j < length ; j++)
			if(call_log[sortingArray[j]]["dateTime"].as<uint32_t>() > call_log[sortingArray[min]]["dateTime"].as<uint32_t>())
				min = j;
		uint16_t temp = sortingArray[min];
		sortingArray[min] = sortingArray[i];
		sortingArray[i] = temp;
	}
	for(int i = 0; i < length; i++)
	{
		if(sortingArray[i] == prevCursor)
			cursor = i;
	}
	if(prevCursor == -1)
		cursor = 0;
	if (length > 2 && cursor > 2) {
		cameraY = -(cursor - 2) * boxHeight;
	}
	while (1) {
		mp.display.fillScreen(TFT_BLACK);
		if(length == 0)
		{
			mp.display.setTextSize(2);
			mp.display.setCursor(0, 40);
			mp.display.setTextColor(TFT_WHITE);
			mp.display.printCenter("No calls");
			cursor = 0;
		}
		else
		{
			cameraY_actual = (cameraY_actual + cameraY) / 2;
			if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
			}
			if(millis() - blinkMillis > 350)
			{
				blinkMillis = millis();
				blinkState = !blinkState;
			}
			for(int i = 0; i < length; i++)
			{
				JsonObject& elem = call_log[sortingArray[i]];
				callLogDrawBoxSD(elem, i, cameraY_actual);

			}
			if(blinkState)
				callLogMenuDrawCursor(cursor, cameraY_actual);
					if(mp.buttons.released(BTN_HOME)) {
			mp.exitedLockscreen = true;
			mp.lockscreen(); // Robert
			}	

			if (mp.buttons.released(BTN_A) || mp.buttons.released(BTN_FUN_RIGHT)) {
				mp.buttons.update();
				break;
			}
			if (mp.buttons.released(BTN_FUN_LEFT)) //delete call log entry
			{
				mp.buttons.update();
				call_log.remove(sortingArray[cursor]);
				File file = SD.open("/.core/call_log.json", "w");
				call_log.prettyPrintTo(file);
				file.close();
				return -10;
			}

			if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
				blinkState = 1;
				blinkMillis = millis();
				mp.buttons.update();
				if (cursor == 0) {
					cursor = length - 1;
					if (length > 3) {
						cameraY = -(cursor - 2) * (boxHeight);
					}
				}
				else {
					cursor--;
					if (cursor > 0 && (cursor * (boxHeight) + cameraY + offset) < (boxHeight)) {
						cameraY += (boxHeight);
					}
				}
			}
			if (mp.buttons.released(BTN_DOWN)) { //BUTTON DOWN
				blinkState = 1;
				blinkMillis = millis();
				mp.buttons.update();
				cursor++;
				if ((cursor * (boxHeight) + cameraY + offset) > 80) {
					cameraY -= (boxHeight);
				}
				if (cursor >= length) {
					cursor = 0;
					cameraY = 0;
				}
			}
		}

		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			mp.buttons.update();
			return -2;
		}
		mp.display.setCursor(0, 0);
		mp.display.setTextFont(2);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.setTextSize(1);
		mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
		mp.display.setTextFont(2);
		mp.display.setCursor(0,-2);
		mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Call log");
		mp.display.setCursor(0,-1);
		mp.display.printf("                   %d/40", length);
		mp.display.fillRect(0, 105, 160, 23, TFT_BLACK);
		mp.display.setCursor(5, 110);
		mp.display.print("Erase");
		mp.display.setCursor(125, 110);
		mp.display.print("View");


		mp.update();

	}
	cursor = sortingArray[cursor];
	return cursor;
}

void callLogDrawBoxSD(JsonObject& object, uint8_t i, int32_t y) {
    uint8_t offset = 20;
    uint8_t boxHeight = 28;
	y += i * boxHeight + offset;
	if (y < 0 || y > mp.display.height()) {
		return;
	}
	DateTime date = DateTime(object["dateTime"].as<uint32_t>());
    mp.display.setTextSize(1);
    mp.display.setTextFont(2);
    mp.display.fillRect(1, y + 1, mp.display.width() - 2, boxHeight-1, TFT_DARKGREY);
    mp.display.setTextColor(TFT_WHITE);
    mp.display.setCursor(2, y + 2);
	char buf[100];
	strncpy(buf, "DD.MM.YYYY hh:mm:ss\0", 100);
    mp.display.drawString(date.format(buf), 24, y);
	String temp = object["contact"].as<char*>();
	if(temp == "")
   		mp.display.drawString(object["number"].as<char*>(), 24, y + 12);
	else
   		mp.display.drawString(temp, 24, y + 12);
	
	switch (object["direction"].as<uint8_t>())
	{
	case 0:
		mp.display.drawBitmap(3, y + 6, missedCallIcon, TFT_RED, 2);
		break;
	case 1:
		mp.display.drawBitmap(3, y + 6, outgoingCallIcon, TFT_GREEN, 2);
		break;
	case 2:
		mp.display.drawBitmap(3, y + 6, incomingCallIcon, TFT_BLUE, 2);
		break;
	}
}

void callLogMenuDrawCursor(uint8_t i, int32_t y) {
    uint8_t offset = 20;
    uint8_t boxHeight = 28;
	y += i * boxHeight + offset;
	mp.display.drawRect(0, y, mp.display.width(), boxHeight + 1, TFT_RED);
}

uint8_t showCall(int id, String number, uint32_t dateTime, String contact, String duration, uint8_t direction)
{
	DateTime time = DateTime(dateTime);
	while (1)
	{
		mp.display.fillScreen(TFT_BLACK);
        mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
        mp.display.setTextFont(2);
        mp.display.setCursor(2,-2);
        mp.display.drawFastHLine(0, 14, BUF2WIDTH, TFT_WHITE);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Call log");
		char buf[100];
		strncpy(buf, "DD.MM.YYYY hh:mm:ss\0", 100);
		mp.display.setCursor(5,20);
		mp.display.print(time.format(buf));
		if(contact == "")
		{
			mp.display.setCursor(5, 40);
			mp.display.println(number);
			mp.display.setCursor(5, 60);
		}
		else
		{
			mp.display.setCursor(5, 40);
			mp.display.println(contact);
			mp.display.setCursor(5, 60);
			mp.display.println(number);
			mp.display.setCursor(5, 80);
		}
		int seconds = duration.toInt() % 60;
		int minutes = duration.toInt() / 60;
		mp.display.print("Duration: ");
		if(minutes < 10)
			mp.display.print("0");
		mp.display.print(minutes);
		mp.display.print(":");
		if(seconds < 10)
			mp.display.print("0");
		mp.display.print(seconds);

		mp.display.fillRect(110, 102, 45, 9*2, TFT_GREENYELLOW);
		mp.display.fillRect(5, 102, 45, 9*2, TFT_RED);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.setCursor(9, 103);
		mp.display.print("Delete");
		mp.display.setCursor(122, 103);
		mp.display.print("Call");

		if (mp.buttons.released(BTN_FUN_LEFT)) //BUTTON BACK
		{
			while(!mp.update());
			return 1;
		}
		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			Serial.println("Go back");
			while(!mp.update());
			break;
		}
		if (mp.buttons.released(BTN_A) || mp.buttons.released(BTN_FUN_RIGHT)) // Call
		{
			while(!mp.update());
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
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setTextColor(TFT_WHITE);
				mp.display.setTextSize(1);
				mp.display.setCursor(0, mp.display.height()/2 - 20);
				mp.display.setTextFont(2);
				mp.display.printCenter(F("No SIM inserted!"));
				mp.display.setCursor(0, mp.display.height()/2);
				mp.display.printCenter(F("Insert SIM and reset"));
				uint32_t tempMillis = millis();
				while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
					mp.update();
				while(!mp.update());
			}
			else if(mp.airplaneMode)
			{
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setTextColor(TFT_WHITE);
				mp.display.setTextSize(1);
				mp.display.setCursor(0, mp.display.height()/2 - 20);
				mp.display.setTextFont(2);
				mp.display.printCenter(F("Can't make calls"));
				mp.display.setCursor(0, mp.display.height()/2);
				mp.display.printCenter(F("Turn off airplane mode"));
				uint32_t tempMillis = millis();
				while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
					mp.update();
				while(!mp.update());
			}
			else
			{
				// Serial.println("AT+GSMBUSY=0");
				// mp.waitForOK();
				bool readyForCall = 0;
				uint32_t timeoutMillis = millis(); 
				while(Serial1.available())
					Serial1.read();
				while(!readyForCall && millis() - timeoutMillis < 10000)
				{
					Serial1.println("AT+CCALR?");
					String input = mp.waitForOK();

					uint16_t helper = input.indexOf(" ", input.indexOf("+CCALR:"));
					readyForCall = input.substring(helper + 1, helper + 2).toInt();
					Serial.println(input);
					if(!readyForCall)
					{
						mp.display.fillScreen(TFT_BLACK);
						mp.display.setTextColor(TFT_WHITE);
						mp.display.setTextSize(1);
						mp.display.setCursor(0, mp.display.height()/2 - 20);
						mp.display.setTextFont(2);
						mp.display.printCenter(F("Registering to network"));
						mp.display.setCursor(0, mp.display.height()/2);
						mp.display.printCenter(F("Please wait..."));
						while(!mp.update());
						delay(1000);
					}
				}
			
				mp.networkRegistered = readyForCall;
				if(readyForCall)
				{
					// Serial1.println("AT+CREG?");
					// uint32_t cregMillis = millis();
					// String cregString = "";
					// int8_t tempRegistered = -1;
					// while(tempRegistered != 1 && tempRegistered != 5 && millis() - cregMillis < 1000)
					// {
					// 	if(cregString != "")
					// 	{
					// 		// Serial.println(cregString);
					// 		// Serial.println(".............");
					// 		if(cregString.indexOf("\n", cregString.indexOf("+CREG:")) != -1)
					// 		{
					// 			uint16_t helper = cregString.indexOf(",", cregString.indexOf("+CREG:"));
					// 			Serial.println(cregString.substring(helper + 1,  helper + 2));
					// 			tempRegistered = cregString.substring(helper + 1,  helper + 2).toInt();
					// 		}
					// 	}
					// 	if(cregString != "" && tempRegistered != -1)
					// 	{
					// 		Serial1.println("AT+CREG?");
					// 	}
					// 	cregString = mp.waitForOK();
					// }
					// if(tempRegistered != -1)
					// 	mp.networkRegistered = tempRegistered;
					// Serial.print("network reg: ");
					// Serial.println(mp.networkRegistered);
					// if(mp.networkRegistered != 5 && mp.networkRegistered != 1)
					// {
						// mp.display.fillScreen(TFT_BLACK);
						// mp.display.setTextColor(TFT_WHITE);
						// mp.display.setTextSize(1);
						// mp.display.setCursor(0, mp.display.height()/2 - 20);
						// mp.display.setTextFont(2);
						// mp.display.printCenter(F("Registering to network"));
						// mp.display.setCursor(0, mp.display.height()/2);
						// mp.display.printCenter(F("Please wait..."));
						// while(!mp.update());
					// 	while(Serial1.available())
					// 			Serial1.read();
					// 	Serial1.println("AT+CFUN=1,1");
					// 	char buffer[300];
					// 	bool found = 0;
					// 	memset(buffer, 0, sizeof(buffer));
					// 	Serial1.flush();
					// 	uint32_t timer = millis();
					// 	while(!found)
					// 	{
					// 		if(Serial1.available())
					// 		{

					// 			char test = (char)Serial1.read();
					// 			strncat(buffer, &test, 1);
					// 			Serial.println(buffer);
					// 		}

					// 		if(strstr(buffer, "RDY") != nullptr)
					// 			found = 1;
					// 		if((millis() - timer > 5000 && mp.sim_module_version == 1) ||
					// 		(millis() - timer > 28000 && mp.sim_module_version == 0))
					// 			break;
					// 	}
					// 	delay(2000);
					// 	mp.checkSim();
					// 	delay(500);
					// 	while(Serial1.available())
					// 		Serial1.read();
					// 	Serial1.println("AT+CREG?");
					// 	uint32_t cregMillis = millis();
					// 	String cregString = "";
					// 	int8_t tempRegistered = -1;
					// 	while(tempRegistered != 1 && tempRegistered != 5 && millis() - cregMillis < 10000)
					// 	{
					// 		cregString = mp.waitForOK();
					// 		if(cregString != "")
					// 		{
					// 			Serial.println(cregString);
					// 			Serial.println("-----------");
					// 			if(cregString.indexOf("\n", cregString.indexOf("+CREG:")) != -1)
					// 			{
					// 				uint16_t helper = cregString.indexOf(",", cregString.indexOf("+CREG:"));
					// 				tempRegistered = cregString.substring(helper + 1,  helper + 2).toInt();
					// 			}
					// 		}
					// 		Serial1.println("AT+CREG?");
					// 	}
					// 	mp.networkRegistered = tempRegistered;
					// 	mp.networkModuleInit();
					// 	if(mp.networkRegistered != 5 && mp.networkRegistered != 1)
					// 	{
					// 		mp.display.fillScreen(TFT_BLACK);
					// 		mp.display.setTextColor(TFT_WHITE);
					// 		mp.display.setTextSize(1);
					// 		mp.display.setCursor(0, mp.display.height()/2 - 20);
					// 		mp.display.setTextFont(2);
					// 		mp.display.printCenter(F("Network unavailable"));
					// 		mp.display.setCursor(0, mp.display.height()/2);
					// 		mp.display.printCenter(F("Get a better signal"));
					// 		uint32_t tempMillis = millis();
					// 		while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
					// 			mp.update();
					// 		while(!mp.update());
					// 	}
					// 	else
					// 	{
					// 		callNumber(number);
					// 	}
					// }
					if(mp.signalStrength == 99)
					{
						Serial1.println("AT+CSQ");
						String buffer = "";
						uint32_t current = millis();
						while(buffer.indexOf("+CSQ:") == -1 && millis() - current >= 2000)
							buffer = Serial1.readString();
						if(buffer.indexOf("+CSQ:") != -1)
							mp.signalStrength = buffer.substring(buffer.indexOf(" ", buffer.indexOf("+CSQ:")) + 1, buffer.indexOf(",", buffer.indexOf(" ", buffer.indexOf("+CSQ:")))).toInt();
						if(mp.signalStrength == 99)
						{
							mp.display.fillScreen(TFT_BLACK);
							mp.display.setTextColor(TFT_WHITE);
							mp.display.setTextSize(1);
							mp.display.setCursor(0, mp.display.height()/2 - 20);
							mp.display.setTextFont(2);
							mp.display.printCenter(F("No signal!"));
							mp.display.setCursor(0, mp.display.height()/2);
							mp.display.printCenter(F("Check your antenna"));
							uint32_t tempMillis = millis();
							while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
								mp.update();
							while(!mp.update());
						}
						else
							callNumber(number);
					}
					else
						callNumber(number);
				}
				else
				{
					mp.display.fillScreen(TFT_BLACK);
					mp.display.setTextColor(TFT_WHITE);
					mp.display.setTextSize(1);
					mp.display.setCursor(0, mp.display.height()/2 - 20);
					mp.display.setTextFont(2);
					mp.display.printCenter(F("Network unavailable"));
					mp.display.setCursor(0, mp.display.height()/2);
					mp.display.printCenter(F("Try again later"));
					uint32_t tempMillis = millis();
					while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
						mp.update();
					while(!mp.update());
				}
			}
			while(!mp.update());
			return 0;
		}

		mp.update();
	}
	return 0;
}
void sendMMI(String code)
{
	mp.inCall = 1;
	String buffer = "";
	// Serial1.println("AT+CUSD=1");
	// delay(10);
	Serial1.print("AT+CUSD=1,\"");// at+cusd=1,"*101#", 15
	Serial1.print(code);
	Serial1.println("\", 15");
	uint32_t tempMillis = millis();
	bool cleared = 0;
	while(buffer.indexOf("\r", buffer.indexOf("+CUSD: 0")) == -1 && millis() - tempMillis < 4000)
	{
		if(Serial1.available())
		{
			tempMillis = millis();
			buffer+=(char)Serial1.read();
			Serial.println(buffer);
			if(buffer.indexOf("+CUSD: 0") != -1 && !cleared)
			{
				buffer = "+CUSD: 0";
				cleared = 1;
			}
		}
		mp.display.fillScreen(TFT_WHITE);
		mp.display.drawRect(9, 34, 142, 60, TFT_BLACK);
		mp.display.drawRect(10, 35, 140, 58, TFT_BLACK);
		mp.display.fillRect(11, 36, 138, 56, TFT_WHITE);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(55, 54);
		mp.display.printCenter("Sending USSD code");
		while(!mp.update());
	}
	if(buffer.indexOf("\r", buffer.indexOf("+CUSD: 0")) == -1)
	{
		mp.inCall = 0;
		mp.display.fillScreen(TFT_WHITE);
		mp.display.drawRect(9, 34, 142, 60, TFT_BLACK);
		mp.display.drawRect(10, 35, 140, 58, TFT_BLACK);
		mp.display.fillRect(11, 36, 138, 56, TFT_WHITE);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(55, 54);
		mp.display.printCenter("Invalid USSD code!");
		while(!mp.update());
		uint32_t tempMillis = millis();
		while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
			mp.update();
		return;
	}
	uint16_t helper = buffer.indexOf("\"") + 1;
	buffer = buffer.substring(helper, buffer.indexOf("\"", helper + 1));
	Serial.println(buffer);
	int32_t y = 1;
	while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
	{
		mp.display.fillScreen(TFT_WHITE);
		// mp.display.drawRect(10, 20, 142, 88, TFT_BLACK);
		// mp.display.drawRect(11, 21, 141, 86, TFT_BLACK);
		mp.display.fillRect(12, 22, 138, 84, TFT_WHITE);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(5, y);
		for(int i = 0; i < buffer.length();i++)
		{
			mp.display.print(buffer[i]);
			if(mp.display.getCursorX() > 150)
			{
				mp.display.print("\n");
				mp.display.setCursor(5, mp.display.getCursorY());
			}
		}
		if (mp.buttons.repeat(BTN_DOWN, 3)) { //BUTTON DOWN
			Serial.println(mp.display.cursor_y);
			if (mp.display.cursor_y >= 110)
			{
				y -= 4;
			}
		}
		if (mp.buttons.repeat(BTN_UP, 3)) { //BUTTON UP
			Serial.println(y);
			if (y < 1)
				y += 4;
		}
		mp.update();
	}
	while(!mp.update());
	mp.inCall = 0;
}