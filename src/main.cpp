#include <Arduino.h>
#include "main.h"
#include "lockscreen.h"
#include "mainMenu.h"
#include "sprites.c"
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
String titles[10] PROGMEM = {
	"Messages",
	"Media",
	"Contacts",
	"Settings",
	"Phone",
	"Apps",
	"Clock",
	"Calculator",
	"Flashlight",
	"Calendar"
};
int textPointer = 0;
StaticJsonBuffer<capacity> jb;
uint8_t audioCount = 0;
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
	if(mp.resolutionMode)
		mp.display.drawString(text, 3, y + 2);
	else
		mp.display.drawString(text, 3, y);
}
int8_t menu(const char* title, String* items, uint8_t length) {
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	int16_t cursor = 0;
	uint8_t offset;
	uint8_t boxHeight;
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
		while (!mp.update());
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, 0);
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
		}

		for (uint8_t i = 0; i < length; i++) {
			menuDrawBox(items[i], i, cameraY_actual);
		}
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
			osc->note(75, 0.05);
			osc->play();
			while (!mp.update());// Exit when pressed
			break;
		}
		if (mp.buttons.released(BTN_B))   //BUTTON BACK
			return -1;

		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
			osc->note(75, 0.05);
			osc->play();
			mp.leds[3] = CRGB::Blue;
			mp.leds[4] = CRGB::Blue;
			while(!mp.update());
			mp.vibration(200);
			FastLED.clear();
			while (!mp.update());

			while (mp.buttons.kpd.pin_read(BTN_UP) == 0);
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
			osc->note(75, 0.05);
			osc->play();
			mp.leds[0] = CRGB::Blue;
			mp.leds[7] = CRGB::Blue;
			while (!mp.update());
			mp.vibration(200);
			FastLED.clear();
			while (!mp.update());

			while (mp.buttons.kpd.pin_read(BTN_DOWN) == 0);

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
	if (millis() % 500 <= 250) {
		return;
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
	if (length > 12) {
		cameraY = -cursor * (boxHeight + 1) - 1;
	}
	while (1) {
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, 0);
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
		}

		for (uint8_t i = 0; i < length; i++) {
			Name = items[i];
			while (Name.indexOf("/", start) != -1)
				start = Name.indexOf("/", start) + 1;
			Name = Name.substring(start, Name.indexOf("."));
			menuDrawBox(Name, i, cameraY_actual);
		}
		menuDrawCursor(cursor, cameraY_actual);

			mp.display.fillRect(0, 0, mp.display.width(), 14, TFT_DARKGREY);
			mp.display.setTextFont(2);
			mp.display.setCursor(0,-2);
			mp.display.drawFastHLine(0, 14, mp.display.width(), TFT_WHITE);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print(title);

		if (mp.buttons.released(BTN_A)) {   //BUTTON CONFIRM
			while (!mp.update());
			break;
		}

		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP

			while (!mp.update());
			if (cursor == 0) {
				cursor = length - 1;
				if (length > 6*scale) {
					cameraY = -(cursor - 5) * (boxHeight + 1) - 1;
				}
			}
			else {
				if (cursor > 0 && (cursor * (boxHeight + 1) - 1 + cameraY + offset) <= boxHeight) {
					cameraY += (boxHeight + 1);
				}
				cursor--;
			}

		}

		if (mp.buttons.released(BTN_DOWN)) { //BUTTON DOWN
			while (!mp.update());
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
			while (!mp.update());
			return -1;
		}
		mp.update();
	}
	return cursor;

}
void listAudio(const char * dirname, uint8_t levels) {
	audioCount = 0;
	while(!mp.SD.begin(5, SPI, 9000000))
		Serial.println(F("SD ERROR"));
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
	uint8_t start = 0;
	SDAudioFile file = root.openNextFile();
	while (file) {
		String Name(file.name());
		Serial.println(Name);
		if (Name.endsWith(F(".MP3")) || Name.endsWith(F(".mp3"))
		 || Name.endsWith(F(".wav")) || Name.endsWith(F(".WAV")))
		{
			Serial.print(counter);
			Serial.print(F(".   "));
			Serial.println(Name);
			audioFiles[counter - 1] = Name;
			Serial.println(Name);
			audioCount++;
			counter++;
		}
		file = root.openNextFile();
	}
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
void callNumber(String number) {
	mp.dataRefreshFlag = 0;

	String localBuffer = "";
	Serial1.print(F("ATD"));
	Serial1.print(number);
	Serial1.print(";\r\n");
	mp.display.setFreeFont(TT1);
	mp.display.setTextColor(TFT_BLACK);
	bool firstPass = 1;
	uint32_t timeOffset = 0;
	uint16_t textLength;
	uint8_t scale;
	String temp;
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

	mp.display.printCenter(number);
	textLength = mp.display.cursor_x - textLength;
	while (1)
	{
		mp.display.fillScreen(TFT_WHITE);
		if (Serial1.available())
			localBuffer = Serial1.readString();
		Serial.println(localBuffer);
		delay(5);
		if (localBuffer.indexOf("CLCC:") != -1)
		{
			if (localBuffer.indexOf(",0,0,0,0") != -1)
			{
				if (firstPass == 1)
				{
					timeOffset = millis();
					firstPass = 0;
				}


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
				Serial.println("CALL ACTIVE");
				mp.display.drawBitmap(29*scale, 24*scale, call_icon, TFT_GREEN, scale);
			}

			else if (localBuffer.indexOf(",0,3,") != -1)
			{
				mp.display.setCursor(25, 9);
				Serial.println("ringing");
				mp.display.printCenter("Ringing...");
				mp.display.drawBitmap(29*scale, 24*scale, call_icon, TFT_DARKGREY, scale);
			}
			else if (localBuffer.indexOf(",0,2,") != -1)
			{
				mp.display.setCursor(25, 9);
				mp.display.printCenter("Calling...");
				mp.display.drawBitmap(29*scale, 24*scale, call_icon, TFT_DARKGREY, scale);
			}
			else if (localBuffer.indexOf(",0,6,") != -1)
			{
				mp.display.fillScreen(TFT_WHITE);
				mp.display.setCursor(32, 9);
				if (timeOffset == 0)
					mp.display.printCenter("00:00");
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
				mp.display.printCenter(number);
				mp.display.fillRect(0, 51*scale, 80*scale, 13*scale, TFT_RED);
				if(mp.resolutionMode)
					mp.display.setCursor(2, 62);
				else
					mp.display.setCursor(2, 112);
				mp.display.print("Call ended");
				Serial.println("ENDED");
				while (!mp.update());
				delay(1000);
				break;
			}
			if(mp.resolutionMode)
					mp.display.setCursor(11, 20);
			else
				mp.display.setCursor(11, 28);
			mp.display.printCenter(number);
			mp.display.fillRect(0, 51*scale, 80*scale, 13*scale, TFT_RED);
			if(mp.resolutionMode)
			{
				mp.display.setCursor(2, 62);
				mp.display.print("press");
				mp.display.drawBitmap(24, 52, letterB, TFT_BLACK);
				mp.display.setCursor(35, 62);
				mp.display.print("to hang up");
			}
			else
			{
				mp.display.setCursor(2, 112);
				mp.display.print("press");
				mp.display.drawBitmap(37, 105, letterB, TFT_BLACK, scale);
				mp.display.setCursor(55, 112);
				mp.display.print("to hang up");
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
				mp.display.printCenter(number);
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
				mp.display.setCursor(25, 9);
				mp.display.printCenter("Calling...");
				mp.display.drawBitmap(29*scale, 24*scale, call_icon, TFT_DARKGREY, scale);
				if(mp.resolutionMode)
					mp.display.setCursor(11, 20);
				else
					mp.display.setCursor(11, 28);
				mp.display.printCenter(number);
				mp.display.fillRect(0, 51*scale, 80*scale, 13*scale, TFT_RED);
				if(mp.resolutionMode)
				{
					mp.display.setCursor(2, 62);
					mp.display.print("press");
					mp.display.drawBitmap(24, 52, letterB, TFT_BLACK);
					mp.display.setCursor(35, 62);
					mp.display.print("to hang up");
				}
				else
				{
					mp.display.setCursor(2, 112);
					mp.display.print("press");
					mp.display.drawBitmap(37, 105, letterB, TFT_BLACK, scale);
					mp.display.setCursor(55, 112);
					mp.display.print("to hang up");
				}
			}
		}
		if (mp.buttons.pressed(BTN_B)) // hanging up
		{
			Serial.println("B PRESSED");
			Serial1.println("ATH");
			while (readSerial().indexOf(",0,6,") == -1)
			{
				Serial1.println("ATH");
			}
			mp.display.fillScreen(TFT_WHITE);
			mp.display.setCursor(32, 9);
			if (timeOffset == 0)
				mp.display.printCenter("00:00");
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
			mp.display.printCenter(number);
			mp.display.fillRect(0, 51*scale, 80*scale, 13*scale, TFT_RED);
			mp.display.setCursor(2, 112);
			mp.display.print("Call ended");
			Serial.println("ENDED");
			while (!mp.update());
			delay(1000);
			break;
		}
		mp.update();
	}
}

void setup()
{
	Serial.begin(115200);
	mp.begin(0);
	osc = new Oscillator();
	osc->setVolume(256);
	addOscillator(osc);
}

void loop()
{
	phoneApp();
	lockscreen();
	mainMenu();
}
