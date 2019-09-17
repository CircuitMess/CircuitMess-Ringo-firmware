#include "phoneApp.h"
void phoneApp() 
{
	mp.dataRefreshFlag = 0;
	String callBuffer = "";
	char key = NO_KEY;
	mp.display.setTextWrap(0);
	mp.display.setTextFont(2);
	bool plusSign = 0;
	uint32_t blinkMillis = millis();
	bool blinkState = 1;


	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	
	uint8_t offset = 19;
	uint8_t boxHeight = 28;
	String favs[50] = {};
	String favsNames[50] = {};

	int32_t cursorPos = 0;
	int8_t length = 1; //5 favs + inputbox
	//int8_t lenActual = 0;
	String favsBuff = sortAndGetFav();
	int entries = getContact(favsBuff, 0).toInt();
	String tempBuff[50] = "";
	int i = 1;
	Serial.println(favsBuff);
	favsBuff = favsBuff.substring(favsBuff.indexOf("|")+1, favsBuff.length());
	Serial.println(favsBuff);

    char* ptr = strtok(&favsBuff[0], "|");

    while(ptr != NULL) 
	{
        favs[i] = ptr;
		favsNames[i] = mp.checkContact(favs[i]);
        ptr = strtok(NULL, "|");
		i++;
    }

	//Serial.println("endbuff");

	for(int d = 1; d <= entries; d++)
	{
		length++;
		Serial.println(favs[d]);
	}
	while (1)
	{
		length = 2;
		favs[0] = callBuffer;
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, 0);
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) 
		{
			cameraY_actual = cameraY;
		}
		if(millis() - blinkMillis >= 350)
		{
			blinkMillis = millis();
			blinkState = !blinkState;
		}	
		//-------------------------------DRAW 0 START-----------------------------------------
		int i = -1;
		for(int k = 0; k < 5; k++)
		{
			if(k == 0)
				phoneMenuDrawBox("", favs[k], i+1, cameraY_actual, blinkState, cursorPos, k);
			else if(favs[k].compareTo("") != 0)
			{
				phoneMenuDrawBox(favsNames[k], favs[k], i+1, cameraY_actual, blinkState, cursorPos, k);
				length++;
				/*
				if(blinkState)
				{
					Serial.println("PHONEAPP-------------");
					Serial.println(favs[k]);
					Serial.println("PHONEAPP-------------");
				}
				*/
			}
				i++;
		}
		if(blinkState)
		{
			phoneMenuDrawCursor(cursorPos, cameraY_actual);
		}	
		drawStuff();
		//-------------------------------DRAW 0 END-------------------------------------------
		
		if(mp.buttons.released(BTN_UP))
		{
			blinkState = 1;
			blinkMillis = millis();
			while(!mp.update());

				if (cursorPos == 0)
				{
					cursorPos = length - 2;
					if (length > 2)
					{
						cameraY = -(cursorPos - 2) * (boxHeight + 1);
					}
				}
				else
				{
					cursorPos--;
					if (cursorPos > 0 && (cursorPos * (boxHeight + 1) + cameraY + offset) < (boxHeight + 1))
					{
						cameraY += (boxHeight + 1);
					}
				}
				//Serial.println("-----------------------UP");
				//Serial.println(cursorPos);
				//Serial.println(length);
				//Serial.println(lenActual);
				//Serial.println("-----------------------");
		}
		if (mp.buttons.released(BTN_DOWN)) 
		{
			blinkState = 1;
			blinkMillis = millis();
			while (!mp.update());
				cursorPos++;
				if ((cursorPos * (boxHeight + 1) + cameraY + offset) > 80)
				{
					cameraY -= (boxHeight + 1);
				}
				if (cursorPos >= length - 1)
				{
					cursorPos = 0;
					cameraY = 0;
				}
				//Serial.println("---------------------DOWN");
				//Serial.println(cursorPos);
				//Serial.println(length);
				//Serial.println(lenActual);
				//Serial.println("-----------------------");
		}
		key = NO_KEY;
		for(int i = 0; i < 12; i++) 
		{
			if(mp.buttons.released(i)) 
			{
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
		if(mp.buttons.released(BTN_HOME)) 
		{
			mp.exitedLockscreen = true;
			mp.lockscreen();
		}
		if(mp.buttons.held(BTN_0, 10))
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

		if (mp.buttons.released(BTN_A) && (callBuffer != "" || cursorPos != 0))//initiate call
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
					if(mp.sim_module_version == 1)
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
					else
					{
						Serial1.println("AT+CREG?");
						String input = mp.waitForOK();

						uint16_t helper = input.indexOf(",", input.indexOf("+CREG:"));
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
				}
				mp.networkRegistered = readyForCall;
				if(readyForCall)
				{
					Serial1.println("AT+CMGF=1");
					mp.waitForOK();
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
								if(cursorPos == 0)
								{
									
									callNumber(callBuffer);
								}
								else
								{
									callBuffer = favs[cursorPos];
									callNumber(callBuffer);
								}
								while(!mp.update());
							}
							callBuffer = "";
						}
					}
					else
					{
						
						if(callBuffer.startsWith("*") && cursorPos == 0)
						{
							sendMMI(callBuffer);
						}
						else
						{
							if (cursorPos == 0)
							{
								callNumber(callBuffer);
							}
							else
							{
								callBuffer = favs[cursorPos];
								callNumber(callBuffer);
							}

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
		//-------------------------------DRAW 1 START-----------------------------------------
		if(blinkState)
			phoneMenuDrawCursor(cursorPos, cameraY_actual);
		drawStuff();
		//-------------------------------DRAW 1 END-------------------------------------------
		if (mp.buttons.released(BTN_B)) //BACK BUTTON
			break;

		mp.update();
	}
	while(!mp.update());
}

