#include "settingsApp.h"
uint16_t pinNumber = 1234;
uint8_t timesRemaining = 0;
String settingsItems[6] PROGMEM = {
    "Network",
    "Display",
    "Date & time",
    "Sound",
    "Security",
    "About & update"
};
String notificationSounds[4] PROGMEM = {
	"Oxygen",
	"Resonance", 
	"Mayday", 
	"Buzzer"
};

bool pinLock = 0;
void settingsMenuDrawBox(String title, uint8_t i, int32_t y) {
	uint8_t	scale = 2;
	uint8_t	boxHeight = 20;
	y += i * boxHeight + settingsMenuYOffset;
	if (y < 0 || y > mp.display.width()) {
		return;
	}


	if (title == "Network") //red
	{
		mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2, 0xFB6D);
		mp.display.drawBitmap(6, y + 2*scale, network, 0x7800);
	}
	if (title == "Display") //green
	{
		mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2, 0x8FEA);
		mp.display.drawBitmap(6, y + 2*scale, displayIcon, 0x0341);
	}
	if (title == "Date & time") //yellow
	{
		mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2, 0xFFED);
		mp.display.drawBitmap(6, y + 2*scale, timeIcon, 0x6B60);
	}
	if (title == "Sound")//blue
	{
		mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2, 0xA7FF);
		mp.display.drawBitmap(6, y + 2*scale, soundIcon, 0x010F);
	}
	if (title == "Security")//purple
	{
		mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2, 0xED1F);
		mp.display.drawBitmap(6, y + 2*scale, security, 0x600F);
	}
	if (title == "About & update")//orange
	{
		mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2, 0xFD29);
		mp.display.drawBitmap(6, y + 2*scale, about, 0x8200);
	}
	mp.display.setTextColor(TFT_BLACK);
	mp.display.setTextSize(1);
	mp.display.setTextFont(2);
	mp.display.drawString(title, 30, y + 2 );
	mp.display.setTextColor(TFT_WHITE);
	mp.display.setFreeFont(TT1);
}
void settingsMenuDrawCursor(uint8_t i, int32_t y, bool pressed) {
	uint8_t	boxHeight = 20;
	if (millis() % 500 <= 250 && pressed == 0) {
		return;
	}
	y += i * boxHeight + settingsMenuYOffset;
	mp.display.drawRect(0, y-1, mp.display.width()-1, boxHeight+2, TFT_RED);
	mp.display.drawRect(1, y, mp.display.width()-3, boxHeight, TFT_RED);
}
int8_t settingsMenu(String* title, uint8_t length, uint8_t _cursor) {
	bool pressed = 0;
	uint8_t cursor = _cursor;
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	uint8_t	boxHeight = 20;
	while (1) {
		while (!mp.update());
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, 0);
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
		}

		for (uint8_t i = 0; i < length; i++) {
			settingsMenuDrawBox(title[i], i, cameraY_actual);
		}
		settingsMenuDrawCursor(cursor, cameraY_actual, pressed);

		if (mp.buttons.timeHeld(BTN_DOWN) == 0 && mp.buttons.timeHeld(BTN_UP) == 0)
			pressed = 0;

		if (mp.buttons.released(BTN_A)) {   //BUTTON CONFIRM
			osc->note(75, 0.05);
			osc->play();

			while (!mp.update());// Exit when pressed
			break;
		}

		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
			osc->note(75, 0.05);
			osc->play();
			if (cursor == 0) {
				cursor = length - 1;
				if (length > 6) {
					cameraY = -(cursor - 2) * boxHeight;
				}
			}
			else {
				cursor--;
				if (cursor > 0 && (cursor * boxHeight + cameraY + settingsMenuYOffset) < boxHeight) {
					cameraY += 15;
				}
			}
			pressed = 1;
		}

		if (mp.buttons.released(BTN_DOWN)) { //BUTTON DOWN
			osc->note(75, 0.05);
			osc->play();
			cursor++;
			if ((cursor * boxHeight + cameraY + settingsMenuYOffset) > 128) {
				cameraY -= boxHeight;
			}
			if (cursor >= length) {
				cursor = 0;
				cameraY = 0;

			}
			pressed = 1;
		}


		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			while(!mp.update());
			return -1;
		}
	}

	return cursor;

}
bool settingsApp() {
	int8_t input = 0;
	while (!mp.update());
	while (1)
	{
		input = settingsMenu(settingsItems, 6, input);
		if (input == -1) //BUTTON BACK
			break;
		if (input == 0)
			networkMenu();
		if (input == 1)
			displayMenu();
		if (input == 2)
			timeMenu();
		if (input == 3)
			soundMenu();
		if (input == 4)
			securityMenu();
		if (input == 5)
			if(updateMenu())
				return true;
	}

	mp.applySettings();
	mp.display.fillScreen(TFT_BLACK);
	mp.display.setTextColor(TFT_WHITE);
	if(mp.SDinsertedFlag)
	{
		mp.saveSettings(1);
		mp.display.setCursor(0, mp.display.height()/2 - 16);
		mp.display.setTextFont(2);
		mp.display.printCenter("Settings saved!");
		uint32_t tempMillis = millis();
		while(millis() < tempMillis + 2000)
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
	}
	else
	{
		mp.display.setCursor(0, mp.display.height()/2 - 20);
		mp.display.setTextFont(2);
		mp.display.printCenter("Can't save - No SD!");
		mp.display.setCursor(0, mp.display.height()/2);
		mp.display.printCenter("Insert SD card and reset");
		uint32_t tempMillis = millis();
		while(millis() < tempMillis + 2000)
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
	}
	return false;

}
void networkMenu() {
	uint8_t cursor = 0;
	bool blinkState = 0;
	uint32_t blinkMillis = millis();
	while (1)
	{
		Serial.println(mp.airplaneMode);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.fillScreen(0xFB6D);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(18, 20);
		mp.display.print("Wifi");
		mp.display.setCursor(22, 58);
		mp.display.print("BT");
		mp.display.setCursor(16, 88);
		mp.display.print("Plane\n   mode");

		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(79, 19);
		mp.display.print("ON");
		mp.display.setCursor(122, 19);
		mp.display.print("OFF");
		mp.display.setCursor(79, 57);
		mp.display.print("ON");
		mp.display.setCursor(122, 57);
		mp.display.print("OFF");
		mp.display.setCursor(79, 95);
		mp.display.print("ON");
		mp.display.setCursor(122, 95);
		mp.display.print("OFF");
		switch (cursor) {

		case 0:
			if (mp.bt == 1)
			{
				mp.display.drawRect(35*2, 27*2, 17*2, 11*2, TFT_BLACK);
			}
			else
			{
				mp.display.drawRect(57*2, 27*2, 20*2, 11*2, TFT_BLACK);
			}
			if (mp.airplaneMode == 1)
			{
				mp.display.drawRect(35*2, 46*2, 17*2, 11*2, TFT_BLACK);
			}
			else
			{
				mp.display.drawRect(57*2, 46*2, 20*2, 11*2, TFT_BLACK);
			}
			break;

		case 1:
			if (mp.wifi == 1)
			{
				mp.display.drawRect(35*2, 8*2, 17*2, 11*2, TFT_BLACK);
			}
			else
			{
				mp.display.drawRect(57*2, 8*2, 20*2, 11*2, TFT_BLACK);
			}

			if (mp.airplaneMode == 1)
			{
				mp.display.drawRect(35*2, 46*2, 17*2, 11*2, TFT_BLACK);
			}
			else
			{
				mp.display.drawRect(57*2, 46*2, 20*2, 11*2, TFT_BLACK);
			}
			break;

		case 2:
			if (mp.wifi == 1)
			{
				mp.display.drawRect(35*2, 8*2, 17*2, 11*2, TFT_BLACK);
			}
			else
			{
				mp.display.drawRect(57*2, 8*2, 20*2, 11*2, TFT_BLACK);
			}

			if (mp.bt == 1)
			{
				mp.display.drawRect(35*2, 27*2, 17*2, 11*2, TFT_BLACK);
			}
			else
			{
				mp.display.drawRect(57*2, 27*2, 20*2, 11*2, TFT_BLACK);
			}
		}




		if (cursor == 0)
		{
			if (blinkState && mp.wifi)
			{
				mp.display.drawRect(35*2, 8*2, 17*2, 11*2, TFT_BLACK);
			}
			else if (blinkState && !mp.wifi)
			{
				mp.display.drawRect(57*2, 8*2, 20*2, 11*2, TFT_BLACK);
			}
			if (mp.buttons.released(BTN_LEFT) && !mp.wifi)
			{
				while(!mp.update());
				osc->note(75, 0.05);
				osc->play();
				blinkState = 1;
				blinkMillis = millis();
				mp.wifi = !mp.wifi;
			}
			if (mp.buttons.released(BTN_RIGHT) && mp.wifi)
			{
				while(!mp.update());
				osc->note(75, 0.05);
				osc->play();
				blinkState = 1;
				blinkMillis = millis();
				mp.wifi = !mp.wifi;
			}
		}
		if (cursor == 1)
		{
			if (blinkState && mp.bt)
			{
				mp.display.drawRect(35*2, 27*2, 17*2, 11*2, TFT_BLACK);
			}
			else if (blinkState && !mp.bt)
			{
				mp.display.drawRect(57*2, 27*2, 20*2, 11*2, TFT_BLACK);
			}
			if (mp.buttons.released(BTN_LEFT) && !mp.bt)
			{
				while(!mp.update());
				osc->note(75, 0.05);
				osc->play();
				blinkState = 1;
				blinkMillis = millis();
				mp.bt = !mp.bt;
			}
			if (mp.buttons.released(BTN_RIGHT) && mp.bt)
			{
				while(!mp.update());
				osc->note(75, 0.05);
				osc->play();
				blinkState = 1;
				blinkMillis = millis();
				mp.bt = !mp.bt;
			}
		}
		if (cursor == 2)
		{
			if (blinkState && mp.airplaneMode)
			{
				mp.display.drawRect(35*2, 46*2, 17*2, 11*2, TFT_BLACK);
			}
			else if (blinkState && !mp.airplaneMode)
			{
				mp.display.drawRect(57*2, 46*2, 20*2, 11*2, TFT_BLACK);
			}
			if (mp.buttons.released(BTN_LEFT) && !mp.airplaneMode)
			{
				while(!mp.update());
				osc->note(75, 0.05);
				osc->play();
				blinkState = 1;
				blinkMillis = millis();
				mp.airplaneMode = !mp.airplaneMode;
			}
			if (mp.buttons.released(BTN_RIGHT) && mp.airplaneMode)
			{
				while(!mp.update());
				osc->note(75, 0.05);
				osc->play();
				blinkState = 1;
				blinkMillis = millis();
				mp.airplaneMode = !mp.airplaneMode;
			}
		}

		if (mp.buttons.released(BTN_UP))
		{
			osc->note(75, 0.05);
			osc->play();
			blinkState = 0;
			blinkMillis = millis();
			while (!mp.update());
			if (cursor == 0)
				cursor = 2;
			else
				cursor--;
		}
		if (mp.buttons.released(BTN_DOWN))
		{
			osc->note(75, 0.05);
			osc->play();
			blinkState = 0;
			blinkMillis = millis();
			while (!mp.update());
			if (cursor == 2)
				cursor = 0;
			else
				cursor++;

		}
		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			while(!mp.update());
			break;
		}

		if(millis() - blinkMillis >= 250)
		{
			blinkState = !blinkState;
			blinkMillis = millis();
		}
		mp.update();
	}
}
void displayMenu() {
	mp.display.setTextFont(1);
	mp.display.setTextColor(TFT_BLACK);
	uint8_t sleepTimeBuffer = mp.sleepTime;
	uint16_t sleepTimeActualBuffer = mp.sleepTimeActual;
	uint8_t cursor = 0;
	while (1)
	{
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.fillScreen(0x8FEA);
		mp.display.setCursor(9*2, 2);
		mp.display.printCenter("Brightness");
		mp.display.drawRect(33, 21, 47*2, 4*2, TFT_BLACK);
		mp.display.drawBitmap(12, 20, noBrightness, TFT_BLACK, 2);
		mp.display.drawBitmap(132, 14, fullBrightness, TFT_BLACK, 2);
		mp.display.fillRect(35, 23, mp.brightness * 9*2, 2*2, TFT_BLACK);

		mp.display.setCursor(9*2, 32);
		mp.display.printCenter("LED brightness");
		mp.display.drawRect(33, 50, 47*2, 4*2, TFT_BLACK);
		mp.display.fillRect(35, 52, mp.pixelsBrightness * 9*2, 2*2, TFT_BLACK);

		String foo = "Sleep: ";
		if (sleepTimeActualBuffer > 60)
		{
			foo += sleepTimeActualBuffer / 60;
			foo += "min";
		}
		else
		{
			foo += sleepTimeActualBuffer;
			foo += "s";
		}
		mp.display.setCursor(10*2, 59);
		mp.display.printCenter(foo);

		mp.display.drawRect(33, 77, 47*2, 4*2, TFT_BLACK);
		mp.display.fillRect(35, 79, sleepTimeBuffer * 9*2, 2*2, TFT_BLACK);

		mp.display.setCursor(12, 72);
		mp.display.print("0s");
		mp.display.setCursor(132, 72);
		mp.display.print("30m");

		mp.display.setCursor(11*2, 87);
		mp.display.printCenter("Background");
		mp.display.fillRect(16*2, 105, 48*2, 9*2, backgroundColors[mp.backgroundIndex]);
		mp.display.setCursor(18*2, 105);
		mp.display.printCenter(backgroundColorsNames[mp.backgroundIndex]);
		mp.display.drawBitmap(11*2, 107, arrowLeft, TFT_BLACK, 2);
		mp.display.drawBitmap(65*2, 107, arrowRight, TFT_BLACK, 2);

		if (cursor == 0)
		{
			if (millis() % 1000 <= 500)
			{
				mp.display.drawBitmap(12, 20, noBrightness, TFT_BLACK, 2);
				mp.display.drawBitmap(132, 14, fullBrightness, TFT_BLACK, 2);

			}
			else
			{
				mp.display.drawBitmap(12, 20, noBrightness, 0x8FEA, 2);
				mp.display.drawBitmap(132, 14, fullBrightness, 0x8FEA, 2);
			}
			if (mp.buttons.released(BTN_LEFT) && mp.brightness > 0)
			{
				osc->note(75, 0.05);
				osc->play();
				mp.brightness--;
				// while(!mp.update());
			}
			if (mp.buttons.released(BTN_RIGHT) && mp.brightness < 5)
			{
				osc->note(75, 0.05);
				osc->play();
				mp.brightness++;
				// while(!mp.update());
			}
		}
		if (cursor == 1)
		{
			for(int i = 0;i<8;i++)
				mp.leds[i] = CRGB::White;
			if (mp.buttons.released(BTN_LEFT) && mp.pixelsBrightness > 0)
			{
				osc->note(75, 0.05);
				osc->play();
				mp.pixelsBrightness--;
				// while(!mp.update());
			}
			if (mp.buttons.released(BTN_RIGHT) && mp.pixelsBrightness < 5)
			{
				osc->note(75, 0.05);
				osc->play();
				mp.pixelsBrightness++;
				// while(!mp.update());
			}
		}
		if (cursor == 2)
		{
			if (millis() % 1000 <= 500)
			{
				mp.display.setCursor(12, 72);
				mp.display.print("0s");
				mp.display.setCursor(132, 72);
				mp.display.print("30m");
			}
			else
			{
				mp.display.setTextColor(0x8FEA);
				mp.display.setCursor(12, 72);
				mp.display.print("0s");
				mp.display.setCursor(132, 72);
				mp.display.print("30m");
				mp.display.setTextColor(TFT_BLACK);
			}
			if (mp.buttons.released(BTN_LEFT) && sleepTimeBuffer!= 0)
			{
				osc->note(75, 0.05);
				osc->play();
				sleepTimeBuffer--;
				while(!mp.update());
			}
			if (mp.buttons.released(BTN_RIGHT) && sleepTimeBuffer!= 5)
			{
				osc->note(75, 0.05);
				osc->play();
				sleepTimeBuffer++;
				while(!mp.update());
			}
		}
		if (cursor == 3)
		{
			if (millis() % 1000 <= 500)
			{
				if (mp.backgroundIndex == 0)
				{
					mp.display.fillRect(65*2 , 105, 20, 20, 0x8FEA);
					mp.display.drawBitmap(11*2, 107, arrowLeft, TFT_BLACK, 2);
					mp.display.drawBitmap(66*2, 107, arrowRight, TFT_BLACK, 2);
				}
				else if (mp.backgroundIndex == 6)
				{
					mp.display.fillRect(5*2 , 105, 20, 20, 0x8FEA);
					mp.display.drawBitmap(10*2, 107, arrowLeft, TFT_BLACK, 2);
					mp.display.drawBitmap(65*2, 107, arrowRight, TFT_BLACK, 2);
				}
				else
				{
					mp.display.fillRect(65*2 , 105, 20, 20, 0x8FEA);
					mp.display.fillRect(5*2, 105, 20, 20, 0x8FEA);
					mp.display.drawBitmap(10*2, 107, arrowLeft, TFT_BLACK, 2);
					mp.display.drawBitmap(66*2, 107, arrowRight, TFT_BLACK, 2);
				}
			}
			else
			{
				mp.display.drawBitmap(11*2, 107, arrowLeft, TFT_BLACK, 2);
				mp.display.drawBitmap(65*2, 107, arrowRight, TFT_BLACK, 2);
			}
			if (mp.buttons.released(BTN_LEFT) && mp.backgroundIndex != 0)
			{
				osc->note(75, 0.05);
				osc->play();
				mp.backgroundIndex--;
				while(!mp.update());
			}
			if (mp.buttons.released(BTN_RIGHT) && mp.backgroundIndex != 6)
			{
				osc->note(75, 0.05);
				osc->play();
				mp.backgroundIndex++;
				while(!mp.update());
			}
		}

		if (mp.buttons.released(BTN_UP))
		{
			osc->note(75, 0.05);
			osc->play();
			while (!mp.update());
  			if (cursor == 0)
				cursor = 3;
			else
				cursor--;
		}
		if (mp.buttons.released(BTN_DOWN))
		{
			osc->note(75, 0.05);
			osc->play();
			while (!mp.update());
			if (cursor == 3)
				cursor = 0;
			else
				cursor++;
		}

		switch (sleepTimeBuffer) { //interpreting value into actual numbers
			case 0:
				sleepTimeActualBuffer = 0;
				break;
			case 1:
				sleepTimeActualBuffer = 10;
				break;
			case 2:
				sleepTimeActualBuffer = 30;
				break;
			case 3:
				sleepTimeActualBuffer = 60;
				break;
			case 4:
				sleepTimeActualBuffer = 600;
				break;
			case 5:
			sleepTimeActualBuffer = 1800;
			break;
		}
		if (mp.buttons.released(BTN_B)) //BUTTON BACK
			break;

		mp.update();

		if (mp.brightness == 0)
			mp.actualBrightness = 230;
		else
			mp.actualBrightness = (5 - mp.brightness) * 51;
		mp.ledcAnalogWrite(LEDC_CHANNEL, mp.actualBrightness);
	}
	mp.sleepTimer = millis();
	mp.sleepTime = sleepTimeBuffer;
	mp.sleepTimeActual = sleepTimeActualBuffer;
}
void soundMenu() {
	if(mp.SDinsertedFlag)
	{
		listAudio("/Music", 0);
		String tempList[audioCount];
		uint16_t tempCount = audioCount;
		for (int i = 0; i < audioCount;i++)
			tempList[i] = audioFiles[i];
		listAudio("/Ringtones", 0);
		for (int i = 0; i < tempCount;i++)
			audioFiles[i + audioCount] = tempList[i];
		audioCount += tempCount;
	}
	String parsedRingtone;
	uint8_t start = 0;
	int8_t i;
	mp.display.setTextFont(1);
	mp.display.setTextColor(TFT_BLACK);
	uint8_t cursor = 0;
	while (mp.ringtone_path.indexOf("/", start) != -1)
		start = mp.ringtone_path.indexOf("/", start) + 1;
	parsedRingtone = mp.ringtone_path.substring(start, mp.ringtone_path.indexOf("."));
	start = 0;
	bool blinkState = 0;
	uint32_t blinkMillis = millis();
	while (1)
	{
		mp.display.setTextFont(2);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.fillScreen(0xA7FF);
		mp.display.setCursor(20, 4);
		mp.display.printCenter("Volume");
		mp.display.drawRect(33, 12*2, 47*2, 4*2, TFT_BLACK);
		mp.display.drawBitmap(4*2, 10*2, noSound, TFT_BLACK, 2);
		mp.display.drawBitmap(67*2, 10*2, fullSound, TFT_BLACK, 2);
		mp.display.fillRect(35, 13*2, mp.volume * 3*2, 2*2, TFT_BLACK);
		mp.display.setCursor(15, 37);
		mp.display.printCenter("Ringtone");
		mp.display.drawRect(3*2, 55, 74*2, 11*2, TFT_BLACK);
		mp.display.setCursor(6*2, 58);
		mp.display.print(parsedRingtone);

		mp.display.setCursor(5, 82);
		mp.display.printCenter("Notification");
		mp.display.drawRect(3*2, 100, 74*2, 11*2, TFT_BLACK);
		mp.display.setCursor(6*2, 103);
		mp.display.printCenter(notificationSounds[mp.notification]);
		if(millis()-blinkMillis >= 350)
		{
			blinkMillis = millis();
			blinkState = !blinkState;
		}
		if (cursor == 0)
		{
			if (blinkState)
			{
				mp.display.drawBitmap(4*2, 10*2, noSound, TFT_BLACK, 2);
				mp.display.drawBitmap(67*2, 10*2, fullSound, TFT_BLACK, 2);
			}
			else
			{
				mp.display.drawBitmap(4*2, 10*2, noSound, 0xA7FF, 2);
				mp.display.drawBitmap(67*2, 10*2, fullSound, 0xA7FF, 2);
			}
			if (mp.buttons.released(BTN_LEFT) && mp.volume != 0)
			{
				mp.volume--;
				osc->setVolume(256 * mp.volume / 14);
				osc->note(75, 0.05);
				osc->play();
				while(!mp.update());
			}
			if (mp.buttons.released(BTN_RIGHT) && mp.volume != 15)
			{
				mp.volume++;
				osc->setVolume(256 * mp.volume / 14);
				osc->note(75, 0.05);
				osc->play();
				while(!mp.update());
			}
		}
		if (cursor == 1)
		{
			if (blinkState)
				mp.display.drawRect(3*2, 55, 74*2, 11*2, TFT_BLACK);
			else
				mp.display.drawRect(3*2, 55, 74*2, 11*2, 0xA7FF);

			if (mp.buttons.released(BTN_A))
			{
				while(!mp.update());
				if(mp.SDinsertedFlag)
				{
					osc->note(75, 0.05);
					osc->play();
					i = audioPlayerMenu("Select ringtone:", audioFiles, audioCount);
					mp.display.setTextColor(TFT_BLACK);
					if (i >= 0)
						mp.ringtone_path = audioFiles[i];
					else
						Serial.println("pressed B in menu");
					start = 0;
					while (mp.ringtone_path.indexOf("/", start) != -1)
						start = mp.ringtone_path.indexOf("/", start) + 1;
					parsedRingtone = mp.ringtone_path.substring(start, mp.ringtone_path.indexOf("."));
				}
				else
				{
					mp.display.setTextColor(TFT_BLACK);
					mp.display.setTextSize(1);
					mp.display.setTextFont(2);
					mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
					mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
					mp.display.fillRect(15, 46, 132, 36, 0xA7FF);
					mp.display.setCursor(47, 55);
					mp.display.printCenter("No SD card!");
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
				}
				
			}
		}
		if (cursor == 2)
		{
			if (blinkState)
				mp.display.drawRect(3*2, 100, 74*2, 11*2, TFT_BLACK);
			else
				mp.display.drawRect(3*2, 100, 74*2, 11*2, 0xA7FF);

			if (mp.buttons.released(BTN_A))
			{
				while(!mp.update());
				int8_t temp = notificationsAudioMenu(notificationSounds, 4);
				if(temp>=0)
					mp.notification = temp;
			}
		}

		if (mp.buttons.released(BTN_UP))
		{
			osc->note(75, 0.05);
			osc->play();
			if (cursor == 0)
				cursor = 2;
			else
				cursor--;
			while(!mp.update());
			blinkState = 0;
			blinkMillis = millis();
		}
		if (mp.buttons.released(BTN_DOWN))
		{
			osc->note(75, 0.05);
			osc->play();
			if (cursor == 2)
				cursor = 0;
			else
				cursor++;
			while(!mp.update());
			blinkState = 0;
			blinkMillis = millis();
		}
		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			while(!mp.update());
			break;
		}
		mp.update();
	}
}
void securityMenu() {

	pinNumber = 1234;

	String pinBuffer = "";
	String reply = "";
	String oldPin = "";
	uint32_t elapsedMillis = millis();
	uint32_t blinkMillis = millis();
	uint8_t cursor = 0;
	bool errorMessage = 0;
	bool confirmMessage = 0;
	bool saved = 0;
	char key = NO_KEY;
	bool blinkState = 0;
	if(!mp.simInserted)
	{
		mp.display.setTextColor(TFT_BLACK);
		mp.display.setTextSize(1);
		mp.display.setTextFont(2);
		mp.display.drawRect(14, 44, 134, 38, TFT_BLACK);
		mp.display.drawRect(13, 43, 136, 40, TFT_BLACK);
		mp.display.fillRect(15, 45, 132, 36, TFT_WHITE);
		mp.display.setCursor(0, mp.display.height()/2 - 21);
		mp.display.setCursor(47, 54);
		mp.display.printCenter("No SIM card!");
		uint32_t tempMillis = millis();
		while(millis() < tempMillis + 2000)
		{
			mp.update();
			if(mp.buttons.pressed(BTN_A) || mp.buttons.pressed(BTN_B))
			{
				while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
					mp.update();
				break;
			}
		}
		return;
	}
	while (!mp.simReady)
	{
		mp.display.setCursor(0, mp.display.height()/2 - 16);
		mp.display.setTextFont(2);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.fillScreen(TFT_BLACK);
		mp.display.printCenter("GSM still booting...");
		mp.update();
	}
	mp.display.setCursor(0, mp.display.height()/2 - 16);
	mp.display.setTextFont(2);
	mp.display.setTextColor(TFT_WHITE);
	mp.display.fillScreen(TFT_BLACK);
	mp.display.printCenter("Loading SIM card...");
	while(!mp.update());
	while (reply.indexOf("+SPIC:") == -1)
	{
		mp.update();
		Serial1.println("AT+SPIC");
		reply = Serial1.readString();
		Serial.println(reply);
		delay(5);
	}
	uint8_t foo = reply.indexOf(" ", reply.indexOf("+SPIC:"));
	timesRemaining = reply.substring(foo, reply.indexOf(",", foo)).toInt();
	Serial.println(timesRemaining);
	delay(5);
	if (timesRemaining == 0) //PUK lock WIP
		mp.enterPUK();
	//check if the SIM card is locked
	while (reply.indexOf("+CLCK:") == -1)
	{
		Serial1.println("AT+CLCK=\"SC\", 2");
		reply = Serial1.readString();
		Serial.println(reply);
		delay(5);
	}
	if (reply.indexOf("+CLCK: 0") != -1)
		pinLock = 0;
	else if (reply.indexOf("+CLCK: 1") != -1)
		pinLock = 1;
	bool pinLockBuffer = pinLock;

	while (1)
	{
		if (timesRemaining == 0) //PUK lock WIP
		{
			mp.enterPUK();
			pinLockBuffer = 1;
			timesRemaining = 3;
		}
		else
		{
			mp.display.setTextColor(TFT_BLACK);
			mp.display.fillScreen(0xED1F);
			mp.display.setTextFont(2);
			mp.display.setCursor(8, 15);
			mp.display.print("PIN lock");
			mp.display.setCursor(78, 15);
			mp.display.print("ON");
			mp.display.setCursor(120, 15);
			mp.display.print("OFF");

			if (pinLockBuffer == 1)
			{
				mp.display.setTextColor(TFT_BLACK);
				mp.display.drawRect(3*2, 40*2, 74*2, 11*2, TFT_BLACK);
			}
			else
			{
				mp.display.setTextColor(TFT_DARKGREY);
				mp.display.fillRect(3*2, 40*2, 74*2, 11*2, TFT_DARKGREY);
			}
			mp.display.setCursor(4*2, 31*2);
			mp.display.print("PIN:");
			mp.display.setCursor(6*2, 83);
			if (pinBuffer != "")
				mp.display.printCenter(pinBuffer);
			else if (pinBuffer == "" && cursor != 1)
				mp.display.printCenter("****");
			if (pinLockBuffer == 1 && cursor != 0)
				mp.display.drawRect(69, 12, 17*2, 11*2, TFT_BLACK);
			else if (pinLockBuffer == 0 && cursor != 0)
				mp.display.drawRect(113, 12, 38, 11*2, TFT_BLACK);
			mp.display.setCursor(2, 111);
			if (cursor == 1 && !errorMessage && !confirmMessage)
				mp.display.print("Press A to save PIN");
			if (millis() - blinkMillis >= multi_tap_threshold) //cursor blinking routine
			{
				blinkMillis = millis();
				blinkState = !blinkState;
			}

			if (cursor == 0)
			{
				if (millis() % 500 <= 250 && pinLockBuffer == 1)
					mp.display.drawRect(69, 12, 17*2, 11*2, TFT_BLACK);
				else if (millis() % 500 <= 250 && pinLockBuffer == 0)
					mp.display.drawRect(113, 12, 38, 11*2, TFT_BLACK);
				if (mp.buttons.released(BTN_LEFT) && pinLockBuffer == 0)
				{
					osc->note(75, 0.05);
					osc->play();
					pinLockBuffer = !pinLockBuffer;
				}
				if (mp.buttons.released(BTN_RIGHT) && pinLockBuffer == 1)
				{
					osc->note(75, 0.05);
					osc->play();
					pinBuffer = "";
					pinLockBuffer = !pinLockBuffer;
					if (pinLock)
						while (1)
						{
							mp.display.setTextColor(TFT_BLACK);
							mp.display.setTextSize(1);
							mp.display.setTextFont(2);
							mp.display.fillScreen(0xED1F);
							mp.display.setCursor(5, 7);
							mp.display.printCenter("Enter PIN:");
							mp.display.drawRect(14, 45, 134, 30, TFT_BLACK);
							mp.display.drawRect(13, 44, 136, 32, TFT_BLACK);
							mp.display.setCursor(47, 52);
							mp.display.printCenter(pinBuffer);

							mp.display.setCursor(1, 112);
							mp.display.print("Erase");
							mp.display.setCursor(110, 112);
							mp.display.print("Confirm");
							mp.display.setCursor(5, 85);
							String temp = "Remaining attempts: ";
							temp+=timesRemaining;
							mp.display.printCenter(temp);

							key = mp.buttons.getKey();
							if (mp.buttons.released(BTN_FUN_RIGHT)) //clear number
							{
								pinBuffer = "";
								while(!mp.update());
							}
							else if (mp.buttons.released(BTN_FUN_LEFT) && pinBuffer != "")
							{
								pinBuffer.remove(pinBuffer.length() - 1);
								while(!mp.update());
							}
							if (key != NO_KEY && isDigit(key) && pinBuffer.length() != 4)
								pinBuffer += key;

							if (mp.buttons.released(BTN_A))//enter PIN
							{
								while(!mp.update());
								reply = "";
								Serial1.print(F("AT+CLCK=\"SC\", 0, \""));
								Serial1.print(pinBuffer);
								Serial1.println("\"");
								while (!Serial1.available());
								while (reply.indexOf("OK", reply.indexOf("AT+CLCK")) == -1 && reply.indexOf("ERROR", reply.indexOf("AT+CLCK")) == -1)
									reply = Serial1.readString();
								mp.display.fillScreen(0xED1F);
								mp.display.setCursor(0, mp.display.height()/2 - 16);
								mp.display.setTextFont(2);
								if (reply.indexOf("OK", reply.indexOf("AT+CLCK")) != -1)
								{
									mp.display.printCenter("PIN OK :)");
									while (!mp.update());
									delay(1000);
									pinLock = 0;
									pinLockBuffer = 0;
									timesRemaining = 3;
									break;
								}
								else if (reply.indexOf("ERROR", reply.indexOf("AT+CLCK")) != -1)
								{


									Serial.println("Start:");
									Serial.println(reply);
									Serial.println("End");
									delay(5);
									pinBuffer = "";
									Serial.println(reply.indexOf("password"));
									delay(5);
									if (reply.indexOf("password") != -1)
									{
										mp.display.printCenter("Wrong PIN :(");
										timesRemaining--;
									}
									else if (reply.indexOf("PUK") != -1)
									{
										timesRemaining = 0;
										break;
									}
									else
										mp.display.printCenter("Invalid PIN");
									while (!mp.update());
									delay(1000);
								}
								pinBuffer = "";
							}
							if (timesRemaining == 0)
								break;
							if (mp.buttons.released(BTN_B))
							{
								pinLockBuffer = 1;
								while (!mp.update());
								break;
							}

							mp.update();
						}

				}
				pinBuffer = "";
			}


			else if (cursor == 1 && pinLockBuffer == 1)
			{
				key = mp.buttons.getKey();
				if (mp.buttons.released(BTN_FUN_RIGHT)) //clear number
				{
					pinBuffer = "";
					while(!mp.update());
				}
				else if (mp.buttons.released(BTN_FUN_LEFT))
				{
					pinBuffer.remove(pinBuffer.length() - 1);
					while(!mp.update());
				}
				if (key != NO_KEY && isdigit(key) && pinBuffer.length() < 4)
					pinBuffer += key;
				mp.display.setCursor(6*2, 83);
				mp.display.setTextFont(2);
				mp.display.printCenter(pinBuffer);
				if (blinkState == 1)
					mp.display.drawFastVLine(mp.display.getCursorX()+1, mp.display.getCursorY()+2, 12, TFT_BLACK);
				if (mp.buttons.released(BTN_A) && pinBuffer.length() == 4 && confirmMessage == 0)
				{
					osc->note(75, 0.05);
					osc->play();

					while (!mp.update());
					pinNumber = pinBuffer.toInt();

					reply = "";

					mp.display.fillScreen(0xED1F);

					while (1)
					{
						mp.display.setTextColor(TFT_BLACK);
						mp.display.setTextSize(1);
						mp.display.setTextFont(2);
						mp.display.fillScreen(0xED1F);
						mp.display.setCursor(5, 7);
						mp.display.printCenter("Enter old PIN:");
						mp.display.drawRect(14, 45, 134, 30, TFT_BLACK);
						mp.display.drawRect(13, 44, 136, 32, TFT_BLACK);
						mp.display.setCursor(47, 52);
						mp.display.printCenter(oldPin);

						mp.display.setCursor(1, 112);
						mp.display.print("Erase");
						mp.display.setCursor(110, 112);
						mp.display.print("Confirm");
						mp.display.setCursor(5, 85);
						String temp = "Remaining attempts: ";
						temp+=timesRemaining;
						mp.display.printCenter(temp);

						key = mp.buttons.getKey();
						if (mp.buttons.released(BTN_FUN_RIGHT)) //clear number
						{
							oldPin = "";
							while(!mp.update());
						}
						else if (mp.buttons.released(BTN_FUN_LEFT))
						{
							oldPin.remove(oldPin.length() - 1);
							while(!mp.update());
						}
						if (key != NO_KEY && isDigit(key) && oldPin.length() != 4)
							oldPin += key;

						if ((mp.buttons.released(BTN_A) || mp.released(BTN_FUN_RIGHT)) && oldPin.length() == 4)//enter PIN
						{
							while(!mp.update());
							if (pinLock)
							{
								reply = "";
								Serial1.print(F("AT+CPWD=\"SC\", \""));
								Serial1.print(oldPin);
								Serial1.print("\", \"");
								Serial1.print(pinBuffer);
								Serial1.println("\"");
								while (!Serial1.available());
								while (reply.indexOf("OK", reply.indexOf("AT+CPWD")) == -1 && reply.indexOf("ERROR", reply.indexOf("AT+CPWD")) == -1)
								{
									reply = Serial1.readString();
									Serial.println(reply);
									delay(5);
								}
								mp.display.fillScreen(0xED1F);
								mp.display.setCursor(0, mp.display.height()/2 - 16);
								mp.display.setTextFont(2);
								mp.display.setTextColor(TFT_BLACK);
								if (reply.indexOf("OK", reply.indexOf("AT+CPWD")) != -1)
								{
									timesRemaining = 3;
									pinLock = 1;
									saved = 1;
									break;
								}
								else if (reply.indexOf("ERROR", reply.indexOf("AT+CPWD")) != -1)
								{

									if(reply.indexOf("incorrect") != -1)
									{
										oldPin = "";
										timesRemaining--;
										if(timesRemaining == 0)
											break;
										mp.display.printCenter("Wrong PIN :(");
									}
									else if (reply.indexOf("PUK") != -1)
									{
										timesRemaining = 0;
										break;
									}
									else
										mp.display.printCenter("Invalid PIN");
									while (!mp.update());
									delay(1000);
								}
							}
							else
							{
								Serial1.print(F("AT+CLCK=\"SC\", 1, \""));
								Serial1.print(oldPin);
								Serial1.println("\"");
								while (!Serial1.available());
								while (reply.indexOf("OK", reply.indexOf("AT+CLCK")) == -1 && reply.indexOf("ERROR", reply.indexOf("AT+CLCK")) == -1)
								{
									reply = Serial1.readString();
									Serial.println(reply);
									delay(5);
								}
								mp.display.fillScreen(0xED1F);
								mp.display.setCursor(0, mp.display.height()/2 - 16);
								mp.display.setTextFont(2);
								mp.display.setTextColor(TFT_BLACK);
								if (reply.indexOf("OK", reply.indexOf("AT+CLCK")) != -1)
								{
									reply = "";
									Serial1.print(F("AT+CPWD=\"SC\", \""));
									Serial1.print(oldPin);
									Serial1.print("\", \"");
									Serial1.print(pinBuffer);
									Serial1.println("\"");
									while (!Serial1.available());
									while (reply.indexOf("OK", reply.indexOf("AT+CPWD")) == -1 && reply.indexOf("ERROR", reply.indexOf("AT+CPWD")) == -1)
									{
										reply = Serial1.readString();
										Serial.println(reply);
										delay(5);
									}
									if (reply.indexOf("OK", reply.indexOf("AT+CPWD")) != -1)
									{
										timesRemaining = 3;
										pinLock = 1;
										saved = 1;
										break;
									}
									else if (reply.indexOf("ERROR", reply.indexOf("AT+CPWD")) != -1)
									{
										oldPin = "";
										while (!mp.update());
										timesRemaining--;
										saved = 0;
										if (timesRemaining == 0)
											break;
										saved = 0;
									}
								}
								else
								{
									saved = 0;
									if (reply.indexOf("incorrect") != -1)
									{
										timesRemaining--;
										mp.display.printCenter("Wrong PIN :(");
									}
									else if (reply.indexOf("PUK") != -1)
									{
										timesRemaining = 0;
										break;
									}
									else
										mp.display.printCenter("Invalid PIN");
									while (!mp.update());
									delay(1000);
									Serial.println(timesRemaining);
									delay(5);
									if (timesRemaining == 0)
										break;
								}

							}
							oldPin = "";
						}
						if (mp.buttons.released(BTN_B))
						{
							saved = 0;
							while (!mp.update());
							break;
						}
						if (timesRemaining == 0)
							break;
						mp.update();
					}

					oldPin = "";
					pinBuffer = "";
					if (saved)
					{
						mp.display.setCursor(2, 111);
						mp.display.fillRect(2*2, 57*2, 78*2, 5*2, 0xED1F);
						mp.display.print("PIN saved!");
						elapsedMillis = millis();
						confirmMessage = 1;
						errorMessage = 0;
						pinLock = 1;
						timesRemaining = 3;
					}
					else
					{
						confirmMessage = 0;
						errorMessage = 0;
						pinLock = 0;
					}
				}

				if (mp.buttons.released(BTN_A) && pinBuffer.length() < 4 && errorMessage == 0)
				{
					osc->note(75, 0.05);
					osc->play();
					while (!mp.update());
					mp.display.setCursor(2, 111);
					mp.display.print("Pin must have 4+ digits");
					elapsedMillis = millis();
					errorMessage = 1;
					confirmMessage = 0;
				}
				if (millis() - elapsedMillis >= 2000 && errorMessage == 1)
				{
					mp.display.fillRect(2*2, 57*2, 78*2, 5*2, 0xED1F);
					errorMessage = 0;
				}
				else if (millis() - elapsedMillis < 2000 && errorMessage == 1)
				{
					mp.display.fillRect(2*2, 57*2, 78*2, 5*2, 0xED1F);
					mp.display.setCursor(2, 111);
					mp.display.print("Pin must have 4+ digits");
				}
				if (millis() - elapsedMillis >= 2000 && confirmMessage == 1)
				{
					mp.display.fillRect(2*2, 57*2, 78*2, 5*2, 0xED1F);
					confirmMessage = 0;
				}
				else if (millis() - elapsedMillis < 2000 && confirmMessage == 1)
				{
					mp.display.fillRect(2*2, 56*2, 78*2, 6*2, 0xED1F);
					mp.display.setCursor(2, 111);
					mp.display.print("PIN saved!");
				}
			}

			if (mp.buttons.released(BTN_UP))
			{
				osc->note(75, 0.05);
				osc->play();
				while (!mp.update());
				if (cursor == 0 && pinLockBuffer == 1)
					cursor = 1;
				else if (pinLockBuffer == 1 && cursor == 1)
					cursor--;
			}
			if (mp.buttons.released(BTN_DOWN))
			{
				osc->note(75, 0.05);
				osc->play();
				while (!mp.update());
				if (cursor == 1)
					cursor = 0;
				else if (pinLockBuffer == 1 && cursor == 0)
					cursor++;
			}
			if (mp.buttons.released(BTN_B)) //BUTTON BACK
				break;
			mp.update();
		}

	}
}
void timeMenu()
{
	bool blinkState = 0;
	uint32_t previousMillis = millis();
	uint8_t cursor = 0;
	uint8_t editCursor = 0;
	String foo="";
	char key;
	mp.display.setTextWrap(0);
	while(1)
	{
		mp.display.fillScreen(0xFFED);
		mp.display.setTextFont(2);
		mp.display.setTextColor(TFT_BLACK);

		mp.display.setCursor(52,10);
		if (mp.clockHour < 10)
			mp.display.print("0");
		mp.display.print(mp.clockHour);
		if(blinkState)
			mp.display.print(":");
		else
			mp.display.setCursor(mp.display.cursor_x + 3, 10);
		if (mp.clockMinute < 10)
			mp.display.print("0");
		mp.display.print(mp.clockMinute);
		if(blinkState)
			mp.display.print(":");
		else
			mp.display.setCursor(mp.display.cursor_x + 3, 10);
		if (mp.clockSecond < 10)
			mp.display.print("0");
		mp.display.print(mp.clockSecond);

		mp.display.setCursor(40,30);
		if (mp.clockDay < 10)
			mp.display.print("0");
		mp.display.print(mp.clockDay);
		mp.display.print("/");

		if (mp.clockMonth < 10)
			mp.display.print("0");
		mp.display.print(mp.clockMonth);
		mp.display.print("/");
		mp.display.print(2000 + mp.clockYear);

		mp.display.setCursor( 0, 65);
		mp.display.printCenter("Edit time");
		mp.display.drawRect(46,63, 68, 20, TFT_BLACK);
		mp.display.setCursor( 40, 95);
		mp.display.printCenter("Force time sync");
		mp.display.drawRect(23, 93, 110, 20, TFT_BLACK);

		if(cursor == 0)
		{
			if(!blinkState)
				mp.display.drawRect(46,63, 68, 20, 0xFFED);
			if(mp.buttons.released(BTN_A))
			{
				osc->note(75, 0.05);
				osc->play();
				while(!mp.update());
				String inputBuffer;
				if(mp.clockHour == 0)
					inputBuffer = "";
				else
					inputBuffer = String(mp.clockHour);

				while(1)
				{
					mp.display.fillScreen(0xFFED);
					mp.display.setCursor(2, 98);
					mp.display.print("Press A to save");
					mp.display.setCursor(2, 110);
					mp.display.print("Press B to cancel");
					switch (editCursor)
					{
						case 0:
							mp.display.setCursor(52,10);
							if(inputBuffer == "")
								mp.display.cursor_x+=16;
							else if (inputBuffer.length() == 1)
							{
								mp.display.print("0"); mp.display.print(inputBuffer);
							}
							else
								mp.display.print(inputBuffer);
							if(blinkState)
								mp.display.drawFastVLine(mp.display.getCursorX() - 1, mp.display.getCursorY() + 3, 11, TFT_BLACK);

							mp.display.print(":");
							if (mp.clockMinute < 10)
								mp.display.print("0");
							mp.display.print(mp.clockMinute);
							mp.display.print(":");
							if (mp.clockSecond < 10)
								mp.display.print("0");
							mp.display.print(mp.clockSecond);

							mp.display.setCursor(40,30);
							if (mp.clockDay < 10)
								mp.display.print("0");
							mp.display.print(mp.clockDay);
							mp.display.print("/");
							if (mp.clockMonth < 10)
								mp.display.print("0");
							mp.display.print(mp.clockMonth);
							mp.display.print("/");
							mp.display.print(2000 + mp.clockYear);
							break;

						case 1:
							mp.display.setCursor(52,10);
							if(mp.clockHour < 10)
								mp.display.print("0");
							mp.display.print(mp.clockHour);
							mp.display.print(":");
							if(inputBuffer == "")
								mp.display.cursor_x+=16;
							else if (inputBuffer.length() == 1)
							{
								mp.display.print("0"); mp.display.print(inputBuffer);
							}
							else
								mp.display.print(inputBuffer);
							if(blinkState)
								mp.display.drawFastVLine(mp.display.getCursorX() - 1, mp.display.getCursorY() + 3, 11, TFT_BLACK);
							mp.display.print(":");
							if (mp.clockSecond < 10)
								mp.display.print("0");
							mp.display.print(mp.clockSecond);

							mp.display.setCursor(40,30);
							if (mp.clockDay < 10)
								mp.display.print("0");
							mp.display.print(mp.clockDay);
							mp.display.print("/");
							if (mp.clockMonth < 10)
								mp.display.print("0");
							mp.display.print(mp.clockMonth);
							mp.display.print("/");
							mp.display.print(2000 + mp.clockYear);
							break;

						case 2:
							mp.display.setCursor(52,10);
							if(mp.clockHour < 10)
								mp.display.print("0");
							mp.display.print(mp.clockHour);
							mp.display.print(":");
							if(mp.clockMinute < 10)
								mp.display.print("0");
							mp.display.print(mp.clockMinute);
							mp.display.print(":");
							if(inputBuffer == "")
								mp.display.cursor_x+=16;
							if (inputBuffer.length() == 1)
							{
								mp.display.print("0"); mp.display.print(inputBuffer);
							}
							else
								mp.display.print(inputBuffer);
							if(blinkState)
								mp.display.drawFastVLine(mp.display.getCursorX() - 1, mp.display.getCursorY() + 3, 11, TFT_BLACK);

							mp.display.setCursor(40,30);
							if (mp.clockDay < 10)
								mp.display.print("0");
							mp.display.print(mp.clockDay);
							mp.display.print("/");
							if (mp.clockMonth < 10)
								mp.display.print("0");
							mp.display.print(mp.clockMonth);
							mp.display.print("/");
							mp.display.print(2000 + mp.clockYear);
							break;

						case 3:
							mp.display.setCursor(52,10);
							if(mp.clockHour < 10)
								mp.display.print("0");
							mp.display.print(mp.clockHour);
							mp.display.print(":");
							if(mp.clockMinute < 10)
								mp.display.print("0");
							mp.display.print(mp.clockMinute);
							mp.display.print(":");
							if (mp.clockSecond < 10)
								mp.display.print("0");
							mp.display.print(mp.clockSecond);

							mp.display.setCursor(40,30);
							if(inputBuffer == "")
								mp.display.cursor_x+=16;
							else if (inputBuffer.length() == 1)
							{
								mp.display.print("0"); mp.display.print(inputBuffer);
							}
							else
								mp.display.print(inputBuffer);
							if(blinkState)
								mp.display.drawFastVLine(mp.display.getCursorX() - 1, mp.display.getCursorY() + 3, 11, TFT_BLACK);

							mp.display.print("/");
							if (mp.clockMonth < 10)
								mp.display.print("0");
							mp.display.print(mp.clockMonth);
							mp.display.print("/");
							mp.display.print(2000 + mp.clockYear);
							break;

						case 4:
							mp.display.setCursor(52,10);
							if(mp.clockHour < 10)
								mp.display.print("0");
							mp.display.print(mp.clockHour);
							mp.display.print(":");
							if(mp.clockMinute < 10)
								mp.display.print("0");
							mp.display.print(mp.clockMinute);
							mp.display.print(":");
							if (mp.clockSecond < 10)
								mp.display.print("0");
							mp.display.print(mp.clockSecond);

							mp.display.setCursor(40,30);
							if (mp.clockDay < 10)
								mp.display.print("0");
							mp.display.print(mp.clockDay);
							mp.display.print("/");
							if(inputBuffer == "")
								mp.display.cursor_x+=16;
							else if (inputBuffer.length() == 1)
							{
								mp.display.print("0"); mp.display.print(inputBuffer);
							}
							else
								mp.display.print(inputBuffer);
							if(blinkState)
								mp.display.drawFastVLine(mp.display.getCursorX() - 1, mp.display.getCursorY() + 3, 11, TFT_BLACK);

							mp.display.print("/");
							mp.display.print(2000 + mp.clockYear);
							break;

						case 5:
							mp.display.setCursor(52,10);
							if(mp.clockHour < 10)
								mp.display.print("0");
							mp.display.print(mp.clockHour);
							mp.display.print(":");
							if(mp.clockMinute < 10)
								mp.display.print("0");
							mp.display.print(mp.clockMinute);
							mp.display.print(":");
							if (mp.clockSecond < 10)
								mp.display.print("0");
							mp.display.print(mp.clockSecond);

							mp.display.setCursor(40,30);
							if (mp.clockDay < 10)
								mp.display.print("0");
							mp.display.print(mp.clockDay);
							mp.display.print("/");
							if (mp.clockMonth < 10)
								mp.display.print("0");
							mp.display.print(mp.clockMonth);
							mp.display.print("/");
							if(inputBuffer == "")
								mp.display.print(2000);
							else
								mp.display.print(2000 + inputBuffer.toInt());
							if(blinkState)
								mp.display.drawFastVLine(mp.display.getCursorX() - 1, mp.display.getCursorY() + 3, 11, TFT_BLACK);
							break;

					}
					key = mp.buttons.getKey();
					if (mp.buttons.released(BTN_FUN_RIGHT)) //clear number
					{
						inputBuffer = "";
						while(!mp.update());
					}
					else if (mp.buttons.released(BTN_FUN_LEFT))
					{
						inputBuffer.remove(inputBuffer.length() - 1);
						while(!mp.update());
					}
					if (key != NO_KEY && isdigit(key) && inputBuffer.length() < 2)
						inputBuffer += key;
					if(millis()-previousMillis >= 500)
					{
						previousMillis = millis();
						blinkState = !blinkState;
					}
					mp.update();
					if(mp.buttons.released(BTN_RIGHT) && editCursor < 5) //RIGHT
					{
						blinkState = 1;
						previousMillis = millis();
						while(!mp.update());
						if(inputBuffer == "")
						{
							switch (editCursor)
							{
								case 0:
									mp.clockHour = 0;
									break;
								case 1:
									mp.clockMinute = 0;
									break;
								case 2:
									mp.clockSecond = 0;
									break;
								case 3:
									mp.clockDay = 0;
									break;
								case 4:
									mp.clockMonth = 0;
									break;
								case 5:
									mp.clockYear = 0;
									break;
							}
						}
						else
						{
							switch (editCursor)
							{
								case 0:
									mp.clockHour = inputBuffer.toInt();
									break;
								case 1:
									mp.clockMinute = inputBuffer.toInt();
									break;
								case 2:
									mp.clockSecond = inputBuffer.toInt();
									break;
								case 3:
									mp.clockDay = inputBuffer.toInt();
									break;
								case 4:
									mp.clockMonth = inputBuffer.toInt();
									break;
								case 5:
									mp.clockYear = inputBuffer.toInt();
									break;
							}
						}
						switch (editCursor)
						{
							case 0:
								if(mp.clockMinute != 0)
									inputBuffer = String(mp.clockMinute);
								else
									inputBuffer = "";
								break;
							case 1:
								if(mp.clockSecond != 0)
									inputBuffer = String(mp.clockSecond);
								else
									inputBuffer = "";
								break;
							case 2:
								if(mp.clockDay != 0)
									inputBuffer = String(mp.clockDay);
								else
									inputBuffer = "";
								break;
							case 3:
								if(mp.clockMonth != 0)
									inputBuffer = String(mp.clockMonth);
								else
									inputBuffer = "";
								break;
							case 4:
								if(mp.clockYear != 0)
									inputBuffer = String(mp.clockYear);
								else
									inputBuffer = "";
								break;
						}
						editCursor++;

					}
					if(mp.buttons.released(BTN_LEFT) && editCursor > 0) //LEFT
					{
						while(!mp.update());
						blinkState = 1;
						previousMillis = millis();
						if(inputBuffer == "")
						{
							switch (editCursor)
							{
								case 0:
									mp.clockHour = 0;
									break;
								case 1:
									mp.clockMinute = 0;
									break;
								case 2:
									mp.clockSecond = 0;
									break;
								case 3:
									mp.clockDay = 0;
									break;
								case 4:
									mp.clockMonth = 0;
									break;
								case 5:
									mp.clockYear = 0;
									break;
							}
						}
						else
						{
							switch (editCursor)
							{
								case 0:
									mp.clockHour = inputBuffer.toInt();
									break;
								case 1:
									mp.clockMinute = inputBuffer.toInt();
									break;
								case 2:
									mp.clockSecond = inputBuffer.toInt();
									break;
								case 3:
									mp.clockDay = inputBuffer.toInt();
									break;
								case 4:
									mp.clockMonth = inputBuffer.toInt();
									break;
								case 5:
									mp.clockYear = inputBuffer.toInt();
									break;
							}
						}
						switch (editCursor)
						{
							case 1:
								if(mp.clockHour != 0)
									inputBuffer = String(mp.clockHour);
								else
									inputBuffer = "";
								break;
							case 2:
								if(mp.clockMinute != 0)
									inputBuffer = String(mp.clockMinute);
								else
									inputBuffer = "";
								break;
							case 3:
								if(mp.clockSecond != 0)
									inputBuffer = String(mp.clockSecond);
								else
									inputBuffer = "";
								break;
							case 4:
								if(mp.clockDay != 0)
									inputBuffer = String(mp.clockDay);
								else
									inputBuffer = "";
								break;
							case 5:
								if(mp.clockMonth != 0)
									inputBuffer = String(mp.clockMonth);
								else
									inputBuffer = "";
								break;
						}
						editCursor--;

					}

					if(mp.buttons.released(BTN_UP) && editCursor > 2) //UP
					{
						while(!mp.update());
						blinkState = 1;
						previousMillis = millis();
						if(inputBuffer == "")
						{
							switch (editCursor)
							{
								case 0:
									mp.clockHour = 0;
									break;
								case 1:
									mp.clockMinute = 0;
									break;
								case 2:
									mp.clockSecond = 0;
									break;
								case 3:
									mp.clockDay = 0;
									break;
								case 4:
									mp.clockMonth = 0;
									break;
								case 5:
									mp.clockYear = 0;
									break;
							}
						}
						else
						{
							switch (editCursor)
							{
								case 0:
									mp.clockHour = inputBuffer.toInt();
									break;
								case 1:
									mp.clockMinute = inputBuffer.toInt();
									break;
								case 2:
									mp.clockSecond = inputBuffer.toInt();
									break;
								case 3:
									mp.clockDay = inputBuffer.toInt();
									break;
								case 4:
									mp.clockMonth = inputBuffer.toInt();
									break;
								case 5:
									mp.clockYear = inputBuffer.toInt();
									break;
							}
						}
						switch (editCursor)
						{
							case 3:
								if(mp.clockHour != 0)
									inputBuffer = String(mp.clockHour);
								else
									inputBuffer = "";
								break;
							case 4:
								if(mp.clockMinute != 0)
									inputBuffer = String(mp.clockMinute);
								else
									inputBuffer = "";
								break;
							case 5:
								if(mp.clockSecond != 0)
									inputBuffer = String(mp.clockSecond);
								else
									inputBuffer = "";
								break;
						}
						editCursor-=3;
					}
					if(mp.buttons.released(BTN_DOWN) && editCursor < 3) //DOWN
					{
						while(!mp.update());
						blinkState = 1;
						previousMillis = millis();
						if(inputBuffer == "")
						{
							switch (editCursor)
							{
								case 0:
									mp.clockHour = 0;
									break;
								case 1:
									mp.clockMinute = 0;
									break;
								case 2:
									mp.clockSecond = 0;
									break;
								case 3:
									mp.clockDay = 0;
									break;
								case 4:
									mp.clockMonth = 0;
									break;
								case 5:
									mp.clockYear = 0;
									break;
							}
						}
						else
						{
							switch (editCursor)
							{
								case 0:
									mp.clockHour = inputBuffer.toInt();
									break;
								case 1:
									mp.clockMinute = inputBuffer.toInt();
									break;
								case 2:
									mp.clockSecond = inputBuffer.toInt();
									break;
								case 3:
									mp.clockDay = inputBuffer.toInt();
									break;
								case 4:
									mp.clockMonth = inputBuffer.toInt();
									break;
								case 5:
									mp.clockYear = inputBuffer.toInt();
									break;
							}
						}
						switch (editCursor)
						{
							case 0:
								if(mp.clockDay != 0)
									inputBuffer = String(mp.clockDay);
								else
									inputBuffer = "";
								break;
							case 1:
								if(mp.clockMonth != 0)
									inputBuffer = String(mp.clockMonth);
								else
									inputBuffer = "";
								break;
							case 2:
								if(mp.clockYear != 0)
									inputBuffer = String(mp.clockYear);
								else
									inputBuffer = "";
								break;
						}
						editCursor+=3;
					}

					if(mp.buttons.released(BTN_A))
					{
						if(inputBuffer == "")
						{
							switch (editCursor)
							{
								case 0:
									mp.clockHour = 0;
									break;
								case 1:
									mp.clockMinute = 0;
									break;
								case 2:
									mp.clockSecond = 0;
									break;
								case 3:
									mp.clockDay = 0;
									break;
								case 4:
									mp.clockMonth = 0;
									break;
								case 5:
									mp.clockYear = 0;
									break;
							}
						}
						else
						{
							switch (editCursor)
							{
								case 0:
									mp.clockHour = inputBuffer.toInt();
									break;
								case 1:
									mp.clockMinute = inputBuffer.toInt();
									break;
								case 2:
									mp.clockSecond = inputBuffer.toInt();
									break;
								case 3:
									mp.clockDay = inputBuffer.toInt();
									break;
								case 4:
									mp.clockMonth = inputBuffer.toInt();
									break;
								case 5:
									mp.clockYear = inputBuffer.toInt();
									break;
							}
						}
						DateTime now = DateTime(mp.clockYear, mp.clockMonth, mp.clockDay, 
							mp.clockHour, mp.clockMinute, mp.clockSecond);
						mp.RTC.adjust(now);
						break;
					}


					if(mp.buttons.released(BTN_B))
						break;

				}
				while(!mp.update());
			}
		}
		else if(cursor == 1)
		{
			if(!blinkState)
				mp.display.drawRect(23, 93, 110, 20, 0xFFED);
			if(mp.buttons.released(BTN_A))
			{
				osc->note(75, 0.05);
				osc->play();
				mp.clockYear = 0;
				previousMillis = millis();
				while(1)
				{
					mp.display.fillScreen(0xFFED);
					mp.display.setCursor(0, mp.display.height()/2 - 16);
					mp.display.printCenter("Fetching time...");
					if(millis() - previousMillis >= 4000)
					{
						mp.display.fillScreen(0xFFED);
						mp.display.setCursor(0, mp.display.height()/2 - 20);
						mp.display.printCenter("Couldn't fetch time");
						mp.display.setCursor(0, mp.display.height()/2);
						mp.display.printCenter("Set it manually");
						while(!mp.update());
						delay(2000);
						break;
					}
					mp.updateTimeGSM();
					if(mp.clockYear < 80 && mp.clockYear >= 19)
					{
						delay(200);
						mp.display.fillScreen(0xFFED);
						mp.display.setCursor(0, mp.display.height()/2 - 16);
						mp.display.printCenter("Time fetched over GSM!");
						while(!mp.update());
						delay(1500);
						break;
					}
				}


			}
		}


		if(millis()-previousMillis >= 500)
		{
			previousMillis = millis();
			blinkState = !blinkState;
			mp.updateTimeRTC();
		}
		if (mp.buttons.released(BTN_UP) && cursor > 0)
		{
			osc->note(75, 0.05);
			osc->play();
			blinkState = 1;
			previousMillis = millis() + 400;
			while (!mp.update());
			cursor--;
		}
		if (mp.buttons.released(BTN_DOWN) && cursor < 1)
		{
			osc->note(75, 0.05);
			osc->play();
			blinkState = 1;
			previousMillis = millis() + 400;
			while (!mp.update());
			cursor++;
		}
		if (mp.buttons.released(BTN_B)) //BUTTON BACK
			break;
		mp.update();
	}

}
bool updateMenu()
{
	bool blinkState = 0;
	uint32_t previousMillis = millis();
	uint8_t cursor = 0;
	String foo="";
	mp.display.setTextWrap(0);
	mp.display.setTextFont(2);

	while(1)
	{
		mp.display.fillScreen(0xFD29);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.setCursor(10, 20);
		mp.display.printCenter("Check for update");
		mp.display.setCursor(40, 50);
		mp.display.printCenter("Factory reset");
		mp.display.setCursor(90, 100);
		mp.display.setTextColor(TFT_DARKGREY);
		foo = "Version: " + (String)((int)mp.firmware_version / 100) + "." + (String)((int)mp.firmware_version / 10) + "." + (String)(mp.firmware_version % 10);
		mp.display.printCenter(foo);
		if(cursor == 0)
		{
			if(blinkState)
				mp.display.drawRect(20,18,117, 20, TFT_BLACK);
			else
				mp.display.drawRect(20,18,117, 20, 0xFD29);
			if(mp.buttons.released(BTN_A))
			{
				if(mp.wifi)
				{
					mp.display.setTextColor(TFT_BLACK);
					mp.display.setTextSize(1);
					mp.display.setTextFont(2);
					mp.display.drawRect(4, 49, 152, 28, TFT_BLACK);
					mp.display.drawRect(3, 48, 154, 30, TFT_BLACK);
					mp.display.fillRect(5, 50, 150, 26, 0xFD29);
					mp.display.setCursor(47, 54);
					mp.display.printCenter("Searching for networks");
					while(!mp.update());
					wifiConnect();
				}
				else
				{
					mp.display.setTextColor(TFT_BLACK);
					mp.display.setTextSize(1);
					mp.display.setTextFont(2);
					mp.display.drawRect(4, 49, 152, 28, TFT_BLACK);
					mp.display.drawRect(3, 48, 154, 30, TFT_BLACK);
					mp.display.fillRect(5, 50, 150, 26, 0xFD29);
					mp.display.setCursor(47, 54);
					mp.display.printCenter("Wifi turned off!");
					uint32_t tempMillis = millis();
					while(millis() < tempMillis + 2000)
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
				}
			}
		}
		else if (cursor == 1)
		{
			if(blinkState)
				mp.display.drawRect(30, 48, 96, 20, TFT_BLACK);
			else
				mp.display.drawRect(30, 48, 96, 20, 0xFD29);
			if(mp.buttons.released(BTN_A))
			{
				while(!mp.update());
				if(mp.SDinsertedFlag)
				{
					mp.display.fillScreen(0xFD29);
					mp.display.setTextColor(TFT_BLACK);
					mp.display.setCursor(10, 20);
					mp.display.printCenter("Erasing in progress...");
					while(!mp.update());

					String contacts_default = "[{\"name\":\"Foobar\", \"number\":\"099123123\"}]";
					String settings_default = "{ \"wifi\": 0, \"bluetooth\": 0, \"airplane_mode\": 0, \"brightness\": 5, \"sleep_time\": 0, \"background_color\": 0, \"notification\" : 0, \"ringtone\" : \"/Music/Default ringtone.wav\" }";

					const char contacts_path[] = "/.core/contacts.json";
					const char settings_path[] = "/.core/settings.json";

					JsonArray& contacts = mp.jb.parseArray(contacts_default);
					JsonObject& settings = mp.jb.parseObject(settings_default);

					mp.SD.remove(contacts_path);
					mp.SD.remove(settings_path);

					SDAudioFile contacts_file = mp.SD.open(contacts_path, "w");
					contacts.prettyPrintTo(contacts_file);
					contacts_file.close();

					SDAudioFile settings_file = mp.SD.open(settings_path, "w");
					settings.prettyPrintTo(settings_file);
					settings_file.close();

					mp.wifi = settings["wifi"];
					mp.bt = settings["bluetooth"];
					mp.airplaneMode = settings["airplane_mode"];
					mp.brightness = settings["brightness"];
					mp.sleepTime = settings["sleep_time"];
					mp.backgroundIndex = settings["background_color"];
					mp.notification = settings["notification"];
					mp.ringtone_path = String(settings["ringtone"].as<char*>());
					Serial.println("ERASED");
					delay(5);
					mp.applySettings();
				}
				else
				{
					mp.wifi = 1;
					mp.bt = 0;
					mp.airplaneMode = 0;
					mp.brightness = 5;
					mp.sleepTime = 0;
					mp.backgroundIndex = 0;
					mp.volume = 10;
					mp.ringtone_path = "/Music/Default ringtone.wav";
					mp.notification = 0;
					mp.applySettings();
				}
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextSize(1);
				mp.display.setTextFont(2);
				mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
				mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
				mp.display.fillRect(15, 46, 132, 36, 0xFD29);
				mp.display.setCursor(0, mp.display.height()/2 - 20);
				mp.display.setCursor(47, 55);
				mp.display.printCenter("Reset to defaults!");
				uint32_t tempMillis = millis();
				while(millis() < tempMillis + 2000)
				{
					mp.update();
					if(mp.buttons.pressed(BTN_A) || mp.buttons.pressed(BTN_B))
					{
						while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
							mp.update();
						break;
					}
				}
				return 1;

			}
		}
		if(millis()-previousMillis >= 250)
		{
			previousMillis = millis();
			blinkState = !blinkState;
		}
		if (mp.buttons.released(BTN_UP) && cursor > 0)
		{
			osc->note(75, 0.05);
			osc->play();
			blinkState = 1;
			previousMillis = millis();
			while (!mp.update());
			cursor--;
		}
		if (mp.buttons.released(BTN_DOWN) && cursor < 1)
		{
			osc->note(75, 0.05);
			osc->play();
			blinkState = 1;
			previousMillis = millis();
			while (!mp.update());
			cursor++;
		}
		if (mp.buttons.released(BTN_B)) //BUTTON BACK
			break;
		mp.update();
	}
	while(!mp.update());
	return false;
}
int8_t notificationsAudioMenu(String* items, uint8_t length) {
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	uint8_t scale = 2;
	uint8_t offset = 23;
	uint8_t boxHeight = 20;
	int16_t cursor = mp.notification;
	if (length > 12) {
		cameraY = -cursor * (boxHeight + 2) - 1;
	}
	while (1) {

		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, 0);
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
		}

		for (uint8_t i = 0; i < length; i++)
			notificationsDrawBox(items[i], i, cameraY_actual);

		notificationsDrawCursor(cursor, cameraY_actual);

		mp.display.fillRect(0, 0, mp.display.width(), 20, TFT_DARKGREY);
		mp.display.setTextFont(2);
		mp.display.setCursor(2,1);
		mp.display.drawFastHLine(0, 19, mp.display.width(), TFT_WHITE);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Notifications");
		mp.display.setCursor(130,110);
		mp.display.print("Play");
		if(mp.released(BTN_FUN_RIGHT))
		{
			while(!mp.update());
			mp.playNotificationSound(cursor);
		}

		if (mp.buttons.released(BTN_A)) {   //BUTTON CONFIRM
			while (!mp.update());
			break;
		}
		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP

			while (!mp.update());
			if (cursor == 0) {
				cursor = length - 1;
				if (length > 6*scale) {
					cameraY = -(cursor - 5) * (boxHeight + 2) - 1;
				}
			}
			else {
				if (cursor > 0 && (cursor * (boxHeight + 2) - 1 + cameraY + offset) <= boxHeight) {
					cameraY += (boxHeight + 2);
				}
				cursor--;
			}

		}

		if (mp.buttons.released(BTN_DOWN)) { //BUTTON DOWN
			while (!mp.update());
			cursor++;
			if ((cursor * (boxHeight + 2) + cameraY + offset) > 54*scale) {
				cameraY -= (boxHeight + 2);
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

void notificationsDrawCursor(uint8_t i, int32_t y) {
	uint8_t offset;
	uint8_t boxHeight;
	offset = 23;
	boxHeight = 20;
	if (millis() % 500 <= 250) {
		return;
	}
	y += i * (boxHeight + 2) + offset;
	mp.display.drawRect(1, y, mp.display.width() - 2, boxHeight + 1, TFT_RED);
	mp.display.drawRect(0, y-1, mp.display.width(), boxHeight + 3, TFT_RED);
}
void notificationsDrawBox(String text, uint8_t i, int32_t y) {
	uint8_t offset;
	uint8_t boxHeight;
	offset = 23;
	boxHeight = 20;
	y += i * (boxHeight + 2) + offset;
	if (y < 0 || y > mp.display.height()) {
		return;
	}
	mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight - 1, TFT_DARKGREY);
	mp.display.setTextColor(TFT_WHITE);
	mp.display.drawString(text, 5, y+2);
}
void wifiConnect()
{
	if(!mp.wifi)
	{
		mp.display.setTextColor(TFT_BLACK);
		mp.display.setTextSize(1);
		mp.display.setTextFont(2);
		mp.display.drawRect(4, 49, 152, 28, TFT_BLACK);
		mp.display.drawRect(3, 48, 154, 30, TFT_BLACK);
		mp.display.fillRect(5, 50, 150, 26, 0xFD29);
		mp.display.setCursor(47, 54);
		mp.display.printCenter("No networks found!");
		uint32_t tempMillis = millis();
		while(millis() < tempMillis + 2000)
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
	}
	bool blinkState = 1;
	unsigned long elapsedMillis = millis();
	String content = ""; //password string
	String prevContent = "";
	delay(1000);
	int n = WiFi.scanNetworks();
	delay(1000);
	Serial.println("scan done");
	Serial.println(n);
	if (n < 1) {
		mp.display.setTextColor(TFT_BLACK);
		mp.display.setTextSize(1);
		mp.display.setTextFont(2);
		mp.display.drawRect(4, 49, 152, 28, TFT_BLACK);
		mp.display.drawRect(3, 48, 154, 30, TFT_BLACK);
		mp.display.fillRect(5, 50, 150, 26, 0xFD29);
		mp.display.setCursor(47, 54);
		mp.display.printCenter("No networks found!");
		uint32_t tempMillis = millis();
		while(millis() < tempMillis + 2000)
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

	}
	else 
	{
		String networkNames[n];
		String wifiSignalStrengths[n];
		bool wifiPasswordNeeded[n];
		Serial.print(n);
		Serial.println(" networks found");
		for (int i = 0; i < n; ++i) {
			// Print SSID and RSSI for each network found
			networkNames[i] = WiFi.SSID(i);
			wifiSignalStrengths[i] = WiFi.RSSI(i);
			wifiPasswordNeeded[i] = !(WiFi.encryptionType(i) == WIFI_AUTH_OPEN);
			Serial.print(i + 1);
			Serial.print(": ");
			Serial.print(WiFi.SSID(i));
			Serial.print(" (");
			Serial.print(WiFi.RSSI(i));
			Serial.print(")");
			Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
			delay(10);
		}
		int8_t selection = wifiNetworksMenu(networkNames, wifiSignalStrengths, n);
		if(selection < 0)
		{
			while(!mp.update());
			return;
		}
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(8, 8);
		mp.display.printCenter(networkNames[selection]);
		mp.display.setCursor(4, 30);
		mp.display.printCenter("Enter password:");
		while (!mp.update());
		mp.display.setCursor(1, 112);
		mp.display.print("Erase");
		mp.display.setCursor(110, 112);
		mp.display.print("Confirm");
		if (wifiPasswordNeeded[selection])
		{
			while (1)
			{
				if (millis() - elapsedMillis >= multi_tap_threshold) //cursor blinking routine 
				{
					elapsedMillis = millis();
					blinkState = !blinkState;
				}

				mp.display.setTextFont(2);
				mp.display.fillRect(1, 55, mp.display.width(), 20, TFT_DARKGREY);
				mp.display.setTextColor(TFT_WHITE);
				mp.display.setCursor(1, 6);
				prevContent = content;
				content = mp.textInput(content, 18);
				if (prevContent != content)
				{
					blinkState = 1;
					elapsedMillis = millis();
				}

				mp.display.setTextWrap(1);
				mp.display.setCursor(1, 56);
				mp.display.setTextFont(2);
				mp.display.printCenter(content);
				if (blinkState == 1)
					mp.display.drawFastVLine(mp.display.getCursorX(), mp.display.getCursorY(), 16, TFT_WHITE);
				
				if((mp.buttons.released(BTN_A) || mp.buttons.released(BTN_FUN_RIGHT)) && content.length() > 0)
				{
					content = "MAKERphone!"; //just for debug purposes

					mp.display.setCursor(20, 50);
					mp.display.fillRect(0, 28, 160, 100, TFT_BLACK);
					mp.display.setCursor(0,40);
					mp.display.printCenter("Connecting");
					mp.display.setCursor(60, 60);
					while (!mp.update());

					char temp[networkNames[selection].length()+1];
					char temp2[content.length()];
					networkNames[selection].toCharArray(temp, networkNames[selection].length()+1);
					content.toCharArray(temp2, content.length()+1);
					WiFi.begin(temp, temp2);
					uint8_t counter = 0;

					while (WiFi.status() != WL_CONNECTED) 
					{
						delay(500);
						mp.display.print(".");
						while (!mp.update());
						counter++;
						if (counter >= 5)
						{
							mp.display.fillRect(0, 40, mp.display.width(), 20, TFT_BLACK);
							mp.display.setCursor(0, 45);
							mp.display.printCenter("Wrong password :(");
							uint32_t tempMillis = millis();
							while(millis() < tempMillis + 2000)
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
					if(WiFi.status() != WL_CONNECTED)
						break;

					int8_t selection = checkForUpdate();
					if(selection == 1)
					{
						mp.display.fillScreen(TFT_BLACK);
						mp.display.setCursor(0,mp.display.height() / 2 - 16);
						mp.display.printCenter(F("Downloading update"));
						while(!mp.update());
						fetchUpdate();
						mp.display.fillScreen(TFT_BLACK);
						mp.display.setCursor(0,mp.display.height() / 2 - 16);
						mp.display.printCenter(F("Installing update"));
						while(!mp.update());
						mp.updateFromFS("/.core/LOADER.BIN");
					}

					else if(selection == 0)
					{
						mp.display.fillRect(0, 40, mp.display.width(), 20, TFT_BLACK);
						mp.display.setCursor(0, 45);
						mp.display.printCenter("No updates available");
						uint32_t tempMillis = millis();
						while(millis() < tempMillis + 2000)
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

					else
						break;
					// while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
					// {
					// 	mp.display.fillScreen(TFT_BLACK);
					// 	mp.display.setTextFont(2);
					// 	mp.display.setCursor(70,70);
					// 	mp.display.printCenter("WIFI success!!!");
					// 	mp.update();
					// }
				}
				if(mp.buttons.released(BTN_B))
					break;
				mp.update();
			}
		}
		else
		{
			while (WiFi.status() != WL_CONNECTED)
				mp.update();
			mp.display.fillScreen(TFT_BLACK);
			mp.display.setCursor(30, 32);
			mp.display.printCenter("CONNECTED!");
			while (!mp.update());
		}
	}
}
int8_t checkForUpdate()
{
	if(WiFi.status() != WL_CONNECTED)
		return 0;
	HTTPClient http;
	const char* ca = \ 
	"-----BEGIN CERTIFICATE-----\n" \  
	"MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \  
	"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \  
	"DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\n" \  
	"PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
	"Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
	"AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
	"rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
	"OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
	"xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
	"7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
	"aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
	"HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
	"SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
	"ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
	"AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
	"R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
	"JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
	"Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
	"-----END CERTIFICATE-----\n";

	Serial.print("[HTTP] begin...\n");
	// configure traged server and url
	http.begin("https://raw.githubusercontent.com/CircuitMess/MAKERphone-firmware/master/README.md", ca); //HTTPS
	//http.begin("http://example.com/index.html"); //HTTP

	Serial.print("[HTTP] GET...\n");
	// start connection and send HTTP header
	int httpCode = http.GET();

	// httpCode will be negative on error
	if (httpCode > 0) {
		// HTTP header has been send and Server response header has been handled
		Serial.printf("[HTTP] GET... code: %d\n", httpCode);

		// file found at server
		if (httpCode == HTTP_CODE_OK) {
			String payload = http.getString();
			http.end();
			uint16_t version = payload.substring(payload.indexOf("version=") + 8, payload.indexOf("\r")).toInt();
			String foo = mp.readFile("/.core/settings.json");
			Serial.println(foo);
			//uint16_t old_version = foo.substring(foo.indexOf("version=") + 8, foo.indexOf("\n")).toInt();
			Serial.println(version);
			if (version > mp.firmware_version)
			{
				String foo = String("Version: " + String((int)version/100) + "." + String((int)version/10)
					+ "." + String(version%10));
				Serial.println(foo);
				while(!mp.buttons.released(BTN_A))
				{
					mp.display.fillScreen(TFT_BLACK);
					mp.display.setTextFont(2);
					mp.display.setTextSize(1);
					mp.display.setTextColor(TFT_WHITE);
					mp.display.setCursor(0,mp.display.height()/2 - 26);
					if(millis() % 1000 <= 500)
						mp.display.printCenter("UPDATE AVAILABLE!");
					mp.display.setCursor(70, 70);
					mp.display.printCenter(foo);
					mp.display.setCursor(4,110);
					mp.display.print("Press A to start update");
					if(mp.buttons.released(BTN_B))
					{
						while(!mp.update());
						return -1;
					}
					mp.update();
				}
				EEPROM.write(FIRMWARE_VERSION_ADDRESS, version);
				EEPROM.commit();
				return 1;
			}
			return 0;
		}
	}
}
bool fetchUpdate()
{
	if(WiFi.status() != WL_CONNECTED)
		return 0;
	HTTPClient http;
	const char* ca = \ 
	"-----BEGIN CERTIFICATE-----\n" \  
	"MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \  
	"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \  
	"DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\n" \  
	"PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
	"Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
	"AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
	"rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
	"OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
	"xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
	"7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
	"aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
	"HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
	"SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
	"ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
	"AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
	"R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
	"JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
	"Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
	"-----END CERTIFICATE-----\n";
	mp.SD.remove("/.core/LOADER.BIN");
	SDAudioFile file = mp.SD.open("/.core/LOADER.BIN", "w");
	Serial.print("[HTTP] begin...\n");
	// configure traged server and url
	http.begin("https://raw.githubusercontent.com/CircuitMess/MAKERphone-firmware/master/firmware.bin", ca); //HTTPS
	//http.begin("http://example.com/index.html"); //HTTP

	Serial.print("[HTTP] GET...\n");
	// start connection and send HTTP header
	int httpCode = http.GET();

	// httpCode will be negative on error
	if (httpCode > 0) {
		// HTTP header has been send and Server response header has been handled
		Serial.printf("[HTTP] GET... code: %d\n", httpCode);

		// file found at server
		if (httpCode == HTTP_CODE_OK) 
		{
			http.writeToStream(&file);  //for large files
			http.end();
			file.close();
			return 1;
		}
	}
	else
		return 0;

}
int8_t wifiNetworksMenu(String* items, String *signals, uint8_t length) {
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	uint8_t offset = 22;
	uint8_t boxHeight = 20;
	int16_t cursor = 0;
	if (length > 12) {
		cameraY = -cursor * (boxHeight + 2) - 1;
	}
	while (1) {

		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, 0);
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
		}

		for (uint8_t i = 0; i < length; i++)
			wifiDrawBox(items[i], signals[i], i, cameraY_actual);

		wifiDrawCursor(cursor, cameraY_actual);

		mp.display.fillRect(0, 0, mp.display.width(), 20, TFT_DARKGREY);
		mp.display.setTextFont(2);
		mp.display.setCursor(2,1);
		mp.display.drawFastHLine(0, 19, mp.display.width(), TFT_WHITE);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Networks");
		if(mp.released(BTN_FUN_RIGHT))
		{
			while(!mp.update());
			mp.playNotificationSound(cursor);
		}

		if (mp.buttons.released(BTN_A)) {   //BUTTON CONFIRM
			while (!mp.update());
			break;
		}
		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP

			while (!mp.update());
			if (cursor == 0) {
				cursor = length - 1;
				if (length > 4) {
					cameraY = -(cursor - 5) * (boxHeight + 2) - 1;
				}
			}
			else {
				if (cursor > 0 && (cursor * (boxHeight + 2) - 1 + cameraY + offset) <= boxHeight) {
					cameraY += (boxHeight + 2);
				}
				cursor--;
			}

		}

		if (mp.buttons.released(BTN_DOWN)) { //BUTTON DOWN
			while (!mp.update());
			cursor++;
			if ((cursor * (boxHeight + 2) + cameraY + offset) > 100) {
				cameraY -= (boxHeight + 2);
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
void wifiDrawBox(String text, String signalStrength, uint8_t i, int32_t y) {
	uint8_t offset = 23;
	uint8_t boxHeight = 20;
	y += i * (boxHeight + 2) + offset;
	if (y < 0 || y > mp.display.height()) {
		return;
	}
	mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight - 1, TFT_DARKGREY);
	mp.display.setTextColor(TFT_WHITE);
	mp.display.drawString(text, 5, y+2);
	int strength = signalStrength.toInt();
	//> -50 full
	// < -40 && > -60 high
	// < -60 && > -85 low
	// < -95 nosignal
	if(strength > -50)
		mp.display.drawBitmap(140, y+2, signalFullIcon, TFT_WHITE, 2);
	else if(strength <= -50 && strength > -70)
		mp.display.drawBitmap(140, y+2, signalHighIcon, TFT_WHITE, 2);
	else if(strength <= -70 && strength > -95)
		mp.display.drawBitmap(140, y+2, signalLowIcon, TFT_WHITE, 2);
	else if(strength <= -95)
		mp.display.drawBitmap(140, y+2, noSignalIcon, TFT_WHITE, 2);
}
void wifiDrawCursor(uint8_t i, int32_t y) {
	uint8_t offset = 23;
	uint8_t boxHeight = 20;
	if (millis() % 500 <= 250) {
		return;
	}
	y += i * (boxHeight + 2) + offset;
	mp.display.drawRect(1, y, mp.display.width() - 2, boxHeight + 1, TFT_RED);
	mp.display.drawRect(0, y-1, mp.display.width(), boxHeight + 3, TFT_RED);
}