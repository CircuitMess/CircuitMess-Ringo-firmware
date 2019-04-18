#include "mainMenu.h"
uint16_t directoryCount = 0;
String directories[100];
String BinaryFiles[100];
uint16_t binaryCount = 0;
void listDirectories(const char * dirname) {
	
	directoryCount = 0;
	Serial.printf("Listing directory: %s\n", dirname);
	
	SDAudioFile root = mp.SD.open(dirname);
	if (!root) {
		Serial.println(F("Failed to open directory"));
		return;
	}
	if (!root.isDirectory()) {
		Serial.println(F("Not a directory"));
		return;

	}
	int counter = 0;

	SDAudioFile file = root.openNextFile();
	while (file) {

		if (file.isDirectory()) {
			String Name(file.name());
			Serial.println(Name);
			if(Name != "/Images\0" && Name != "/Music\0" && Name != "/Video\0"
			 && Name != "/System Volume Information\0" && mp.SD.exists(String(Name + "/" + Name + ".BIN")))
			{
				Serial.println(Name);
				directories[directoryCount] = Name.substring(1);
				counter++;
				directoryCount++;
			}
		  }
		file = root.openNextFile();
	}
}
void listBinaries(const char * dirname, uint8_t levels) {
	
	binaryCount = 0;
	Serial.printf("Listing directory: %s\n", dirname);

	SDAudioFile root = mp.SD.open(dirname);
	if (!root) {
		Serial.println(F("Failed to open directory"));
		return;
	}
	if (!root.isDirectory()) {
		Serial.println(F("Not a directory"));
		return;

	}
	int counter = 1;

	SDAudioFile file = root.openNextFile();
	while (file) {

		/*if (file.isDirectory()) {
		  Serial.print(F("  DIR : "));
		  Serial.println(file.name());
		  if (levels) {
		  listBinaries(fs, file.name(), levels - 1);
		  }
		  }
		  else {
		  String Name = file.name();
		  if (Name.endsWith(F(F(".bin")))
		  {
		  Serial.print(file.name());
		  BinaryFiles[counter-1] = file.name();
		  }
		  }*/
		char temp[100];
		// file.getName(temp, 100);
		String Name(file.name());
		Serial.println(Name);
		if (Name.endsWith(F(".BIN")) || Name.endsWith(F(".bin")))
		{
			Serial.print(counter);
			Serial.print(F(".   "));
			Serial.println(Name);
			BinaryFiles[counter - 1] = Name;
			counter++;
			binaryCount++;
		}
		file = root.openNextFile();
	}
}
int16_t scrollingMainMenu()
{
	bool previousButtonState = 0;
	bool cursorState = 0;
	uint16_t index = 0;
	uint8_t cursorX = 0;
	uint8_t cursorY = 0;
	uint8_t elements = 10 + directoryCount; //10 default apps
	Serial.println(elements);
	delay(5);
	uint8_t x_elements = 3;
	uint8_t y_elements = ceil((float)elements/x_elements);
	
	uint8_t pageNumber;
	if(elements < 6)
		pageNumber = 0;
	else

		pageNumber = ceil((float)(elements - 6)/3);
	
	Serial.println(pageNumber);
	Serial.println(y_elements);
	delay(5);
	uint8_t pageIndex = 0;
	uint8_t cameraY = 0;
	String appNames[] = {"Clock", "Calculator", "Flashlight", "Calendar", "Invaders"};
	String passcode = "";
	uint32_t passcodeMillis = millis();
	uint32_t elapsedMillis = millis();
	uint32_t elapsedMillis2 = millis();
	bool newScreen = 1;
	mp.display.fillScreen(TFT_BLACK);
	while(!mp.update());
	while (1)
	{
		
		mp.display.fillRect(0,0,mp.display.width(), 14, TFT_BLACK);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_WHITE);

		// Draw the icons
		if(newScreen)
		{
			mp.display.fillScreen(TFT_BLACK);
			for (int i = 0; i < 6;i++)
			{
				uint8_t tempX = i%x_elements;
				uint8_t tempY = i/x_elements;
				switch (pageIndex * 3 + i)
				{
					case 0:
						// Serial.println(index);
						// Serial.println(i);
						// Serial.println(tempX);
						// Serial.println(tempY);
						// Serial.println(F("-------------"));
						// delay(5);
						mp.display.drawIcon(bigMessages, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 1:
						mp.display.drawIcon(bigMedia, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 2:
						mp.display.drawIcon(bigContacts, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 3:
						mp.display.drawIcon(bigSettings, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 4:
						mp.display.drawIcon(bigPhone, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 5:
						mp.display.drawIcon(bigApps, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 6:
						mp.display.drawIcon(clock_icon, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 7:
						mp.display.drawIcon(calculator_icon, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 8:
						mp.display.drawIcon(flash_icon, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					case 9:
						mp.display.drawIcon(calendar_icon, 4 + tempX*52, 18 + tempY*56, width, bigIconHeight, 2);
						break;
					default: 
						if(pageIndex * 3 + i < elements)
						{
							Serial.println(directories[pageIndex * 3 + i - 10]);
							delay(5);
							if(mp.SD.exists(String("/" + directories[pageIndex * 3 + i - 10] + "/icon.bmp")))
								mp.display.drawBmp(String("/" + directories[pageIndex * 3 + i - 10] + "/icon.bmp"), 4 + tempX * 52, 18 + tempY * 56, 2);
							else
								mp.display.drawIcon(defaultIcon, 4 + tempX * 52, 18 + tempY * 56, width, bigIconHeight, 2);
							
						}
						break;
				}
			}
			newScreen = 0;
		}

		mp.display.fillRect(0, 0, 160, 12, TFT_BLACK);
		while(cursorY*x_elements + cursorX >= elements)
			cursorX--;
	
		index = cursorY * x_elements + cursorX;
		// Serial.println(index);
		// Serial.println(pageIndex);
		// Serial.println(F("-----------------"));
		// delay(5);
		mp.display.setTextFont(2);
		mp.display.setCursor(0,-2);
		mp.display.drawFastHLine(0, 14, mp.display.width(), TFT_WHITE);
		if(index < 10)
			mp.display.print(titles[index]);
		else
			mp.display.print(directories[index-10]);
		
		if (millis() - elapsedMillis >= 250) {
			elapsedMillis = millis();
			cursorState = !cursorState;
		}
		if (millis() - elapsedMillis2 >= 100) {
			elapsedMillis2 = millis();
			previousButtonState = mp.kpd.pin_read(BTN_B);
		}
		if (millis() - passcodeMillis >= 1000)
			passcode = "";

		// mp.display.drawIcon(bigMessages, 4 + cursorX*52, 18 + cursorY*56, width, bigIconHeight, 2);

		mp.display.drawRect(3 + cursorX * 52, 17 + (cameraY) * 56, 50, 54, cursorState ? TFT_RED : TFT_BLACK);
		mp.display.drawRect(2 + cursorX * 52, 16 + (cameraY) * 56, 52, 56, cursorState ? TFT_RED : TFT_BLACK);

		///////////////////////////////////////
		//////Checking for button input////////
		///////////////////////////////////////
		if (mp.buttons.released(BTN_A)) //CONFIRM
		{
			// osc->note(75, 0.05);
			// osc->play();
			while (!mp.update());
			return cursorY * x_elements + cursorX;  //returns index of selected icon
		}
		if (mp.buttons.released(BTN_UP)) //UP
		{
			mp.display.drawRect(3 + cursorX * 52, 17 + (cameraY) * 56, 50, 54, TFT_BLACK);
			mp.display.drawRect(2 + cursorX * 52, 16 + (cameraY) * 56, 52, 56, TFT_BLACK);
			osc->note(75, 0.05);
			osc->play();
			passcode += "UP";
			passcodeMillis = millis();
			mp.leds[0] = CRGB::Blue;
			mp.leds[7] = CRGB::Blue;
			while(!mp.update());
			mp.vibration(200);
			FastLED.clear();
			while (!mp.update());

			if (cursorY == 0) 
			{
				newScreen = 1;
				cursorY = y_elements-1;
				
				if(pageNumber > 0)
				{
					cameraY = 1;
					pageIndex = pageNumber;
				}
			}
			else if(cameraY % 2 == 1)
			{
				cursorY--;
				cameraY = 0;
			}
			else if(cameraY % 2 == 0 && pageIndex > 0)
			{
				newScreen = 1;
				cameraY = 0;
				cursorY--;
				pageIndex--;
			}
			elapsedMillis = millis();
			cursorState = 1;
		}
		if (mp.buttons.released(BTN_DOWN))//DOWN
		{
			mp.display.drawRect(3 + cursorX * 52, 17 + (cameraY) * 56, 50, 54, TFT_BLACK);
			mp.display.drawRect(2 + cursorX * 52, 16 + (cameraY) * 56, 52, 56, TFT_BLACK);
			osc->note(75, 0.05);
			osc->play();
			passcode += "DOWN";
			passcodeMillis = millis();
			mp.leds[3] = CRGB::Blue;
			mp.leds[4] = CRGB::Blue;
			while (!mp.update());
			mp.vibration(200);
			FastLED.clear();
			while (!mp.update());

			if (cursorY == y_elements - 1) 
			{
				newScreen = 1;
				cursorY = 0;
				pageIndex = 0;
				cameraY = 0;
			}
			else if(cameraY % 2 == 0)
			{
				cursorY++;
				cameraY++;
			}
			else if (cameraY % 2 == 1 && pageIndex < pageNumber)
			{
				newScreen = 1;
				cameraY = 1;
				cursorY++;
				pageIndex++;
			}
			
			elapsedMillis = millis();
			cursorState = 1;
		}
		if (mp.buttons.released(BTN_LEFT)) //LEFT
		{
			mp.display.drawRect(3 + cursorX * 52, 17 + (cameraY) * 56, 50, 54, TFT_BLACK);
			mp.display.drawRect(2 + cursorX * 52, 16 + (cameraY) * 56, 52, 56, TFT_BLACK);
			osc->note(75, 0.05);
			osc->play();
			passcode += "LEFT";
			passcodeMillis = millis();
			mp.leds[6] = CRGB::Blue;
			mp.leds[5] = CRGB::Blue;
			while (!mp.update());
			mp.vibration(200);
			FastLED.clear();
			while (!mp.update());

			if (cursorX == 0) {
				cursorX = x_elements - 1;
			}
			else
				cursorX--;
			elapsedMillis = millis();
			cursorState = 1;
		}
		if (mp.buttons.released(BTN_RIGHT))//RIGHT
		{
			mp.display.drawRect(3 + cursorX * 52, 17 + (cameraY) * 56, 50, 54, TFT_BLACK);
			mp.display.drawRect(2 + cursorX * 52, 16 + (cameraY) * 56, 52, 56, TFT_BLACK);
			osc->note(75, 0.05);
			osc->play();
			passcode += "RIGHT";
			passcodeMillis = millis();
			mp.leds[1] = CRGB::Blue;
			mp.leds[2] = CRGB::Blue;
			while (!mp.update());
			mp.vibration(200);
			FastLED.clear();
			while (!mp.update());

			if (cursorX == x_elements - 1) {
				cursorX = 0;
			}
			else
				cursorX++;
			elapsedMillis = millis();
			cursorState = 1;
		}
		if (mp.buttons.released(BTN_B))//B BUTTON
		{

			mp.leds[0] = CRGB::Red;
			mp.leds[7] = CRGB::Red;
			FastLED.show();
			mp.vibration(200);
			FastLED.clear();
			while(!mp.update());
			return -2;
		}
		if (passcode == "UPUPDOWNDOWNLEFTRIGHTLEFTRIGHT")
			return -3;
		mp.update();

	}
}
void mainMenu()
{
	Serial.println(F("entered main menu"));
	mp.dataRefreshFlag = 0;
	listDirectories("/");
	while (1)
	{
		int8_t index = scrollingMainMenu();
		Serial.println(index);
		delay(5);
		if(index < 10) 
		{
			if (titles[index] == "Apps")
			{
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setCursor(0, mp.display.height()/2 - 20);
				mp.display.setTextFont(2);
				mp.display.printCenter(F("PLACEHOLDER"));
				mp.display.setCursor(0, mp.display.height()/2);
				mp.display.printCenter(F("PLACEHOLDER"));
				uint32_t tempMillis = millis();
				while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
					mp.update();
				while(!mp.update());
			}

			if (titles[index] == "Messages")
			{
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setTextColor(TFT_WHITE);
				if(mp.simInserted && !mp.airplaneMode)
				{
					mp.display.setCursor(0, mp.display.height()/2 - 16);
					mp.display.printCenter(F("Loading messages..."));
					while(!mp.update());
					messagesApp();
				}
				else if(!mp.simInserted)
				{
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
					mp.display.setCursor(0, mp.display.height()/2 - 20);
					mp.display.setTextFont(2);
					mp.display.printCenter(F("Can't access SMS!"));
					mp.display.setCursor(0, mp.display.height()/2);
					mp.display.printCenter(F("Turn off airplane mode"));
					uint32_t tempMillis = millis();
					while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
						mp.update();
					while(!mp.update());
				}

			}

			if (titles[index] == "Media")
			{
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setTextColor(TFT_WHITE);
				if(mp.SDinsertedFlag)
				mediaApp();
				else
				{
					mp.display.setCursor(0, mp.display.height()/2 - 20);
					mp.display.setTextFont(2);
					mp.display.printCenter(F("No SD inserted!"));
					mp.display.setCursor(0, mp.display.height()/2);
					mp.display.printCenter(F("Insert SD card and reset"));
					uint32_t tempMillis = millis();
					while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
						mp.update();
					while(!mp.update());
				}
			}

			if (titles[index] == "Phone")
			{
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setTextColor(TFT_WHITE);
				if(mp.simInserted && !mp.airplaneMode)
					phoneApp();
				else if(!mp.simInserted)
				{
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
			}

			if (titles[index] == "Contacts") {
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setTextColor(TFT_WHITE);
				if(mp.SDinsertedFlag && !mp.airplaneMode)
				{
					mp.display.fillScreen(TFT_BLACK);
					mp.display.setCursor(0,mp.display.height()/2 -16);
					mp.display.printCenter(F("Loading contacts..."));
					contactsAppSD();
				}
				else if(!mp.SDinsertedFlag)
				{
					mp.display.setCursor(0, mp.display.height()/2 - 20);
					mp.display.setTextFont(2);
					mp.display.printCenter(F("No SD inserted!"));
					mp.display.setCursor(0, mp.display.height()/2);
					mp.display.printCenter(F("Insert SD and reset"));
					uint32_t tempMillis = millis();
					while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
						mp.update();
					while(!mp.update());
				}
				else if(mp.airplaneMode)
				{
					mp.display.setCursor(0, mp.display.height()/2 - 20);
					mp.display.setTextFont(2);
					mp.display.printCenter(F("Can't access contacts!"));
					mp.display.setCursor(0, mp.display.height()/2);
					mp.display.printCenter(F("Turn off airplane mode"));
					uint32_t tempMillis = millis();
					while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
						mp.update();
					while(!mp.update());
				}
			}

			if (titles[index] == "Settings")
				if(settingsApp())
					return;
			if(titles[index] == "Clock")
				clockApp();
			if(titles[index] == "Calculator")
				calculatorApp();
			if(titles[index] == "Flashlight")
				flashlightApp();
			if(titles[index] == "Calendar")
				calendarApp();
			if (index == -2)
			{
				Serial.println(F("pressed"));
				break;
			}
		}
		else
		{
			mp.display.fillScreen(TFT_BLACK);
			mp.display.setCursor(0,mp.display.height() / 2 - 16);
			mp.display.printCenter(F("LOADING NOW..."));
			while(!mp.update());

			String foo = directories[index - 10]; //10 is the number of preloaded apps
			initWavLib();
			mp.updateFromFS(String("/" + foo + "/" + foo + ".bin"));
		}
		mp.update();
	}
}