String getContact(String data, int no)
{
	String ret = "";
	int index = data.indexOf("|") + 1;
	int prevIndex = 0;
	if(no == 0)
	{
		ret = data.substring(0, data.indexOf("|"));
	}
	else
	{
		for (int i = 0; i < no; i++)
		{
			prevIndex = index;
			index += data.indexOf("|", index) + 1;
		}
		ret = data.substring(prevIndex, index - 1);
	}
	return ret;
}

void drawStuff()
{
	mp.display.setTextColor(TFT_WHITE);
	mp.display.setTextSize(1);
	//mp.display.fillRect(0, 79, mp.display.width(), 26, TFT_DARKGREY);
	mp.display.fillRect(0, 0, mp.display.width(), 18, TFT_BLACK);
	mp.display.fillRect(0, 110, mp.display.width(), 20, TFT_BLACK);
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
}

void phoneMenuDrawBox(String name, String number, uint8_t i, int32_t y, bool blState, int32_t cPos, bool inputFlag) 
{
    uint8_t offset = 19;
    uint8_t boxHeight = 28;
	y += i * boxHeight + offset;
	int tempCursor = 0;
	int k;
	if (y < 0 || y > mp.display.height()) 
	{
		return;
	}
    mp.display.setTextSize(1);
    mp.display.setTextFont(2);
    mp.display.fillRect(1, y + 1, mp.display.width() - 2, boxHeight-1, TFT_DARKGREY);
    mp.display.setTextColor(TFT_WHITE);
    mp.display.setCursor(2, y + 2);
    mp.display.drawString(name, 4, y);
	if(blState && cPos == 0 && !inputFlag)
	{
		mp.display.setTextSize(2);
		mp.display.setCursor(4, y - 2);
		mp.display.print(number);
		if (mp.display.cursor_x + 4  >= mp.display.width())
		{
   			mp.display.fillRect(1, y + 1, mp.display.width() - 2, boxHeight-1, TFT_DARKGREY);
			mp.display.setCursor(mp.display.width() - mp.display.cursor_x - 14, y - 2);
			mp.display.print(number);
		}
		// mp.display.drawString(number + "|", 4, y - 2);
		mp.display.drawFastVLine(mp.display.getCursorX() + 1, mp.display.getCursorY() + 6, 20, TFT_WHITE);
		mp.display.drawFastVLine(mp.display.getCursorX() + 2, mp.display.getCursorY() + 6, 20, TFT_WHITE);
		Serial.println(mp.display.cursor_x);
		// if (number.length() > 9)
		// {
		// 	mp.display.fillRect(0, 19, mp.display.width() - 1, 26, TFT_DARKGREY);
		// 	mp.display.setCursor((mp.display.cursor_x - (14 * (number.length() - 8))), 17);
		// 	mp.display.print(number + "|");
		// }
		// mp.display.setCursor(0, 76);
		// mp.display.setTextSize(2);

		
	}
	else if((cPos == 0 && !inputFlag) || i == 0)
	{
		mp.display.setTextSize(2);
		mp.display.setCursor(4, y - 2);
		mp.display.print(number);
		if (mp.display.cursor_x + 4  >= mp.display.width())
		{
   			mp.display.fillRect(1, y + 1, mp.display.width() - 2, boxHeight-1, TFT_DARKGREY);
			mp.display.setCursor(mp.display.width() - mp.display.cursor_x - 14, y - 2);
			mp.display.print(number);
		}
	}
	else
	{
		mp.display.setTextSize(1);
		mp.display.drawString(number, 4, y + 12);
	}
		
}

