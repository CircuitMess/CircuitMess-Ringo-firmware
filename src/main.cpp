#include <Arduino.h>
#include "main.h"
#include "lockscreen.h"
#include "mainMenu.h"
#include "sprites.c"
MAKERphone mp;
int backgroundColors[7] = {
    TFT_CYAN,
    TFT_GREEN,
    TFT_RED,
    TFT_YELLOW,
    TFT_WHITE,
    TFT_ORANGE,
    TFT_PINK
};
String backgroundColorsNames[7] = {
    "Cyan",
    "Green",
    "Red",
    "Yellow",
    "White",
    "Orange",
    "Pink"
};
String titles[10] = {
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
int32_t cameraY = 0;
int32_t cameraY_actual = 0;
uint16_t cursor = 0;

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
			mp.gui.osc->note(75, 0.05);
			mp.gui.osc->play();
			while (!mp.update());// Exit when pressed
			break;
		}
		if (mp.buttons.released(BTN_B))   //BUTTON BACK
			return -1;

		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
			mp.gui.osc->note(75, 0.05);
			mp.gui.osc->play();
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
			mp.gui.osc->note(75, 0.05);
			mp.gui.osc->play();
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
void setup() {
  Serial.begin(115200);
  mp.begin(0);
}
uint32_t timer = millis();
void loop()
{
  lockscreen();
  mainMenu();
}
