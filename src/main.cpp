#include <Arduino.h>
#include "main.h"
#include "mainMenu.h"
MAKERphone mp;
Oscillator *osc;
int backgroundColors[7] PROGMEM = {
	TFT_CYAN,
	TFT_GREEN,
	TFT_RED,
	TFT_YELLOW,
	TFT_WHITE,
	TFT_ORANGE,
	TFT_PINK
};
String backgroundColorsNames[7] PROGMEM = {
	"Cyan",
	"Green",
	"Red",
	"Yellow",
	"White",
	"Orange",
	"Pink"
};
String titles[9] PROGMEM = {
	"Phone",
	"Contacts",
	"Messages",
	"Settings",
	"Media",
	"Clock",
	"Calculator",
	"Flashlight",
	"Calendar"
};
StaticJsonBuffer<capacity> jb;
uint16_t audioCount = 0;
String audioFiles[100];

void menuDrawBox(String text, uint8_t i, int32_t y) {
	uint8_t scale;
	uint8_t offset;
	uint8_t boxHeight;
	if(mp.resolutionMode)
	{
		scale = 1;
		offset = menuYOffset;
		boxHeight = 7;
	}
	else
	{
		scale = 2;
		offset = 17;
		boxHeight = 15;
	}
	y += i * (boxHeight + 1) + offset;
	if (y < 0 || y > mp.display.height()) {
		return;
	}
	mp.display.fillRect(1, y + 1, mp.display.width() - 2, boxHeight - (scale-1), TFT_DARKGREY);
	mp.display.setTextColor(TFT_WHITE);
	mp.display.setCursor(2, y + 2);
	mp.display.drawString(text, 3, y - 1);
}
int8_t menu(const char* title, String* items, uint8_t length) {
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	int16_t cursor = 0;
	uint8_t offset;
	uint8_t boxHeight;
	bool blinkState = 0;
	uint32_t blinkMillis = millis();
	if(mp.resolutionMode)
	{
		offset = menuYOffset;
		boxHeight = 7;
	}
	else
	{
		offset = 19;
		boxHeight = 15;
	}
	while (1) {
		mp.update();
		
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
		for (uint8_t i = 0; i < length; i++) {
			menuDrawBox(items[i], i, cameraY_actual);
		}
		if(blinkState)
			menuDrawCursor(cursor, cameraY_actual);

		// last draw the top entry thing
		if(mp.resolutionMode)
		{
			mp.display.fillRect(0, 0, mp.display.width(), 6, TFT_DARKGREY);
			mp.display.setFreeFont(TT1);
			mp.display.setCursor(0,5);
			mp.display.drawFastHLine(0, 6, mp.display.width(), TFT_WHITE);
		}
		else
		{
			mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
			mp.display.setTextFont(2);
			mp.display.setCursor(1,-2);
			mp.display.drawFastHLine(0, 14, mp.display.width(), TFT_WHITE);
		}
		mp.display.print(title);

		if (mp.buttons.released(BTN_A)) {   //BUTTON CONFIRM
			mp.osc->note(75, 0.05);
			mp.osc->play();
			while(!mp.update());// Exit when pressed
			break;
		}
		if (mp.buttons.released(BTN_B))   //BUTTON BACK
			return -1;

		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
			blinkState = 1;
			blinkMillis = millis();
			mp.osc->note(75, 0.05);
			mp.osc->play();
			mp.leds[3] = CRGB::Blue;
			mp.leds[4] = CRGB::Blue;
			while(!mp.update());

			if (cursor == 0) {
				cursor = length - 1;
				if (length > 6) {
					cameraY = -(cursor - 5) * 8;
				}
			}
			else {
				cursor--;
				if (cursor > 0 && (cursor * 8 + cameraY + offset) < 14) {
					cameraY += 8;
				}
			}
		}

		if (mp.buttons.released(BTN_DOWN)) { //BUTTON DOWN
			blinkState = 1;
			blinkMillis = millis();
			mp.osc->note(75, 0.05);
			mp.osc->play();
			mp.leds[0] = CRGB::Blue;
			mp.leds[7] = CRGB::Blue;
			while(!mp.update());


			cursor++;
			if ((cursor * 8 + cameraY + offset) > 54) {
				cameraY -= 8;
			}
			if (cursor >= length) {
				cursor = 0;
				cameraY = 0;

			}

		}
	}
	return cursor;

}
void menuDrawCursor(uint8_t i, int32_t y) {
	uint8_t offset;
	uint8_t boxHeight;
	if(mp.resolutionMode)
	{
		offset = menuYOffset;
		boxHeight = 7;
	}
	else
	{
		offset = 17;
		boxHeight = 15;
	}
	y += i * (boxHeight + 1) + offset;
	mp.display.drawRect(0, y, mp.display.width(), boxHeight + 2, TFT_RED);
}
int16_t audioPlayerMenu(const char* title, String* items, uint16_t length, uint16_t index) {
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	String Name;
	uint8_t scale = 2;
	uint8_t offset = 17;
	uint8_t boxHeight = 15;
	uint16_t start = 0;
	int16_t cursor = index;
	bool blinkState = 0;
	uint32_t blinkMillis = millis();
	if (length > 6) {
		cameraY = -cursor * (boxHeight + 1) - 1;
	}
	while (1) {
		if(!mp.SDinsertedFlag)
			return -1;
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
		for (uint8_t i = 0; i < length; i++) {
			Name = items[i];
			while (Name.indexOf("/", start) != -1)
				start = Name.indexOf("/", start) + 1;
			Name = Name.substring(start, Name.indexOf("."));
			start = 0;
			menuDrawBox(Name, i, cameraY_actual);
		}
		if(blinkState)
			menuDrawCursor(cursor, cameraY_actual);

		mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
		mp.display.setTextFont(2);
		mp.display.setCursor(0,-2);
		mp.display.drawFastHLine(0, 14, mp.display.width(), TFT_WHITE);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print(title);

		if (mp.buttons.released(BTN_A)) {   //BUTTON CONFIRM
			while(!mp.update());
			break;
		}

		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
			blinkState = 1;
			blinkMillis = millis();
			while(!mp.update());
			if (cursor == 0) {
				cursor = length - 1;
				if (length > 6) {
					cameraY = -(cursor - 5) * (boxHeight + 1) - 1;
				}
			}
			else {
				if (cursor > 0 && (cursor * (boxHeight + 1) + cameraY + offset) < 20) {
					cameraY += (boxHeight + 1);
				}
				cursor--;
			}

		}

		if (mp.buttons.released(BTN_DOWN)) { //BUTTON DOWN
			blinkState = 1;
			blinkMillis = millis();
			while(!mp.update());
			cursor++;
			if ((cursor * (boxHeight + 1) + cameraY + offset) > 54*scale) {
				cameraY -= (boxHeight + 1);
			}
			if (cursor >= length) {
				cursor = 0;
				cameraY = 0;

			}

		}
		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			while(!mp.update());
			return -1;
		}
		mp.update();
	}
	return cursor;

}
void listAudio(const char * dirname, uint8_t levels) {
	audioCount = 0;
	while(!SD.begin(5, SPI, 8000000))
		Serial.println(F("SD ERROR"));
	File root = SD.open(dirname);
	if (!root) {
		Serial.println(F("Failed to open directory"));
		return;
	}
	if (!root.isDirectory()) {
		Serial.println(F("Not a directory"));
		return;
	}
	int counter = 1;
	uint8_t start = 0;
	File file = root.openNextFile();
	while (file) {
		String Name(file.name());
		if (Name.endsWith(F(".MP3")) || Name.endsWith(F(".mp3"))
		 || Name.endsWith(F(".wav")) || Name.endsWith(F(".WAV")))
		{
			audioFiles[counter - 1] = Name;
			audioCount++;
			counter++;
		}
		file = root.openNextFile();
	}
	root.close();
	file.close();
}
String readSerial() {
	uint8_t _timeout = 0;
	while (!Serial1.available() && _timeout < 300)
	{
		delay(20);
		_timeout++;
	}
	if (Serial1.available()) {
		return Serial1.readString();
	}
	return "";
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
void callNumber(String number) 
{
	String contact = mp.checkContact(number);
	mp.inCall = 1;
	mp.dataRefreshFlag = 0;
	char c;
	String localBuffer = "";
	String buffer = "";
	Serial1.print(F("ATD"));
	Serial1.print(number);
	Serial1.print(";\r\n");
	mp.display.setFreeFont(TT1);
	mp.display.setTextColor(TFT_BLACK);
	bool firstPass = 1;
	uint32_t timeOffset = 0;
	uint8_t scale;
	String temp;
	unsigned int tmp_time = 0;
	if(mp.resolutionMode)
	{
		scale = 1;
		mp.display.setFreeFont(TT1);
	}
	else
	{
		scale = 2;
		mp.display.setTextFont(2);
	}
	mp.display.setTextSize(1);
	digitalWrite(soundSwitchPin, 1);
	if(contact == "")
		mp.display.printCenter(number);
	else
		mp.display.printCenter(contact);
	uint8_t callState = 1;
	uint8_t callIdNumber = 0;
	
	while (1)
	{
		mp.display.fillScreen(TFT_WHITE);
		if (Serial1.available())
		{
			c = Serial1.read();
			buffer += c;
		}
		if(buffer.indexOf("CLCC:") != -1 && buffer.indexOf("\r", buffer.indexOf("CLCC:")) != -1)
		{
			if(callIdNumber < 1)
			{
				uint16_t helper = buffer.indexOf("+CLCC: ") + 7;
				callIdNumber = buffer.substring(helper, helper + 1).toInt();
				Serial.print("call id: ");
				Serial.println(callIdNumber);
			}
			localBuffer = buffer;
			buffer = "";
		}
		if(buffer.indexOf("\r") != -1)
			buffer = "";
		// Serial.println("---------------");
		// Serial.println(buffer);
		// delay(5);
		if(buffer.indexOf("OK", buffer.indexOf("AT+CLVL=")) != -1)
			buffer = "";
		if (localBuffer.indexOf(String("CLCC: " + String(callIdNumber))) != -1 || localBuffer.indexOf("AT+CLVL") != -1)
		{
			if(localBuffer.indexOf(String(String(callIdNumber) + ",0,0,0,0")) != -1 
			|| localBuffer.indexOf("AT+CLVL") != -1  )
			{
				callState = 2;
				
			}

			if(localBuffer.indexOf(String(String(callIdNumber) + ",0,3,0,0")) != -1 )
			{
				callState = 0;
				
			}

			if(localBuffer.indexOf(String(String(callIdNumber) + ",0,2,0,0")) != -1)
			{
				callState = 1;

			}

			if(localBuffer.indexOf(String(String(callIdNumber) + ",0,6,0,0")) != -1)
			{
				mp.display.fillScreen(TFT_WHITE);
				mp.display.setCursor(32, 9);
				if (timeOffset == 0)
				{
					mp.display.printCenter("00:00");
					tmp_time = 0;
				}
				else
				{
					temp = "";
					if ((int((millis() - timeOffset) / 1000) / 60) > 9)
						temp += (int((millis() - timeOffset) / 1000) / 60) ;
					else
					{
						temp += "0";
						temp += (int((millis() - timeOffset) / 1000) / 60);
					}
					temp += ":";
					if (int((millis() - timeOffset) / 1000) % 60 > 9)
						temp += (int((millis() - timeOffset) / 1000) % 60);
					else
					{
						temp += "0";
						temp += (int((millis() - timeOffset) / 1000) % 60);
					}
					mp.display.setCursor(9, 9);
					mp.display.printCenter(temp);
				}
				mp.display.drawBitmap(29*scale, 24*scale, call_icon, TFT_RED, scale);
				if(mp.resolutionMode)
					mp.display.setCursor(11, 20);
				else
					mp.display.setCursor(11, 28);
				if(contact == "")
					mp.display.printCenter(number);
				else
					mp.display.printCenter(contact);
				mp.display.fillRect(0, 51*scale, 80*scale, 13*scale, TFT_RED);
				if(mp.resolutionMode)
					mp.display.setCursor(2, 62);
				else
					mp.display.setCursor(2, 112);
				mp.display.print("Call ended");
				Serial.println("ENDED");
				while(!mp.update());

				mp.updateTimeRTC();
				if(mp.SDinsertedFlag)
					mp.addCall(number, mp.checkContact(number), mp.RTC.now().unixtime(), tmp_time, 1);

				delay(1000);
				break;
			}
		}


		else if (localBuffer.indexOf("CLCC:") == -1)
		{
			if (localBuffer.indexOf("ERROR") != -1)
			{

				mp.display.setCursor(3, 9);
				mp.display.printCenter("Couldn't dial number!");
				mp.display.drawBitmap(29*scale, 24*scale, call_icon, TFT_RED, scale);
				if(mp.resolutionMode)
					mp.display.setCursor(11, 20);
				else
					mp.display.setCursor(11, 28);
				if(contact == "")
					mp.display.printCenter(number);
				else
					mp.display.printCenter(contact);
				mp.display.fillRect(0, 51*scale, 80*scale, 13*scale, TFT_RED);
				if(mp.resolutionMode)
				{
					mp.display.setCursor(2, 57);
					mp.display.print("Invalid number or");
					mp.display.setCursor(2, 63);
					mp.display.print("SIM card missing!");
				}
				else
				{
					mp.display.setCursor(2, 100);
					mp.display.print("Invalid number or");
					mp.display.setCursor(2, 112);
					mp.display.print("SIM card missing!");
				}
				while (!mp.buttons.released(BTN_B))
					mp.update();
				break;
			}
			else
			{
				callState = 1;
			}
		}
		if (mp.buttons.pressed(BTN_FUN_RIGHT)) // hanging up
		{
			
			mp.display.fillScreen(TFT_WHITE);
			mp.display.setCursor(32, 9);
			if (timeOffset == 0)
			{
				tmp_time = 0;
				mp.display.printCenter("00:00");
			}
			else
			{
				temp = "";
				if ((int((millis() - timeOffset) / 1000) / 60) > 9)
					temp += (int((millis() - timeOffset) / 1000) / 60) ;
				else
				{
					temp += "0";
					temp += (int((millis() - timeOffset) / 1000) / 60);
				}
				temp += ":";
				if (int((millis() - timeOffset) / 1000) % 60 > 9)
					temp += (int((millis() - timeOffset) / 1000) % 60);
				else
				{
					temp += "0";
					temp += (int((millis() - timeOffset) / 1000) % 60);
				}
				mp.display.setCursor(9, 9);
				mp.display.printCenter(temp);
			}
			mp.display.drawBitmap(29*scale, 24*scale, call_icon, TFT_RED, scale);
			mp.display.setCursor(11, 28);
			if(contact == "")
				mp.display.printCenter(number);
			else
				mp.display.printCenter(contact);
			mp.display.fillRect(0, 51*scale, 80*scale, 13*scale, TFT_RED);
			mp.display.setCursor(2, 112);
			mp.display.print("Call ended");
			Serial.println("ENDED");
			while(!mp.update());
			mp.updateTimeRTC();
			Serial.println("B PRESSED");
			// Serial1.println("ATH");
			// mp.waitForOK();
			Serial1.println("ATH");
			buffer = mp.waitForOK();
			Serial.println(buffer);
			if(mp.SDinsertedFlag)
				mp.addCall(number, mp.checkContact(number), mp.RTC.now().unixtime(), tmp_time, 1);
			delay(1000);
			break;
		}
		if(mp.buttons.released(BTN_UP) && mp.callSpeakerVolume < 5)
		{
			mp.callSpeakerVolume++;
			if (mp.sim_module_version == 1)
				Serial1.printf("AT+CLVL=%d\r", mp.callSpeakerVolume*20);
			else if (mp.sim_module_version == 0)
				Serial1.printf("AT+CLVL=%d\r", mp.callSpeakerVolume);
			mp.display.fillRect(56, 111, 20, 15, TFT_RED);
			mp.display.setCursor(59, 109);
			mp.display.print(mp.callSpeakerVolume);
		}
		if(mp.buttons.released(BTN_DOWN) && mp.callSpeakerVolume > 0)
		{
			mp.callSpeakerVolume--;
			if (mp.sim_module_version == 1)
				Serial1.printf("AT+CLVL=%d\r", mp.callSpeakerVolume*20);
			else if (mp.sim_module_version == 0)
				Serial1.printf("AT+CLVL=%d\r", mp.callSpeakerVolume);
			mp.display.fillRect(56, 111, 20, 15, TFT_RED);
			mp.display.setCursor(59, 109);
			mp.display.print(mp.callSpeakerVolume);
		}
		switch (callState)
		{
			case 0:
				mp.display.setCursor(25, 9);
				mp.display.printCenter("Ringing...");
				mp.display.drawBitmap(29*scale, 24*scale, call_icon, TFT_DARKGREY, scale);
				mp.display.setCursor(11, 28);
				if(contact == "")
					mp.display.printCenter(number);
				else
					mp.display.printCenter(contact);
				mp.display.fillRect(0, 51*scale, 80*scale, 13*scale, TFT_RED);
				mp.display.setCursor(5, 109);
				mp.display.print("Volume: ");
				mp.display.print(mp.callSpeakerVolume);
				mp.display.setCursor(100, 109);
				mp.display.print("Hang up");
				break;
			case 1:
				mp.display.setCursor(25, 9);
				mp.display.printCenter("Calling...");
				mp.display.drawBitmap(29*scale, 24*scale, call_icon, TFT_DARKGREY, scale);
				mp.display.setCursor(11, 28);
				if(contact == "")
					mp.display.printCenter(number);
				else
					mp.display.printCenter(contact);
				mp.display.fillRect(0, 51*scale, 80*scale, 13*scale, TFT_RED);
				mp.display.setCursor(5, 109);
				mp.display.print("Volume: ");
				mp.display.print(mp.callSpeakerVolume);
				mp.display.setCursor(100, 109);
				mp.display.print("Hang up");
				break;
			case 2:
				if (firstPass == 1)
				{
					timeOffset = millis();
					firstPass = 0;
				}

				String temp = "";
				if ((int((millis() - timeOffset) / 1000) / 60) > 9)
					temp += (int((millis() - timeOffset) / 1000) / 60) ;
				else
				{
					temp += "0";
					temp += (int((millis() - timeOffset) / 1000) / 60);
				}
				temp += ":";
				if (int((millis() - timeOffset) / 1000) % 60 > 9)
					temp += (int((millis() - timeOffset) / 1000) % 60);
				else
				{
					temp += "0";
					temp += (int((millis() - timeOffset) / 1000) % 60);
				}
				mp.display.setCursor(9, 9);
				mp.display.printCenter(temp);
				mp.display.drawBitmap(29*scale, 24*scale, call_icon, TFT_GREEN, scale);
				mp.display.setCursor(11, 28);
				if(contact == "")
					mp.display.printCenter(number);
				else
					mp.display.printCenter(contact);
				mp.display.fillRect(0, 51*scale, 80*scale, 13*scale, TFT_RED);
				mp.display.setCursor(5, 109);
				mp.display.print("Volume: ");
				mp.display.print(mp.callSpeakerVolume);
				mp.display.setCursor(100, 109);
				mp.display.print("Hang up");
				break;
		}
		for(int i = 0; i < 12;i++)
		{
			
			if(mp.buttons.released(i))
			{
				if(i == 9)
					Serial1.println("AT+VTS=*");
				else if(i == 10)
					Serial1.println("AT+VTS=0");
				else if(i == 11)
					Serial1.println("AT+VTS=#");
				else
				{
					Serial1.print("AT+VTS=");
					Serial1.println(i+1);
				}
				break;
			}
		}
		tmp_time = int((millis() - timeOffset) / 1000);
		mp.update();
	}
	if(mp.SDinsertedFlag)
		mp.saveSettings();
	digitalWrite(soundSwitchPin, 0);
	mp.inCall = 0;
}
bool startupWizard()
{
	int16_t oldSleepTimeActual = mp.sleepTimeActual;
	int16_t oldSleepTime = mp.sleepTime;
	mp.sleepTime = 0;
	mp.sleepTimeActual = 0;
	mp.homePopupEnable(0);
	mp.shutdownPopupEnable(0);
	mp.display.fillScreen(TFT_WHITE);
	uint32_t tMillis = millis();
	bool goOut = 1;
	// Connect charger
	while(digitalRead(CHRG_INT))
	{
		mp.display.fillScreen(TFT_WHITE);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(4,4);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.printCenter("Startup wizard");
		mp.display.setCursor(0, mp.display.height()/2 - 14);
		mp.display.printCenter("Connect your charger");
		mp.display.drawBitmap(72, 74, batteryChargingIcon, TFT_BLACK, 3);
		Serial.println(mp.batteryVoltage);
		mp.update();
	}
	mp.display.setTextColor(TFT_BLACK);
	mp.display.setTextSize(1);
	mp.display.setTextFont(2);
	mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
	mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
	mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
	mp.display.setCursor(47, 55);
	mp.display.printCenter("Charger connected!");
	tMillis = millis();
	while(millis() < tMillis + 1500)
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
	mp.display.setTextColor(TFT_BLACK);
	mp.display.setTextSize(1);
	mp.display.setTextFont(2);
	mp.display.drawRect(4, 25, 154, 78, TFT_BLACK);
	mp.display.drawRect(3, 24, 156, 80, TFT_BLACK);
	mp.display.fillRect(5, 26, 152, 76, TFT_WHITE);
	mp.display.setCursor(47, 30);
	mp.display.printCenter("Charge the phone");
	mp.display.setCursor(47, 48);
	mp.display.printCenter("for at least an hour.");
	mp.display.setCursor(47, 74);
	mp.display.printCenter("Press A to continue");
	tMillis = millis();
	while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
		mp.update();
	while(!mp.update());
 
	mp.shutdownPopupEnable(0);

	// Buttons testing
	bool buttonsArray[18] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	bool allPressed = 1;
	while(1)
	{
		mp.display.fillScreen(TFT_WHITE);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(4,4);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.printCenter("Let's test the buttons");
		mp.display.setCursor(3, 97);
		mp.display.print("Press every button");
		mp.display.setCursor(3, 112);
		mp.display.print("to continue");
		allPressed = 1;
		for(int i = 0; i < 18; i++)
		{
			if(buttonsArray[i])
				mp.display.fillCircle(13 + 26*(i%6), 33 + int(i/6)*22, 8, TFT_GREEN);
			else
			{
				allPressed = 0;
				switch (i)
				{
				case 0:
					if(mp.buttons.pressed(BTN_FUN_LEFT))
						buttonsArray[i] = 1;
					break;
				case 1:
					if(mp.buttons.pressed(14))
						buttonsArray[i] = 1;
					break;
				case 2:
					if(mp.buttons.pressed(13))
						buttonsArray[i] = 1;
					break;
				case 3:
					if(mp.buttons.pressed(BTN_FUN_RIGHT))
						buttonsArray[i] = 1;
					break;
				case 4:
					if(mp.buttons.pressed(BTN_A))
						buttonsArray[i] = 1;
					break;
				case 5:
					if(mp.buttons.pressed(BTN_B))
						buttonsArray[i] = 1;
					break;
				case 6:
					if(mp.buttons.pressed(BTN_1))
						buttonsArray[i] = 1;
					break;
				case 7:
					if(mp.buttons.pressed(BTN_2))
						buttonsArray[i] = 1;
					break;
				case 8:
					if(mp.buttons.pressed(BTN_3))
						buttonsArray[i] = 1;
					break;
				case 9:
					if(mp.buttons.pressed(BTN_4))
						buttonsArray[i] = 1;
					break;
				case 10:
					if(mp.buttons.pressed(BTN_5))
						buttonsArray[i] = 1;
					break;
				case 11:
					if(mp.buttons.pressed(BTN_6))
						buttonsArray[i] = 1;
					break;
				case 12:
					if(mp.buttons.pressed(BTN_7))
						buttonsArray[i] = 1;
					break;
				case 13:
					if(mp.buttons.pressed(BTN_8))
						buttonsArray[i] = 1;
					break;
				case 14:
					if(mp.buttons.pressed(BTN_9))
						buttonsArray[i] = 1;
					break;
				case 15:
					if(mp.buttons.pressed(BTN_ASTERISK))
						buttonsArray[i] = 1;
					break;
				case 16:
					if(mp.buttons.pressed(BTN_0))
						buttonsArray[i] = 1;
					break;
				case 17:
					if(mp.buttons.pressed(BTN_HASHTAG))
						buttonsArray[i] = 1;
					break;
				}
			}
		}
		mp.display.setCursor(12, 25);
		mp.display.drawBitmap(59, 27, homeButton);
		mp.display.drawBitmap(33, 27, powerButton);
		mp.display.print("-            -   A   B");
		mp.display.setCursor(10, 47);
		mp.display.print("1   2   3   4   5   6");
		mp.display.setCursor(10, 69);
		mp.display.print("7   8   9   *   0   #");
		mp.update();

		if(allPressed)
		{
			mp.display.setTextColor(TFT_BLACK);
			mp.display.setTextSize(1);
			mp.display.setTextFont(2);
			mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
			mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
			mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
			mp.display.setCursor(47, 55);
			mp.display.printCenter("Buttons OK!");
			uint32_t tempMillis = millis();
			while(millis() < tempMillis + 1500)
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
			break;
		}
	}

	// Joystick testing
	bool tested[8] = {0,0,0,0,0,0,0,0};
	bool allTested = 1;
	while(1)
	{
		mp.display.fillScreen(TFT_WHITE);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(4,4);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.printCenter("Let's test the joystick");
		mp.display.setCursor(3, 97);
		mp.display.print("Test every direction");
		mp.display.setCursor(3, 112);
		mp.display.print("to continue");
		mp.display.drawBitmap(74, 24, joystickArrowUp);
		mp.display.drawBitmap(98, 30, joystickArrowUpRight);
		mp.display.drawBitmap(100, 54, joystickArrowRight);
		mp.display.drawBitmap(98, 73, joystickArrowDownRight);
		mp.display.drawBitmap(74, 76, joystickArrowDown);
		mp.display.drawBitmap(45, 73, joystickArrowDownLeft);
		mp.display.drawBitmap(40, 54, joystickArrowLeft);
		mp.display.drawBitmap(45, 30, joystickArrowUpLeft);
		allTested = 1;
		for(uint8_t i = 0; i < 8; i++)
		{
			if(tested[i])
			{
				switch (i)
				{
					case 0:
						mp.display.drawBitmap(74, 24, joystickArrowUp, TFT_GREEN);
						break;
					case 1:
						mp.display.drawBitmap(98, 30, joystickArrowUpRight, TFT_GREEN);
						break;
					case 2:
						mp.display.drawBitmap(100, 54, joystickArrowRight, TFT_GREEN);
						break;
					case 3:
						mp.display.drawBitmap(98, 73, joystickArrowDownRight, TFT_GREEN);
						break;
					case 4:
						mp.display.drawBitmap(74, 76, joystickArrowDown, TFT_GREEN);
						break;
					case 5:
						mp.display.drawBitmap(45, 73, joystickArrowDownLeft, TFT_GREEN);
						break;
					case 6:
						mp.display.drawBitmap(40, 54, joystickArrowLeft, TFT_GREEN);
						break;
					case 7:
						mp.display.drawBitmap(45, 30, joystickArrowUpLeft, TFT_GREEN);
						break;

				}
			}
			else
			{
				allTested = 0;
				switch (i)
				{
					case 0:
						if(mp.buttons.getJoystickY() < 100 && mp.buttons.getJoystickX() < 750 && mp.buttons.getJoystickX() > 350)
							tested[i] = 1;
						break;
					case 1:
						if(mp.buttons.getJoystickY() < 200 && mp.buttons.getJoystickX() < 200)
							tested[i] = 1;
						break;
					case 2:
						if(mp.buttons.getJoystickY() < 750 && mp.buttons.getJoystickY() > 350 && mp.buttons.getJoystickX() < 100)
							tested[i] = 1;
						break;
					case 3:
						if(mp.buttons.getJoystickY() > 900 && mp.buttons.getJoystickX() < 200)
							tested[i] = 1;
						break;
					case 4:
						if(mp.buttons.getJoystickY() > 1000 && mp.buttons.getJoystickX() < 750 && mp.buttons.getJoystickX() > 350)
							tested[i] = 1;
						break;
					case 5:
						if(mp.buttons.getJoystickY() > 900 && mp.buttons.getJoystickX() > 900)
							tested[i] = 1;
						break;
					case 6:
						if(mp.buttons.getJoystickY() < 750 && mp.buttons.getJoystickY() > 350 && mp.buttons.getJoystickX() > 1000)
							tested[i] = 1;
						break;
					case 7:
						if(mp.buttons.getJoystickY() < 200 && mp.buttons.getJoystickX() > 900)
							tested[i] = 1;
						break;

				}
			}
		}
		if(allTested)
		{
			mp.display.setTextColor(TFT_BLACK);
			mp.display.setTextSize(1);
			mp.display.setTextFont(2);
			mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
			mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
			mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
			mp.display.setCursor(47, 55);
			mp.display.printCenter("Joystick OK!");
			uint32_t tempMillis = millis();
			while(millis() < tempMillis + 1500)
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
			break;
		}
		mp.update();
	}

	

	//Speaker testing
	bool state = 0;
	bool playing = 0;
	uint8_t tempNotification = mp.notification;
	bool blinkState = 0;
	uint32_t blinkMillis = millis();
	uint8_t cursor = 0;
	uint32_t callMillis = millis();
	mp.ringVolume = 10;
	goOut = 1;
	while (goOut)
	{
		mp.display.fillScreen(TFT_WHITE);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(4,4);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.printCenter("Speaker testing");
		mp.display.setCursor(0,mp.display.height()/2 - 14);
		mp.display.printCenter("Listen to the speaker");
		uint32_t speakerMillis = millis();
		callMillis = millis();
		state = 1;
		while(millis() - speakerMillis < 2000)
		{
			if(millis() - callMillis >= 1000)
			{
				state = 1;
				callMillis = millis();
			}
			if(state)
			{
				mp.playNotificationSound(4);
				state = 0;
			}
			mp.update();
		}
		while(1)
		{
			mp.display.fillScreen(TFT_WHITE);
			mp.display.setTextFont(2);
			mp.display.setTextSize(1);
			mp.display.setTextColor(TFT_BLACK);
			mp.display.setCursor(4, 110);
			mp.display.print("Press A to confirm");

			if(millis() - blinkMillis >= 350)
			{
				blinkMillis = millis();
				blinkState = !blinkState;
			}
			mp.display.setCursor(0,6);
			mp.display.printCenter("Did you hear");
			mp.display.setCursor(0,22);
			mp.display.printCenter("the speaker");

			mp.display.setCursor(4, 110);
			mp.display.print("Press A to confirm");
			switch (cursor)
			{
				case 0:
					mp.display.drawRect(28, 50, 35, 18, blinkState ? TFT_RED : TFT_WHITE);
					mp.display.drawRect(27, 49, 37, 20, blinkState ? TFT_RED : TFT_WHITE);
					break;
				case 1:
					mp.display.drawRect(98, 50, 33, 18, blinkState ? TFT_RED : TFT_WHITE);
					mp.display.drawRect(97, 49, 35, 20, blinkState ? TFT_RED : TFT_WHITE);
					break;
				case 2:
					mp.display.drawRect(59, 80, 42, 18, blinkState ? TFT_RED : TFT_WHITE);
					mp.display.drawRect(58, 79, 44, 20, blinkState ? TFT_RED : TFT_WHITE);
					break;
			}

			if(mp.buttons.released(BTN_LEFT) && cursor == 1)
			{
				blinkState = 1;
				blinkMillis = millis();
				cursor = 0;
			}
			if(mp.buttons.released(BTN_RIGHT) && cursor == 0)
			{
				blinkState = 1;
				blinkMillis = millis();
				cursor = 1;
			}
			if(mp.buttons.released(BTN_DOWN) && cursor < 2)
			{
				blinkState = 1;
				blinkMillis = millis();
				cursor = 2;
			}
			if(mp.buttons.released(BTN_UP) && cursor == 2)
			{
				blinkState = 1;
				blinkMillis = millis();
				cursor = 0;
			}
			mp.display.setCursor(0,50);
			mp.display.printCenter("Yes        No");
			mp.display.setCursor(0,80);
			mp.display.printCenter("Retry");
			if(mp.buttons.released(BTN_A))
			{
				while(!mp.update());
				if(cursor == 1)
				{
					mp.display.fillScreen(TFT_WHITE);
					mp.display.setTextColor(TFT_BLACK);
					mp.display.setTextSize(1);
					mp.display.setTextFont(2);
					mp.display.setCursor(47, 5);
					mp.display.printCenter("Don't worry!");
					mp.display.setCursor(47, 30);
					mp.display.printCenter("Check soldering joints");
					mp.display.setCursor(47, 48);
					mp.display.printCenter("and contact us via");
					mp.display.setCursor(47, 66);
					mp.display.printCenter("circuitmess.com/contact");
					mp.display.setCursor(47, 84);
					mp.display.printCenter("for more assistance.");
					mp.display.setCursor(4, 112);
					mp.display.print("Press A to continue");
					while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
						mp.update();
					goOut = 0;
					break;
				}
				else
				{
					if(cursor == 0)
						goOut = 0;
					break;
				}
			}
			mp.update();
		}
	}
	mp.notification = tempNotification;
	while(!mp.update());


	//SIM module testing
	goOut = 1;
	bool response = 0;
	uint32_t tempMillis = millis();
		String reply = "";
	while(goOut)
	{
		mp.display.fillScreen(TFT_WHITE);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(4,4);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.printCenter("Network module test");
		mp.display.setCursor(0,mp.display.height()/2 - 22);
		mp.display.printCenter("SIM functionality test");
		mp.display.setCursor(0,mp.display.height()/2 - 2);
		mp.display.printCenter("Performing test 1/3");
		while(!mp.update());
		tempMillis = millis();
		reply = "";
		response = 0;
		while(millis() < tempMillis + 1500)
		{
			Serial1.println("AT");

			reply = Serial1.readString();
			Serial.println(reply);
			if(reply.indexOf("OK") != -1)
			{
				response = 1;
				break;
			}
		}
		if(response)
		{
			mp.display.setTextColor(TFT_BLACK);
			mp.display.setTextSize(1);
			mp.display.setTextFont(2);
			mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
			mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
			mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
			mp.display.setCursor(47, 55);
			mp.display.printCenter("AT OK!");
			tempMillis = millis();
			while(millis() < tempMillis + 1000)
				mp.update();
			break;
		}
		else
		{
			Serial1.begin(9600, SERIAL_8N1, 17, 16);
			while(millis() < tempMillis + 1500)
			{
				Serial1.println("AT");
				if(reply.indexOf("OK") != -1)
				{
					response = 1;
					break;
				}
				reply = Serial1.readString();
			}
			if(response)
			{
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextSize(1);
				mp.display.setTextFont(2);
				mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
				mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
				mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
				mp.display.setCursor(47, 55);
				mp.display.printCenter("AT OK!");
				tempMillis = millis();
				while(millis() < tempMillis + 1000)
					mp.update();
				break;
			}
			else
			{
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextSize(1);
				mp.display.setTextFont(2);
				mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
				mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
				mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
				mp.display.setCursor(47, 48);
				mp.display.printCenter("Network module");
				mp.display.setCursor(47, 63);
				mp.display.printCenter("not inserted!");
				tempMillis = millis();
				while(millis() < tempMillis + 1000)
					mp.update();
				tempMillis = millis();
				bool blinkState = 0;
				uint32_t blinkMillis = millis();
				uint8_t cursor = 0;
				while(1)
				{
					mp.display.fillScreen(TFT_WHITE);
					mp.display.setTextFont(2);
					mp.display.setTextSize(1);
					mp.display.setTextColor(TFT_BLACK);
					mp.display.setCursor(4, 110);
					mp.display.print("Press A to confirm");

					if(millis() - blinkMillis >= 350)
					{
						blinkMillis = millis();
						blinkState = !blinkState;
					}
					mp.display.setCursor(0,6);
					mp.display.printCenter("Reinsert the");
					mp.display.setCursor(0,22);
					mp.display.printCenter("network module");

					mp.display.setCursor(4, 110);
					mp.display.print("Press A to confirm");
					switch (cursor)
					{
						case 0:
							mp.display.drawRect(28, 50, 42, 18, blinkState ? TFT_RED : TFT_WHITE);
							mp.display.drawRect(27, 49, 44, 20, blinkState ? TFT_RED : TFT_WHITE);
							break;
						case 1:
							mp.display.drawRect(98, 50, 33, 18, blinkState ? TFT_RED : TFT_WHITE);
							mp.display.drawRect(97, 49, 35, 20, blinkState ? TFT_RED : TFT_WHITE);
							break;
					}

					if(mp.buttons.released(BTN_LEFT) && cursor == 1)
					{
						blinkState = 1;
						blinkMillis = millis();
						cursor = 0;
					}
					if(mp.buttons.released(BTN_RIGHT) && cursor == 0)
					{
						blinkState = 1;
						blinkMillis = millis();
						cursor = 1;
					}
					mp.display.setCursor(0,50);
					mp.display.printCenter("Retry      Skip");
					if(mp.buttons.released(BTN_A))
					{
						while(!mp.update());
						if(cursor == 1)
						{
							goOut = 0;
							break;
						}
						else
							break;
					}
					mp.update();
				}
			}
		}
	}
	if(response)
	{
		mp.sim_module_version = 1;
		while(1)
		{
			mp.display.fillScreen(TFT_WHITE);
			mp.display.setTextFont(2);
			mp.display.setTextSize(1);
			mp.display.setCursor(4,4);
			mp.display.setTextColor(TFT_BLACK);
			mp.display.printCenter("Network module test");
			mp.display.setCursor(0,mp.display.height()/2 - 22);
			mp.display.printCenter("SIM functionality test");
			mp.display.setCursor(0,mp.display.height()/2 - 2);
			mp.display.printCenter("Performing test 2/3");
			while(!mp.update());
			tempMillis = millis();
			response = 0;
			reply = "";
			while(millis() < tempMillis + 1500)
			{
				Serial1.println("AT+CSQ");

				reply = Serial1.readString();
				Serial.println(reply);
				if(reply.indexOf("+CSQ:") != -1)
				{
					response = 1;
					break;
				}
			}
			if(response)
			{
				uint16_t foo = reply.indexOf(" ", reply.indexOf("+CSQ:")) + 1;
				if(reply.substring(foo, reply.indexOf(",", foo)).toInt() == 99)
					response = 0;
			}
			if(response)
			{
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextSize(1);
				mp.display.setTextFont(2);
				mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
				mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
				mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
				mp.display.setCursor(47, 55);
				mp.display.printCenter("Signal OK!");
				tempMillis = millis();
				while(millis() < tempMillis + 1000)
					mp.update();
				break;
			}
			else
			{
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextSize(1);
				mp.display.setTextFont(2);
				mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
				mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
				mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
				mp.display.setCursor(47, 55);
				mp.display.printCenter("Signal error!");
				tempMillis = millis();
				while(millis() < tempMillis + 1000)
					mp.update();
				bool blinkState = 0;
				uint32_t blinkMillis = millis();
				bool cursor = 0;
				while(!mp.buttons.released(BTN_A))
				{
					if(millis() - blinkMillis >= 350)
					{
						blinkMillis = millis();
						blinkState = !blinkState;
					}
					mp.display.fillScreen(TFT_WHITE);
					mp.display.setCursor(0, 32);
					mp.display.printCenter("Check the antenna");
					mp.display.setCursor(0,70);
					mp.display.printCenter("Retry      Skip");
					mp.display.setCursor(4, 110);
					mp.display.print("Press A to confirm");
					if(cursor)
					{
						mp.display.drawRect(98, 70, 33, 18, blinkState ? TFT_RED : TFT_WHITE);
						mp.display.drawRect(97, 69, 35, 20, blinkState ? TFT_RED : TFT_WHITE);
					}
					else
					{
						mp.display.drawRect(28, 70, 42, 18, blinkState ? TFT_RED : TFT_WHITE);
						mp.display.drawRect(27, 69, 44, 20, blinkState ? TFT_RED : TFT_WHITE);
					}

					if(mp.buttons.released(BTN_LEFT) && cursor)
					{
						while(!mp.update());
						blinkState = 1;
						blinkMillis = millis();
						cursor = 0;
					}
					if(mp.buttons.released(BTN_RIGHT) && !cursor)
					{
						while(!mp.update());
						blinkState = 1;
						blinkMillis = millis();
						cursor = 1;
					}
					mp.update();
				}
				if(cursor)
					break;
				else
				{
					mp.display.fillScreen(TFT_WHITE);
					mp.display.setCursor(0,mp.display.height()/2 - 22);
					mp.display.printCenter("Refreshing signal data");
					mp.display.setCursor(0,mp.display.height()/2 - 2);
					mp.display.printCenter("Please wait...");
					tempMillis = millis();
					while(millis() < tempMillis + 4000)
						mp.update();
				}

			}
		}
		while(1)
		{
			mp.display.fillScreen(TFT_WHITE);
			mp.display.setTextFont(2);
			mp.display.setTextSize(1);
			mp.display.setCursor(4,4);
			mp.display.setTextColor(TFT_BLACK);
			mp.display.printCenter("Network module test");
			mp.display.setCursor(0,mp.display.height()/2 - 22);
			mp.display.printCenter("SIM functionality test");
			mp.display.setCursor(0,mp.display.height()/2 - 2);
			mp.display.printCenter("Performing test 3/3");
			while(!mp.update());
			mp.checkSim();
			if(mp.simInserted)
			{
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextSize(1);
				mp.display.setTextFont(2);
				mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
				mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
				mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
				mp.display.setCursor(47, 55);
				mp.display.printCenter("SIM card OK!");
				tempMillis = millis();
				while(millis() < tempMillis + 1000)
					mp.update();
				bool blinkState = 0;
				uint32_t blinkMillis = millis();
				bool cursor = 0;
				while(!mp.buttons.released(BTN_A))
				{
					if(millis() - blinkMillis >= 350)
					{
						blinkMillis = millis();
						blinkState = !blinkState;
					}
					mp.display.fillScreen(TFT_WHITE);
					mp.display.setCursor(0, 22);
					mp.display.printCenter("Make a test call?");
					mp.display.setCursor(0,70);
					mp.display.printCenter("Yes        No");
					mp.display.setCursor(4, 110);
					mp.display.print("Press A to confirm");
					if(cursor)
					{
						mp.display.drawRect(98, 70, 33, 18, blinkState ? TFT_RED : TFT_WHITE);
						mp.display.drawRect(97, 69, 35, 20, blinkState ? TFT_RED : TFT_WHITE);
					}
					else
					{
						mp.display.drawRect(28, 70, 42, 18, blinkState ? TFT_RED : TFT_WHITE);
						mp.display.drawRect(27, 69, 44, 20, blinkState ? TFT_RED : TFT_WHITE);
					}

					if(mp.buttons.released(BTN_LEFT) && cursor)
					{
						while(!mp.update());
						blinkState = 1;
						blinkMillis = millis();
						cursor = 0;
					}
					if(mp.buttons.released(BTN_RIGHT) && !cursor)
					{
						while(!mp.update());
						blinkState = 1;
						blinkMillis = millis();
						cursor = 1;
					}
					mp.update();
				}
				if(!cursor)
				{
					while(!mp.update());
					phoneApp();
				}
				break;
			}
			else
			{
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextSize(1);
				mp.display.setTextFont(2);
				mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
				mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
				mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
				mp.display.setCursor(47, 55);
				mp.display.printCenter("SIM card not found!");
				tempMillis = millis();
				while(millis() < tempMillis + 1000)
					mp.update();
				tempMillis = millis();
				while(millis() < tempMillis + 1000)
					mp.update();
				bool blinkState = 0;
				uint32_t blinkMillis = millis();
				bool cursor = 0;
				while(!mp.buttons.released(BTN_A))
				{
					if(millis() - blinkMillis >= 350)
					{
						blinkMillis = millis();
						blinkState = !blinkState;
					}
					mp.display.fillScreen(TFT_WHITE);
					mp.display.setCursor(0, 16);
					mp.display.printCenter("Reinsert SIM card");
					mp.display.setCursor(0, 32);
					mp.display.printCenter("and press retry");
					mp.display.setCursor(0,70);
					mp.display.printCenter("Retry      Skip");
					mp.display.setCursor(4, 110);
					mp.display.print("Press A to confirm"); // prompt za manual turn on
					if(cursor)
					{
						mp.display.drawRect(98, 70, 33, 18, blinkState ? TFT_RED : TFT_WHITE);
						mp.display.drawRect(97, 69, 35, 20, blinkState ? TFT_RED : TFT_WHITE);
					}
					else
					{
						mp.display.drawRect(28, 70, 42, 18, blinkState ? TFT_RED : TFT_WHITE);
						mp.display.drawRect(27, 69, 44, 20, blinkState ? TFT_RED : TFT_WHITE);
					}

					if(mp.buttons.released(BTN_LEFT) && cursor)
					{
						blinkState = 1;
						blinkMillis = millis();
						cursor = 0;
					}
					if(mp.buttons.released(BTN_RIGHT) && !cursor)
					{
						blinkState = 1;
						blinkMillis = millis();
						cursor = 1;
					}
					mp.update();
				}
				if(cursor)
					break;
				else
				{
					mp.display.setCursor(0,50);
					mp.display.fillScreen(TFT_WHITE);
					mp.display.printCenter("Reseting network module");
					while(!mp.update());
					while(Serial1.available())
						Serial1.read();
					Serial1.println("AT+CFUN=1,1");
					char buffer[300];
					bool found = 0;
					memset(buffer, 0, sizeof(buffer));
					Serial1.flush();
					uint32_t timer = millis();
					while(!found)
					{
						if(Serial1.available())
						{

							char test = (char)Serial1.read();
							strncat(buffer, &test, 1);
							Serial.println(buffer);
						}
						if(strstr(buffer, "RDY") != nullptr)
							found = 1;
						if((millis() - timer > 5000 && (mp.sim_module_version == 1 || mp.sim_module_version == 255)) ||
						(millis() - timer > 28000 && mp.sim_module_version == 0))
							break;
					}
					delay(3000);
					// mp.tft.setTextFont(2);
					// mp.tft.setTextSize(1);
					// mp.tft.setTextColor(TFT_BLACK);
					// mp.tft.fillRect(0, 0, 160, 128, TFT_WHITE);
					// mp.tft.setCursor(40, 37);
					// mp.tft.print("Turning off...");
					// mp.tft.setCursor(40, 60);
					// mp.tft.print("You need to ");
					// mp.tft.setCursor(20, 74);
					// mp.tft.print("power on manually");

					// delay(2500);
					// Serial.println("TURN OFF");
					// digitalWrite(OFF_PIN, 1);
				}

			}


			mp.update();
		}
	}

	//SD card testing
	while(1)
	{
		mp.display.fillScreen(TFT_WHITE);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(4,4);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.printCenter("Startup wizard");
		mp.display.setCursor(0,mp.display.height()/2 - 16);
		mp.display.printCenter("SD card test");
		while(!mp.update());
		delay(500);
		mp.SDinsertedFlag = 1;
		uint32_t tempMillis = millis();
		while (!SD.begin(5, SPI, 8000000))
		{
			Serial.println("SD ERROR");
			if(millis()-tempMillis > 5)
			{
				mp.SDinsertedFlag = 0;
				break;
			}
		}
		if(!mp.SDinsertedFlag)
		{
			SD.end();
			mp.display.setTextColor(TFT_BLACK);
			mp.display.setTextSize(1);
			mp.display.setTextFont(2);
			mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
			mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
			mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
			mp.display.setCursor(47, 55);
			mp.display.printCenter("SD card error!");
			tempMillis = millis();
			while(millis() < tempMillis + 1000)
				mp.update();
			bool blinkState = 0;
			uint32_t blinkMillis = millis();
			bool cursor = 0;
			while(!mp.buttons.released(BTN_A))
			{
				if(millis() - blinkMillis >= 350)
				{
					blinkMillis = millis();
					blinkState = !blinkState;
				}
				mp.display.fillScreen(TFT_WHITE);
				mp.display.setCursor(0, 32);
				mp.display.printCenter("Reinsert SD card");
				mp.display.setCursor(0,70);
				mp.display.printCenter("Retry      Skip");
				mp.display.setCursor(4, 110);
				mp.display.print("Press A to confirm");
				if(cursor)
				{
					mp.display.drawRect(98, 70, 33, 18, blinkState ? TFT_RED : TFT_WHITE);
					mp.display.drawRect(97, 69, 35, 20, blinkState ? TFT_RED : TFT_WHITE);
				}
				else
				{
					mp.display.drawRect(28, 70, 42, 18, blinkState ? TFT_RED : TFT_WHITE);
					mp.display.drawRect(27, 69, 44, 20, blinkState ? TFT_RED : TFT_WHITE);
				}

				if(mp.buttons.released(BTN_LEFT) && cursor)
				{
					blinkState = 1;
					blinkMillis = millis();
					cursor = 0;
				}
				if(mp.buttons.released(BTN_RIGHT) && !cursor)
				{
					blinkState = 1;
					blinkMillis = millis();
					cursor = 1;
				}
				mp.update();
			}
			while(!mp.update());
			if(cursor)
				break;
		}
		else
		{
			mp.display.setTextColor(TFT_BLACK);
			mp.display.setTextSize(1);
			mp.display.setTextFont(2);
			mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
			mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
			mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
			mp.display.setCursor(47, 55);
			mp.display.printCenter("SD card OK!");
			tempMillis = millis();
			while(millis() < tempMillis + 1000)
				mp.update();
			break;
		}
	}

	//Neopixel testing
	goOut = 1;
	while(goOut)
	{
		mp.display.fillScreen(TFT_WHITE);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(4,4);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.printCenter("Testing the LEDs");
		mp.display.setCursor(0,mp.display.height()/2 - 20);
		mp.display.printCenter("Take a look at the");
		mp.display.setCursor(0,mp.display.height()/2 - 2);
		mp.display.printCenter("RGB LEDs on the back");
		while(!mp.update());
		for (uint8_t i = 0; i < NUMPIXELS; i++) {
			for (uint8_t x = 0; x <= 128; x += 2) {
				mp.leds[i] = CRGB(x, 0, 0);
				delay(1);
				FastLED.show();
			}
		}
		for (uint8_t i = 0; i < NUMPIXELS; i++) {
			for (int16_t x = 128; x >= 0; x -= 2) {
				mp.leds[i] = CRGB(x, 0, 0);
				delay(1);
				FastLED.show();
			}
		}
		for (uint8_t i = 0; i < NUMPIXELS; i++) {
			for (uint8_t x = 0; x <= 128; x += 2) {
				mp.leds[i] = CRGB(0, x, 0);
				delay(1);
				FastLED.show();
			}
		}
		for (uint8_t i = 0; i < NUMPIXELS; i++) {
			for (int16_t x = 128; x >= 0; x -= 2) {
				mp.leds[i] = CRGB(0, x, 0);
				delay(1);
				FastLED.show();
			}
		}
		for (uint8_t i = 0; i < NUMPIXELS; i++) {
			for (uint8_t x = 0; x <= 128; x += 2) {
				mp.leds[i] = CRGB(0, 0, x);
				delay(1);
				FastLED.show();
			}
		}
		for (uint8_t i = 0; i < NUMPIXELS; i++) {
			for (int16_t x = 128; x >= 0; x -= 2) {
				mp.leds[i] = CRGB(0, 0, x);
				delay(1);
				FastLED.show();
			}
		}
		bool blinkState = 0;
		uint32_t blinkMillis = millis();
		uint8_t cursor = 0;
		while(1)
		{
			mp.display.fillScreen(TFT_WHITE);
			mp.display.setTextFont(2);
			mp.display.setTextSize(1);
			mp.display.setTextColor(TFT_BLACK);
			mp.display.setCursor(4, 110);
			mp.display.print("Press A to confirm");

			if(millis() - blinkMillis >= 350)
			{
				blinkMillis = millis();
				blinkState = !blinkState;
			}
			mp.display.setCursor(0,6);
			mp.display.printCenter("Did every LED");
			mp.display.setCursor(0,22);
			mp.display.printCenter("work correctly?");

			mp.display.setCursor(4, 110);
			mp.display.print("Press A to confirm");
			switch (cursor)
			{
				case 0:
					mp.display.drawRect(28, 50, 35, 18, blinkState ? TFT_RED : TFT_WHITE);
					mp.display.drawRect(27, 49, 37, 20, blinkState ? TFT_RED : TFT_WHITE);
					break;
				case 1:
					mp.display.drawRect(98, 50, 33, 18, blinkState ? TFT_RED : TFT_WHITE);
					mp.display.drawRect(97, 49, 35, 20, blinkState ? TFT_RED : TFT_WHITE);
					break;
				case 2:
					mp.display.drawRect(59, 80, 42, 18, blinkState ? TFT_RED : TFT_WHITE);
					mp.display.drawRect(58, 79, 44, 20, blinkState ? TFT_RED : TFT_WHITE);
					break;
			}

			if(mp.buttons.released(BTN_LEFT) && cursor == 1)
			{
				blinkState = 1;
				blinkMillis = millis();
				cursor = 0;
			}
			if(mp.buttons.released(BTN_RIGHT) && cursor == 0)
			{
				blinkState = 1;
				blinkMillis = millis();
				cursor = 1;
			}
			if(mp.buttons.released(BTN_DOWN) && cursor < 2)
			{
				blinkState = 1;
				blinkMillis = millis();
				cursor = 2;
			}
			if(mp.buttons.released(BTN_UP) && cursor == 2)
			{
				blinkState = 1;
				blinkMillis = millis();
				cursor = 0;
			}
			mp.display.setCursor(0,50);
			mp.display.printCenter("Yes        No");
			mp.display.setCursor(0,80);
			mp.display.printCenter("Retry");
			if(mp.buttons.released(BTN_A))
			{
				while(!mp.update());
				if(cursor == 1)
				{
					mp.display.fillScreen(TFT_WHITE);
					mp.display.setTextColor(TFT_BLACK);
					mp.display.setTextSize(1);
					mp.display.setTextFont(2);
					mp.display.setCursor(47, 5);
					mp.display.printCenter("Don't worry!");
					mp.display.setCursor(47, 30);
					mp.display.printCenter("Check soldering joints");
					mp.display.setCursor(47, 48);
					mp.display.printCenter("and contact us via");
					mp.display.setCursor(47, 66);
					mp.display.printCenter("circuitmess.com/contact");
					mp.display.setCursor(47, 84);
					mp.display.printCenter("for more assistance.");
					mp.display.setCursor(4, 112);
					mp.display.print("Press A to continue");
					while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
						mp.update();
					goOut = 0;
					break;
				}
				else
				{
					if(cursor == 0)
						goOut = 0;
					break;
				}
			}
			mp.update();
		}
	}
	while(!mp.update());
	timeMenu();
	// Wifi testing
	// if(!mp.wifi)
	// {
		// mp.wifi = 1;
	delay(50);
	WiFi.begin();
	delay(50);
	// }
	mp.display.fillScreen(TFT_WHITE);
	mp.display.setTextFont(2);
	mp.display.setTextSize(1);
	mp.display.setCursor(4,4);
	mp.display.setTextColor(TFT_BLACK);
	mp.display.printCenter("Wi-Fi testing");
	mp.display.setCursor(0,mp.display.height()/2 - 14);
	mp.display.printCenter("Searching networks");
	while(!mp.update());
	wifiConnect();
	mp.shutdownPopupEnable(1);
	EEPROM.writeBool(33, 0);
	EEPROM.commit();
  	mp.display.setTextColor(TFT_BLACK);
	mp.display.setTextSize(1);
	mp.display.setTextFont(2);
 	mp.display.fillScreen(TFT_WHITE);
	mp.display.drawRect(10, 45, 138, 38, TFT_BLACK);
	mp.display.drawRect(9, 44, 140, 40, TFT_BLACK);
	mp.display.fillRect(11, 46, 136, 36, TFT_WHITE);
	mp.display.setCursor(43, 55);
	mp.display.printCenter("Setup wizard done!");
	mp.update();
	tempMillis = millis();
	while(millis() < tempMillis + 2000)
	mp.update();
	mp.sleepTimeActual = oldSleepTimeActual;
	mp.sleepTime = oldSleepTime;
	mp.sleepTimer = millis();
 	return 1;
}
void controlTry() //for debug purposes
{
	int16_t oldSleepTimeActual = mp.sleepTimeActual;
	int16_t oldSleepTime = mp.sleepTime;
	int16_t y = 0;
	mp.sleepTime = 0;
	mp.sleepTimeActual = 0;
	mp.textInput("");
	mp.textPointer = 0;

	String content = "";
	String prevContent = "";
	unsigned long elapsedMillis = millis();
	bool blinkState = 1;
	uint8_t scale;
	char c = ' ';
	String outBuffer = "";
	uint32_t cursorY = 1;
	mp.dataRefreshFlag = 0;
	uint32_t blinkMillis = millis();
	bool cursor = 0;
	String buffer = "";
	while(!mp.buttons.released(BTN_A))
	{
		if(millis() - blinkMillis >= 350)
		{
			blinkMillis = millis();
			blinkState = !blinkState;
		}
		mp.display.fillScreen(TFT_DARKGREY);
		mp.display.setTextSize(1);
		mp.display.setTextFont(2);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.setCursor(0, 32);
		mp.display.printCenter("List operators?");
		mp.display.setCursor(0,70);
		mp.display.printCenter("Yes        No");
		mp.display.setCursor(4, 110);
		mp.display.print("Press A to confirm");
		if(cursor)
		{
			mp.display.drawRect(98, 70, 33, 18, blinkState ? TFT_RED : TFT_DARKGREY);
			mp.display.drawRect(97, 69, 35, 20, blinkState ? TFT_RED : TFT_DARKGREY);
		}
		else
		{
			mp.display.drawRect(28, 70, 42, 18, blinkState ? TFT_RED : TFT_DARKGREY);
			mp.display.drawRect(27, 69, 44, 20, blinkState ? TFT_RED : TFT_DARKGREY);
		}

		if(mp.buttons.released(BTN_LEFT) && cursor)
		{
			blinkState = 1;
			blinkMillis = millis();
			cursor = 0;
		}
		if(mp.buttons.released(BTN_RIGHT) && !cursor)
		{
			blinkState = 1;
			blinkMillis = millis();
			cursor = 1;
		}
		mp.update();
	}
	while(!mp.update());
	if(!cursor)
	{
		Serial1.println("AT+COPS=?");
		while(!Serial1.available());
		String temp = Serial1.readString();
		Serial.println(temp);
		if(temp.indexOf("ERROR") == -1)
		{
			mp.display.fillScreen(TFT_DARKGREY);
			mp.display.setTextColor(TFT_WHITE);
			mp.display.setTextFont(2);
			mp.display.setCursor(0,mp.display.height()/2-12);
			mp.display.printCenter("Loading operators");
			while(!mp.update());
			while(!Serial1.available());
			temp = Serial1.readString();
			temp = temp.substring(2, temp.indexOf("\r", 2));
			Serial.println(temp);
			mp.display.fillScreen(TFT_DARKGREY);
			mp.display.setTextColor(TFT_WHITE);
			mp.display.setTextFont(1);
			while(!mp.buttons.released(BTN_B))
			{
				mp.display.fillScreen(TFT_DARKGREY);
				mp.display.setCursor(1, cursorY);
				mp.display.setTextWrap(1);
				mp.display.print(temp);
				if (mp.buttons.repeat(BTN_DOWN, 3)) { //BUTTON DOWN
					Serial.println(mp.display.cursor_y);
					if (mp.display.cursor_y >= 110)
					{

						cursorY -= 4;
					}
				}

				if (mp.buttons.repeat(BTN_UP, 3)) { //BUTTON UP
					if (cursorY < 1)
					{
						cursorY += 4;
					}
				}
				mp.update();
			}
			while(!mp.update());
			mp.display.setTextFont(2);
		}
	}
	// mp.dataRefreshFlag = 1;
	while (1)//+CLCC: 1,1,4,0,0,"+385953866578",145,""
	{
		mp.display.fillScreen(TFT_DARKGREY);

		mp.display.setTextColor(TFT_WHITE);
        scale = 2;
        mp.display.setTextFont(1);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.setCursor(0,0);
		mp.display.print(buffer);
		if(mp.display.getCursorY() > 120)
			buffer = "";
		buffer.replace("\n\n", "");

		if (millis() - elapsedMillis >= multi_tap_threshold) //cursor blinking routine
		{
			elapsedMillis = millis();
			blinkState = !blinkState;
		}
		mp.display.setTextColor(TFT_WHITE);
		mp.display.setCursor(2, -1);
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

		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			mp.sleepTimeActual = oldSleepTimeActual;
			mp.sleepTime = oldSleepTime;
			mp.sleepTimer = millis();
			Serial.println("B pressed");
			while(!mp.update());
			break;
		}
		if (mp.buttons.released(BTN_A) && content != "") // SEND SMS
		{
			Serial1.println(content);
			content = "";
			prevContent = "";
			mp.textInput("");
			mp.textPointer = 0;
		}
		if(Serial1.available())
		{
			String input = Serial1.readString();
			Serial.println(input);
			if(input.indexOf("+CREG:") != -1)
				buffer+=input;
			
		}
		if(Serial.available())
		{
			Serial1.println(Serial.readString());
		}
		mp.update();
	}
}
void setup()
{
	Serial.begin(115200);	
	EEPROM.begin(256);
	mp.homePopupEnable(0);
	mp.shutdownPopupEnable(0);

	if(EEPROM.readBool(34))
	{
		EEPROM.writeBool(34, 0);
		EEPROM.commit();
		SD.begin(5, SPI, 8000000);
		mp.tft.init();
		mp.tft.invertDisplay(0);
		mp.tft.setRotation(3);
		mp.tft.setTextColor(TFT_WHITE);
		mp.tft.setTextSize(1);
		mp.tft.setTextFont(2);
		Serial.println(EEPROM.readString(35).c_str());
		Serial.println(EEPROM.readString(100).c_str());
		delay(1000);
		WiFi.begin(EEPROM.readString(35).c_str(), EEPROM.readString(100).c_str());
		delay(1000);
		Serial.print("Connected: ");
		Serial.println(WiFi.status() == WL_CONNECTED);
		Serial.println("FREE HEAP:");
		Serial.println(ESP.getFreeHeap());
		delay(5);
		if(!fetchUpdate())
		{
			mp.tft.fillRect(0,0,160,128,TFT_BLACK);
			mp.tft.setCursor(10,mp.tft.height()/2 - 26);
			mp.tft.print("Error while downloading");
			mp.tft.setCursor(22,mp.tft.height()/2 - 5);
			mp.tft.print("Reverting firmware!");
			ESP.restart();
		}
		mp.tft.fillRect(0,0,160,128,TFT_BLACK);
		mp.tft.setCursor(20,mp.tft.height()/2 - 26);
		mp.tft.print("Installing firmware...");
		mp.tft.setCursor(32,mp.tft.height()/2 - 5);
		mp.tft.print("Don't turn off!");
		mp.updateFromFS("/.core/LOADER.BIN");
		ESP.restart();
	}
	mp.begin();
	// osc = new Oscillator();
	// mp.osc->setVolume(mp.oscillatorVolumeList[mp.volume]);
	// addOscillator(osc);
	Serial.print("Setup: ");
	Serial.println(EEPROM.readBool(33));
	if(EEPROM.readBool(33))
		startupWizard();
	mp.shutdownPopupEnable(1);
	// startupWizard();
	// controlTry();
	// settingsApp();
	mp.lockscreen();
	// pinMode(39, INPUT_PULLUP);
	// mp.buttons.activateInterrupt();
}
void loop()
{
	// mp.dataRefreshFlag = 1;
	// mp.display.fillScreen(TFT_WHITE);
	// mp.display.setTextColor(TFT_BLACK);
	// mp.display.setTextFont(2);
	// mp.display.setTextSize(1);
	// mp.display.setCursor(40, 40);
	// mp.display.printCenter((float)mp.batteryVoltage);
	// mp.display.setCursor(60,60);
	// mp.display.printCenter((float)mp.simVoltage);
	// mp.update();

	// startupWizard();
	// messagesApp();
	// phoneApp();
	// clockApp();

	// mp.update();
	// mp.display.fillScreen(TFT_BLACK);
	// mp.display.setCursor(20,0);
	// mp.display.printCenter("Buttons INT: ");
	// mp.display.setCursor(20,20);
	// mp.display.printCenter(digitalRead(BTN_INT));
	// mp.display.setCursor(20,40);
	// mp.display.printCenter("RTC INT");
	// mp.display.setCursor(20,60);
	// mp.display.printCenter(digitalRead(RTC_INT));
	// mp.display.setCursor(20,80);
	// mp.display.printCenter("   USB INT: ");
	// mp.display.setCursor(20,100);
	// mp.display.printCenter(digitalRead(39));

	
	mainMenu();
	// mediaApp();
	// phoneApp();
	// calculatorApp();
	// contactsApp();

	// mp.display.fillScreen(TFT_BLACK);
	// mp.display.setTextColor(TFT_WHITE);
	// mp.display.setTextSize(2);
	// mp.display.setCursor(10, 30);
	// mp.display.print(mp.buttons.released(BTN_2));
	// mp.display.setCursor(30, 30);
	// mp.display.print(mp.buttons.released(BTN_1));

	// for(int i = 0; i < 16; ++i) {
	// 	Serial.print(mp.buttons.states[i]);
	// 	Serial.print(" ");
	// }
	// Serial.println("");

	// mp.update();
}