void phoneMenuDrawCursor(uint8_t i, int32_t y) 
{
    uint8_t offset = 19;
    uint8_t boxHeight = 28;
	y += i * boxHeight + offset;
	mp.display.drawRect(0, y, mp.display.width(), boxHeight + 1, TFT_RED);
}


void phoneMenuInputBox(uint8_t i, int32_t y, String callBuffer) 
{
	uint8_t offset = 19;
    uint8_t boxHeight = 28;
	y += offset + 1;
	if (y < 0 || y > mp.display.height()) 
	{
		return;
	}
    mp.display.setFreeFont(TT1);
    mp.display.setTextSize(1);
    mp.display.fillRect(1, y + 1, mp.display.width() - 2, boxHeight - 2, TFT_DARKGREY);
    //mp.display.drawBitmap(2, y + 4, newContactIcon, TFT_WHITE, 2);
    mp.display.setTextColor(TFT_WHITE);
    mp.display.setCursor(32, y + 6);
    mp.display.setTextFont(2);
    mp.display.print(callBuffer);


	if (mp.display.cursor_x + 4 >= mp.display.width())
	{
		mp.display.fillRect(0, 79, mp.display.width(), 26, TFT_DARKGREY);
		mp.display.setCursor(mp.display.width() - mp.display.cursor_x - 14, 76);
		mp.display.print(callBuffer);
	}
	
}

void callLog() 
{
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
		if(!file)
			Serial.println("CONTACTS ERROR");
	}

	jb.clear();
	JsonArray& jarr = jb.parseArray(file);
	file.close();

	if(!jarr.success())
	{
		Serial.println("Error");
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



int callLogMenu(JsonArray& call_log, int prevCursor)
{
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
	if (length > 2 && cursor > 2) 
	{
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
				mp.lockscreen(); 
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

void callLogDrawBoxSD(JsonObject& object, uint8_t i, int32_t y) 
{
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

void callLogMenuDrawCursor(uint8_t i, int32_t y) 
{
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
					if(mp.sim_module_version == 1)
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
					else
					{
						Serial1.println("AT+CREG?");
						String input = mp.waitForOK();

						uint16_t helper = input.indexOf(",", input.indexOf("+CREG:"));
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
	int32_t y = -1;
	while(!mp.buttons.released(BTN_B))
	{
		mp.display.fillScreen(TFT_WHITE);
		// mp.display.drawRect(10, 20, 142, 88, TFT_BLACK);
		// mp.display.drawRect(11, 21, 141, 86, TFT_BLACK);
		mp.display.fillRect(12, 22, 138, 84, TFT_WHITE);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(2, y);
		mp.display.setTextWrap(0);
		
		/* 		for(int i = 0; i < buffer.length();i++)
		{
			mp.display.print(buffer[i]);
			if(mp.display.getCursorX() > 150)
			{
				mp.display.print("\n");
				mp.display.setCursor(5, mp.display.getCursorY());
			}
		} */
		for(uint16_t i = 0; i < buffer.length(); i++)
		{
			int16_t leftX = 154 - mp.display.getCursorX();
			if(mp.display.getCursorX() < 2)	mp.display.setCursor(2, mp.display.getCursorY()); 
			bool lineFlag = false;
			if( buffer[i] == ' ' || i==0) {
				String helpString = "";
				for(uint8_t j = 1; j < 24; j++){
				if (j == 21) {
					lineFlag = true;
					mp.display.print(buffer[i]);
				}
				else if(buffer[i+j] != ' ' && i+j != buffer.length()-1) helpString += buffer[i+j];
				else {
					int16_t oldX = mp.display.getCursorX();
		        	int16_t oldY = mp.display.getCursorY();
					mp.display.setCursor(0, -20);
					mp.display.print(helpString);
					int16_t awayX = mp.display.getCursorX();
					mp.display.setCursor(oldX, oldY);
					//mp.display.print(content[i]);
					if(awayX > leftX) mp.display.println("");
					else mp.display.print(buffer[i]);
					if(mp.display.getCursorX() < 2)	mp.display.setCursor(2, mp.display.getCursorY()); 
					break;
					}
				}
			}
			else mp.display.print(buffer[i]);
			if (lineFlag && mp.display.getCursorX() > 144 ) {
				mp.display.print("-");
				mp.display.println();
				mp.display.setCursor(2, mp.display.getCursorY());
				lineFlag = false;
			}

			if(mp.display.getCursorY() > 120)
				break; 
		}
		if (mp.buttons.repeat(BTN_DOWN, 3)) { //BUTTON DOWN
			if (mp.display.cursor_y >= 96)
			{
				y -= 4;
				Serial.println(y);
			}
		}

		if (mp.buttons.repeat(BTN_UP, 3)) { //BUTTON UP
			if (y < -1)
			{
				y += 4;
				Serial.println(y);
			}
		}
		mp.display.fillRect(0, 111, 160, 20, TFT_DARKGREY);
		mp.display.setCursor(2, 111);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Press B to go back");
	
		mp.update();
	}
	mp.buttons.update();
	mp.inCall = 0;
}

String sortAndGetFav()
{
	String ret = "";
	uint16_t contactCount = 0;
	String contacts[50][2] = {};
	String numBuf [50] = {};
	int i, j;
	
	//OPEN FILES START
	File file1 = SD.open("/.core/contacts.json", "r");
	if (file1.size() < 2)
	{
		Serial.println("Override");
		file1.close();
		jb.clear();
		JsonArray& contactsjarr = jb.createArray();
		delay(10);
		File file2 = SD.open("/.core/contacts.json", "w");
		contactsjarr.prettyPrintTo(file2);
		file2.close();
		file1 = SD.open("/.core/contacts.json", "r");
		if (!file1)
			Serial.println("CONTACTS ERROR");
	}
	jb.clear();
	JsonArray& contactsjarr = jb.parseArray(file1);
	file1.close();
	//OPEN FILES END
	
	if(!contactsjarr.success())
	{
		Serial.println("Error");
	}
	else
	{
		i = 0;
		Serial.println("json contacts loop start---------------");
		for (JsonObject& elem : contactsjarr) 
		{
			contacts[i][0] = elem["number"].as<char*>();
			contacts[i][1] = "0";
			numBuf[i] = contacts[i][0];
			Serial.println(contacts[i][0]);
			contactCount++;
			i++;
		}
		Serial.println("json contacts loop end---------------");
	}
	
	File file = SD.open("/.core/call_log.json", "r");
	if(file.size() < 2)
	{
		Serial.println("Override");
		file.close();
		JsonArray& logjarr = jb.createArray();
		delay(10);
		File file1 = SD.open("/.core/call_log.json", "w");
		logjarr.prettyPrintTo(file1);
		file1.close();
		file = SD.open("/.core/call_log.json", "r");
		if(!file)
			Serial.println("CALL LOG ERROR");
	}
	jb.clear();
	JsonArray& logjarr = jb.parseArray(file);
	file.close();

	if(!logjarr.success())
	{
		Serial.println("Error");
	}
	else
	{
		int tempInt = 0;
		for (i = 0; i < contactCount; i++)
		{
			/*
			Serial.println("loop ins start----------------");
			Serial.println(" ");
			Serial.print("i: ");
			Serial.print(i);
			Serial.println(" ");
			*/
			for (JsonObject &logelem : logjarr)
			{
				/*
				Serial.println("	json loop ins start---------------");
				Serial.println(" ");
				Serial.print("		number from log: ");
				Serial.print(logelem["number"].as<char *>());
				Serial.println(" ");
				Serial.print("		number from array: ");
				Serial.print(contacts[i][0]);
				Serial.println(" ");
				*/
				if (contacts[i][0] == logelem["number"].as<char *>())
				{
					//Serial.println("			matches");
					tempInt = contacts[i][1].toInt();
					tempInt++;
					contacts[i][1] = String(tempInt);
				}
				//Serial.println("	json loop ins end---------------");
			}
			//Serial.println("loop ins end----------------");
			Serial.println("Contact: ");
			Serial.println(contacts[i][0]);
			Serial.println("Score: ");
			Serial.println(contacts[i][1]);
		}
	}
	Serial.println("SORTED BELOW THIS LINE");
	//SORT START
	String temp = "", temp2 = "";
	for(i = 0; i < contactCount; i++)
	{		
		for(j = i + 1; j < contactCount; j++)
		{
			if(contacts[i][1].toInt() < contacts[j][1].toInt())
			{
				temp  = contacts[i][1];
				temp2 = contacts[i][0];
				contacts[i][1] = contacts[j][1];
				contacts[i][0] = contacts[j][0];
				contacts[j][1] = temp;
				contacts[j][0] = temp2;
			}
		}
	}
	//SORT END
	ret = String(contactCount) + "|";
	for(i = 0; i < contactCount; i++)
	{
		ret += contacts[i][0] + "|";
		/*
		Serial.println("Contact: ");
		Serial.println(contacts[i][0]);
		Serial.println("Score: ");
		Serial.println(contacts[i][1]);
		*/
	}
	Serial.println(ret);
	return ret;
}

