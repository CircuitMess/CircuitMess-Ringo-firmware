#include "settingsApp.h"
uint16_t pinNumber = 1234;
uint8_t timesRemaining = 0;
boolean colorSetup = 0;
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
	bool blinkState = 0;
	uint32_t blinkMillis = millis();
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
			settingsMenuDrawBox(title[i], i, cameraY_actual);
		}
		if(blinkState)
			settingsMenuDrawCursor(cursor, cameraY_actual, pressed);

		if (mp.buttons.timeHeld(BTN_DOWN) == 0 && mp.buttons.timeHeld(BTN_UP) == 0)
			pressed = 0;

		if (mp.buttons.released(BTN_A)) {   //BUTTON CONFIRM
			mp.osc->note(75, 0.05);
			mp.osc->play();

			while(!mp.update());// Exit when pressed
			break;
		}
		if(mp.buttons.released(BTN_HOME)) {
			mp.exitedLockscreen = true;
			mp.lockscreen();
		}


		if (mp.buttons.pressed(BTN_UP)) {  //BUTTON UP
			blinkState = 1;
			blinkMillis = millis();
			mp.osc->note(75, 0.05);
			mp.osc->play();
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

		if (mp.buttons.pressed(BTN_DOWN)) { //BUTTON DOWN
			blinkState = 1;
			blinkMillis = millis();
			mp.osc->note(75, 0.05);
			mp.osc->play();
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
	while(!mp.update());
	while (1)
	{
		input = settingsMenu(settingsItems, 6, input);
		if (input == -1) //BUTTON BACK
			break;
		if (input == 0)
			networkMenu();
		if (input == 1)
			displayMenu();
		if (input == 2){
			colorSetup = 1;
			timeMenu();
		}
		if (input == 3)
			soundMenu();
		if (input == 4)
			securityMenu();
		if (input == 5)
			if(updateMenu())
				return true;
	}

	mp.display.fillScreen(TFT_BLACK);
	mp.display.setTextColor(TFT_WHITE);
	mp.display.setTextFont(2);
	mp.display.setCursor(0, mp.display.height()/2 - 22);
	mp.display.printCenter("Applying settings...");
	mp.display.setCursor(0, mp.display.height()/2 - 2);
	mp.display.printCenter("Please wait");
	while(!mp.update());
	mp.applySettings();
	mp.display.fillScreen(TFT_BLACK);
	if(mp.SDinsertedFlag)
	{
		mp.saveSettings(0);
		mp.display.setCursor(0, mp.display.height()/2 - 16);
		mp.display.setTextFont(2);
		mp.display.printCenter("Settings saved!");
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
	else
	{
		mp.display.setCursor(0, mp.display.height()/2 - 20);
		mp.display.setTextFont(2);
		mp.display.printCenter("Can't save - No SD!");
		mp.display.setCursor(0, mp.display.height()/2);
		mp.display.printCenter("Insert SD card and reset");
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
	return false;

}
void networkMenu() {
	uint8_t cursor = 0;
	bool blinkState = 0;
	uint32_t blinkMillis = millis();
	uint16_t color = 0;
	mp.dataRefreshFlag = 1;
	while (1)
	{
		// Serial.println(mp.airplaneMode);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.fillScreen(0xFB6D);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		// mp.display.setCursor(18, 20);
		// mp.display.print("Wifi");
		// mp.display.setCursor(22, 58);
		// mp.display.print("BT");
		// mp.display.setCursor(16, 88);
		mp.display.setCursor(10,15);
		mp.display.printCenter("Airplane mode");
		mp.display.drawBitmap(12, 15, airplaneModeIcon, TFT_BLACK, 2);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(47, 39);
		mp.display.print("ON");
		mp.display.setCursor(90, 39);
		mp.display.print("OFF");
		if(mp.airplaneMode || !mp.simInserted)
			color = TFT_DARKGREY;
		else
			color = TFT_BLACK;
		mp.display.drawBitmap(12, 75, signalHighIcon, color, 2);
		mp.display.setCursor(75,75);
		mp.display.setTextColor(color);
		mp.display.printCenter("Signal strength");
		mp.display.setCursor(95, 95);
		mp.display.printCenter(mp.signalStrength);
		// mp.display.setCursor(79, 57);
		// mp.display.print("ON");
		// mp.display.setCursor(122, 57);
		// mp.display.print("OFF");
		// mp.display.setCursor(79, 95);
		// mp.display.print("ON");
		// mp.display.setCursor(122, 95);
		// mp.display.print("OFF");
		if(mp.airplaneMode)
			mp.display.drawRect(38, 36, 34, 22, blinkState ? TFT_BLACK : 0xFB6D);
		else
			mp.display.drawRect(82, 36, 40, 22, blinkState ? TFT_BLACK : 0xFB6D);

		// switch (cursor) {
		
		// case 0:
		// 	if (mp.bt == 1)
		// 	{
		// 		mp.display.drawRect(35*2, 27*2, 17*2, 11*2, TFT_BLACK);
		// 	}
		// 	else
		// 	{
		// 		mp.display.drawRect(57*2, 27*2, 20*2, 11*2, TFT_BLACK);
		// 	}
		// 	if (mp.airplaneMode == 1)
		// 	{
		// 		mp.display.drawRect(35*2, 46*2, 17*2, 11*2, TFT_BLACK);
		// 	}
		// 	else
		// 	{
		// 		mp.display.drawRect(57*2, 46*2, 20*2, 11*2, TFT_BLACK);
		// 	}
		// 	break;

		// case 1:
		// 	if (mp.wifi == 1)
		// 	{
		// 		mp.display.drawRect(35*2, 8*2, 17*2, 11*2, TFT_BLACK);
		// 	}
		// 	else
		// 	{
		// 		mp.display.drawRect(57*2, 8*2, 20*2, 11*2, TFT_BLACK);
		// 	}

		// 	if (mp.airplaneMode == 1)
		// 	{
		// 		mp.display.drawRect(35*2, 46*2, 17*2, 11*2, TFT_BLACK);
		// 	}
		// 	else
		// 	{
		// 		mp.display.drawRect(57*2, 46*2, 20*2, 11*2, TFT_BLACK);
		// 	}
		// 	break;

		// case 2:
		// 	if (mp.wifi == 1)
		// 	{
		// 		mp.display.drawRect(35*2, 8*2, 17*2, 11*2, TFT_BLACK);
		// 	}
		// 	else
		// 	{
		// 		mp.display.drawRect(57*2, 8*2, 20*2, 11*2, TFT_BLACK);
		// 	}

		// 	if (mp.bt == 1)
		// 	{
		// 		mp.display.drawRect(35*2, 27*2, 17*2, 11*2, TFT_BLACK);
		// 	}
		// 	else
		// 	{
		// 		mp.display.drawRect(57*2, 27*2, 20*2, 11*2, TFT_BLACK);
		// 	}
		// }




		// if (cursor == 0)
		// {
		// 	if (blinkState && mp.wifi)
		// 	{
		// 		mp.display.drawRect(35*2, 8*2, 17*2, 11*2, TFT_BLACK);
		// 	}
		// 	else if (blinkState && !mp.wifi)
		// 	{
		// 		mp.display.drawRect(57*2, 8*2, 20*2, 11*2, TFT_BLACK);
		// 	}
		// 	if (mp.buttons.released(BTN_LEFT) && !mp.wifi)
		// 	{
		// 		mp.update();
		// 		mp.osc->note(75, 0.05);
		// 		mp.osc->play();
		// 		blinkState = 1;
		// 		blinkMillis = millis();
		// 		mp.wifi = !mp.wifi;
		// 	}
		// 	if (mp.buttons.released(BTN_RIGHT) && mp.wifi)
		// 	{
		// 		mp.update();
		// 		mp.osc->note(75, 0.05);
		// 		mp.osc->play();
		// 		blinkState = 1;
		// 		blinkMillis = millis();
		// 		mp.wifi = !mp.wifi;
		// 	}
		// }
		// if (cursor == 1)
		// {
		// 	if (blinkState && mp.bt)
		// 	{
		// 		mp.display.drawRect(35*2, 27*2, 17*2, 11*2, TFT_BLACK);
		// 	}
		// 	else if (blinkState && !mp.bt)
		// 	{
		// 		mp.display.drawRect(57*2, 27*2, 20*2, 11*2, TFT_BLACK);
		// 	}
		// 	if (mp.buttons.released(BTN_LEFT) && !mp.bt)
		// 	{
		// 		mp.update();
		// 		mp.osc->note(75, 0.05);
		// 		mp.osc->play();
		// 		blinkState = 1;
		// 		blinkMillis = millis();
		// 		mp.bt = !mp.bt;
		// 	}
		// 	if (mp.buttons.released(BTN_RIGHT) && mp.bt)
		// 	{
		// 		mp.update();
		// 		mp.osc->note(75, 0.05);
		// 		mp.osc->play();
		// 		blinkState = 1;
		// 		blinkMillis = millis();
		// 		mp.bt = !mp.bt;
		// 	}
		// }
		// if (cursor == 2)
		// {
		// 	if (blinkState && mp.airplaneMode)
		// 	{
		// 		mp.display.drawRect(35*2, 46*2, 17*2, 11*2, TFT_BLACK);
		// 	}
		// 	else if (blinkState && !mp.airplaneMode)
		// 	{
		// 		mp.display.drawRect(57*2, 46*2, 20*2, 11*2, TFT_BLACK);
		// 	}
		// 	if (mp.buttons.released(BTN_LEFT) && !mp.airplaneMode)
		// 	{
		// 		mp.update();
		// 		mp.osc->note(75, 0.05);
		// 		mp.osc->play();
		// 		blinkState = 1;
		// 		blinkMillis = millis();
		// 		mp.airplaneMode = !mp.airplaneMode;
		// 	}
		// 	if (mp.buttons.released(BTN_RIGHT) && mp.airplaneMode)
		// 	{
		// 		mp.update();
		// 		mp.osc->note(75, 0.05);
		// 		mp.osc->play();
		// 		blinkState = 1;
		// 		blinkMillis = millis();
		// 		mp.airplaneMode = !mp.airplaneMode;
		// 	}
		// }

		// if (mp.buttons.released(BTN_UP))
		// {
		// 	mp.osc->note(75, 0.05);
		// 	mp.osc->play();
		// 	blinkState = 0;
		// 	blinkMillis = millis();
		// 	mp.update();
		// 	if (cursor == 0)
		// 		cursor = 2;
		// 	else
		// 		cursor--;
		// }
		// if (mp.buttons.released(BTN_DOWN))
		// {
		// 	mp.osc->note(75, 0.05);
		// 	mp.osc->play();
		// 	blinkState = 0;
		// 	blinkMillis = millis();
		// 	mp.update();
		// 	if (cursor == 2)
		// 		cursor = 0;
		// 	else
		// 		cursor++;

		// }
		if((mp.buttons.pressed(BTN_LEFT) && !mp.airplaneMode)
		|| (mp.buttons.pressed(BTN_RIGHT) && mp.airplaneMode))
			mp.airplaneMode = !mp.airplaneMode;

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
	mp.dataRefreshFlag = 0;
}
void displayMenu() 
{
	mp.display.setTextFont(1);
	mp.display.setTextColor(TFT_BLACK);
	uint8_t sleepTimeBuffer = mp.sleepTime;
	uint16_t sleepTimeActualBuffer = mp.sleepTimeActual;
	uint8_t cursor = 0;
	bool blinkState = 0;
	uint32_t blinkMillis = millis();
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
		if(millis() - blinkMillis > 350)
		{
			blinkMillis = millis();
			blinkState = !blinkState;
		}
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
		switch(cursor)
		{
			case 0:
				if (blinkState)
				{
					mp.display.drawBitmap(12, 20, noBrightness, TFT_BLACK, 2);
					mp.display.drawBitmap(132, 14, fullBrightness, TFT_BLACK, 2);

				}
				else
				{
					mp.display.drawBitmap(12, 20, noBrightness, 0x8FEA, 2);
					mp.display.drawBitmap(132, 14, fullBrightness, 0x8FEA, 2);
				}
				if (mp.buttons.pressed(BTN_LEFT) && mp.brightness > 0)
				{
					mp.osc->note(75, 0.05);
					mp.osc->play();
					mp.brightness--;
					while(!mp.update());
				}
				if (mp.buttons.pressed(BTN_RIGHT) && mp.brightness < 5)
				{
					mp.osc->note(75, 0.05);
					mp.osc->play();
					mp.brightness++;
					while(!mp.update());
				}
			break;
			case 1:
				for(int i = 0;i<8;i++)
					mp.leds[i] = CRGB::White;
				if (mp.buttons.pressed(BTN_LEFT) && mp.pixelsBrightness > 0)
				{
					mp.buttons.update();
					mp.osc->note(75, 0.05);
					mp.osc->play();
					mp.pixelsBrightness--;
				}
				if (mp.buttons.pressed(BTN_RIGHT) && mp.pixelsBrightness < 5)
				{
					mp.buttons.update();
					mp.osc->note(75, 0.05);
					mp.osc->play();
					mp.pixelsBrightness++;
				}
			break;
			case 2:
				if(blinkState)
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
				if (mp.buttons.pressed(BTN_LEFT) && sleepTimeBuffer!= 0)
				{
					mp.osc->note(75, 0.05);
					mp.osc->play();
					sleepTimeBuffer--;
					while(!mp.update());
				}
				if (mp.buttons.pressed(BTN_RIGHT) && sleepTimeBuffer!= 5)
				{
					mp.osc->note(75, 0.05);
					mp.osc->play();
					sleepTimeBuffer++;
					while(!mp.update());
				}
			break;
			case 3:
				if(blinkState)
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
				if (mp.buttons.pressed(BTN_LEFT) && mp.backgroundIndex != 0)
				{
					mp.osc->note(75, 0.05);
					mp.osc->play();
					mp.backgroundIndex--;
					while(!mp.update());
				}
				if (mp.buttons.pressed(BTN_RIGHT) && mp.backgroundIndex != 6)
				{
					mp.osc->note(75, 0.05);
					mp.osc->play();
					mp.backgroundIndex++;
					while(!mp.update());
				}
			break;
		}
		if (mp.buttons.pressed(BTN_UP))
		{
			blinkState = 1;
			blinkMillis = millis();
			mp.osc->note(75, 0.05);
			mp.osc->play();
			while(!mp.update());
  			if (cursor == 0)
				cursor = 3;
			else
				cursor--;
		}
		if (mp.buttons.pressed(BTN_DOWN))
		{
			blinkState = 1;
			blinkMillis = millis();
			mp.osc->note(75, 0.05);
			mp.osc->play();
			while(!mp.update());
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
void soundMenu() 
{
	uint8_t currentScreen = 0;

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
	uint16_t start = 0;
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
	uint8_t micDraw = 0;
	while (1)
	{
		if(currentScreen == 0) //SCREEN 1
		{
			mp.display.setTextFont(2);
			mp.display.setTextSize(1);
			mp.display.setTextColor(TFT_BLACK);
			mp.display.fillScreen(0xA7FF);
			mp.display.setCursor(20, 8);
			mp.display.printCenter("Ring volume");
			mp.display.drawRect(36, 25, 88, 8, TFT_BLACK);
			mp.display.fillRect(38, 27, mp.ringVolume * 6, 4, TFT_BLACK);
			mp.display.drawBitmap(8, 20, noSound, TFT_BLACK, 2);
			mp.display.drawBitmap(134, 20, fullSound, TFT_BLACK, 2);
			

			mp.display.setCursor(29, 43);
			mp.display.printCenter("Media volume");
			mp.display.drawRect(36, 58, 88, 8, TFT_BLACK);
			mp.display.fillRect(38, 60, mp.mediaVolume * 6, 4, TFT_BLACK);
			mp.display.drawBitmap(8, 54, noSound, TFT_BLACK, 2);
			mp.display.drawBitmap(134, 54, fullSound, TFT_BLACK, 2);

			mp.display.setCursor(29, 75);
			mp.display.printCenter("Mic sensitivity");
			mp.display.drawRect(36, 92, 88, 8, TFT_BLACK);
			if(mp.micGain > 14)
				micDraw = 14;
			else
				micDraw = mp.micGain;
			mp.display.fillRect(38, 94, micDraw * 6, 4, TFT_BLACK);
			mp.display.drawBitmap(1, 94, micMinus, TFT_BLACK, 2);
			mp.display.drawBitmap(125, 88, micPlus, TFT_BLACK, 2);
			mp.display.drawBitmap(66, 114, arrowDown, TFT_BLACK, 2);
		}
		else if(currentScreen == 1) //SCREEN 2
		{

			mp.display.setTextFont(2);
			mp.display.setTextSize(1);
			mp.display.setTextColor(TFT_BLACK);
			mp.display.fillScreen(0xA7FF);

			mp.display.drawBitmap(66, 5, arrowUp, TFT_BLACK, 2);

			mp.display.setCursor(15, 30);
			mp.display.printCenter("Ringtone");
			mp.display.drawRect(6, 47, 148, 17, TFT_BLACK);
			mp.display.setCursor(12, 48);
			mp.display.printCenter(parsedRingtone);

			mp.display.setCursor(5, 78);
			mp.display.printCenter("Notification");
			mp.display.drawRect(6, 94, 148, 17, TFT_BLACK);
			mp.display.setCursor(12, 93);
			mp.display.printCenter(notificationSounds[mp.notification]);

		}
		
		if (millis() - blinkMillis >= 350)
			{
				blinkMillis = millis();
				blinkState = !blinkState;
			}
		switch (cursor)
		{
			case 0: //RING VOLUME
				mp.display.drawBitmap(8, 20, noSound, blinkState ? TFT_BLACK : 0xA7FF, 2);
				mp.display.drawBitmap(134, 20, fullSound, blinkState ? TFT_BLACK : 0xA7FF, 2);
				if (mp.buttons.repeat(BTN_LEFT,7) && mp.ringVolume > 0)
				{
					mp.ringVolume--;
					mp.osc->setVolume(mp.oscillatorVolumeList[mp.ringVolume]);
					mp.osc->note(75, 0.05);
					mp.osc->play();
					while(!mp.update());
					// Serial.print("Volume: "); Serial.println(mp.volume);
				}
				if (mp.buttons.repeat(BTN_RIGHT,7) && mp.ringVolume < 14)
				{
					mp.ringVolume++;
					mp.osc->setVolume(mp.oscillatorVolumeList[mp.ringVolume]);
					mp.osc->note(75, 0.05);
					mp.osc->play();
					while(!mp.update());
					// Serial.print("Volume: "); Serial.println(mp.volume);
				}
				break;
			
			case 1: //MEDIA VOLUME
				mp.display.drawBitmap(8, 54, noSound, blinkState ? TFT_BLACK : 0xA7FF, 2);
				mp.display.drawBitmap(134, 54, fullSound, blinkState ? TFT_BLACK : 0xA7FF, 2);
				if (mp.buttons.repeat(BTN_LEFT,7) && mp.mediaVolume > 0)
				{
					mp.mediaVolume--;
					mp.osc->setVolume(mp.oscillatorVolumeList[mp.mediaVolume]);
					mp.osc->note(75, 0.05);
					mp.osc->play();
					while(!mp.update());
					// Serial.print("Volume: "); Serial.println(mp.volume);
				}
				if (mp.buttons.repeat(BTN_RIGHT,7) && mp.mediaVolume < 14)
				{
					mp.mediaVolume++;
					mp.osc->setVolume(mp.oscillatorVolumeList[mp.mediaVolume]);
					mp.osc->note(75, 0.05);
					mp.osc->play();
					while(!mp.update());
					// Serial.print("Volume: "); Serial.println(mp.volume);
				}
				break;

			case 2: //MIC SENSITIVITY
			
				mp.display.drawBitmap(125, 88, micPlus, blinkState ? TFT_BLACK : 0xA7FF, 2);
				mp.display.drawBitmap(1, 94, micMinus, blinkState ? TFT_BLACK : 0xA7FF, 2);
				if(mp.buttons.repeat(BTN_RIGHT, 7))
				{
					if(mp.micGain < 15)
						mp.micGain++;
				}
				if(mp.buttons.repeat(BTN_LEFT, 7))
				{
					if(mp.micGain > 0)
						mp.micGain--;
				}

				break;

			case 3: //ARROWDOWN
				if(blinkState)
					mp.display.fillRect(65, 105, 148, 22, 0xA7FF);
				if(mp.buttons.released(BTN_A))
					{
						cursor++;
						currentScreen = 1;
					}
				break;
			case 4: //ARROWUP
				if(blinkState)
					mp.display.fillRect(0, 0, 148, 30, 0xA7FF);
				if (mp.buttons.released(BTN_A))
				{
					cursor--;
					currentScreen = 0;
				}
				break;
			case 5: //RINGTONE
				if(blinkState)
					mp.display.drawRect(6, 47, 148, 17, 0xA7FF);
				if (mp.buttons.released(BTN_A))
				{
					while (!mp.update());
					if (mp.SDinsertedFlag)
					{
						mp.osc->note(75, 0.05);
						mp.osc->play();
						i = ringtoneAudioMenu(audioFiles, audioCount);
						mp.display.setTextColor(TFT_BLACK);
						if (i > -1)
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
						while (millis() < tempMillis + 1500)
						{
							mp.update();
							if (mp.buttons.pressed(BTN_A) || mp.buttons.pressed(BTN_B))
							{
								while (!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
									mp.update();
								break;
							}
						}
						while (!mp.update());
					}
				}
				break;
			case 6: //NOTIFICATION
				if(blinkState)
					mp.display.drawRect(6, 94, 148, 17, 0xA7FF);
				if (mp.buttons.released(BTN_A))
				{
					while (!mp.update())
						;
					int8_t temp = notificationsAudioMenu(notificationSounds, 4);
					if (temp >= 0)
						mp.notification = temp;
				}
				break;
		}

		if (mp.buttons.pressed(BTN_UP))
		{
			
			mp.osc->setVolume(mp.oscillatorVolumeList[mp.mediaVolume]);
			mp.osc->note(75, 0.05);
			mp.osc->play();
			if (cursor > 0)
				cursor--;

			if(cursor <= 3)
				currentScreen = 0;
			if(cursor >= 4)
				currentScreen = 1;
			while(!mp.update());
			blinkState = 0;
			blinkMillis = millis();
			Serial.println("cursor:");
			Serial.println(cursor);
			Serial.println("currscreen:");
			Serial.println(currentScreen);
		}
		if (mp.buttons.pressed(BTN_DOWN))
		{
			
			mp.osc->setVolume(mp.oscillatorVolumeList[mp.mediaVolume]);
			mp.osc->note(75, 0.05);
			mp.osc->play();
			if (cursor < 6)
				cursor++;
			if(cursor <= 3)
				currentScreen = 0;
			if(cursor >= 4)
				currentScreen = 1;
			while(!mp.update());
			blinkState = 0;
			blinkMillis = millis();
			Serial.println("cursor:");
			Serial.println(cursor);
			Serial.println("currscreen:");
			Serial.println(currentScreen);
		}
		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			while(!mp.update());
			break;
		}
		mp.update();
	}
	mp.osc->setVolume(mp.oscillatorVolumeList[mp.mediaVolume]);
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
	if(mp.sim_module_version == 255)
	{
		mp.display.setTextColor(TFT_BLACK);
		mp.display.setTextSize(1);
		mp.display.setTextFont(2);
		mp.display.drawRect(14, 44, 134, 38, TFT_BLACK);
		mp.display.drawRect(13, 43, 136, 40, TFT_BLACK);
		mp.display.fillRect(15, 45, 132, 36, TFT_WHITE);
		mp.display.setCursor(0, mp.display.height()/2 - 21);
		mp.display.setCursor(47, 54);
		mp.display.printCenter("No network board!");
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
		return;
	}
	else if(!mp.simInserted)
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
		while(!mp.update());
		return;
	}
	while (!mp.simReady)
	{
		mp.display.setCursor(0, mp.display.height()/2 - 16);
		mp.display.setTextFont(2);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.fillScreen(TFT_BLACK);
		mp.display.printCenter("GSM still booting...");
		while(!mp.update());
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
	uint16_t foo = reply.indexOf(" ", reply.indexOf("+SPIC:"));
	timesRemaining = reply.substring(foo, reply.indexOf(",", foo)).toInt();
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
			if (millis() - blinkMillis >= 350) //cursor blinking routine
			{
				blinkMillis = millis();
				blinkState = !blinkState;
			}

			if (cursor == 0)
			{
				if (blinkState && pinLockBuffer == 1)
					mp.display.drawRect(69, 12, 17*2, 11*2, TFT_BLACK);
				else if (blinkState && pinLockBuffer == 0)
					mp.display.drawRect(113, 12, 38, 11*2, TFT_BLACK);
				if (mp.buttons.pressed(BTN_LEFT) && pinLockBuffer == 0)
				{
					mp.osc->note(75, 0.05);
					mp.osc->play();
					pinLockBuffer = !pinLockBuffer;
				}
				if (mp.buttons.pressed(BTN_RIGHT) && pinLockBuffer == 1)
				{
					mp.osc->note(75, 0.05);
					mp.osc->play();
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
							if (mp.buttons.released(BTN_FUN_LEFT) && pinBuffer != "")
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
									while(!mp.update());
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
									while(!mp.update());
									delay(1000);
								}
								pinBuffer = "";
							}
							if (timesRemaining == 0)
								break;
							if (mp.buttons.released(BTN_B))
							{
								pinLockBuffer = 1;
								while(!mp.update());
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
				if (mp.buttons.released(BTN_FUN_LEFT))
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
					mp.osc->note(75, 0.05);
					mp.osc->play();

					while(!mp.update());
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
						if (mp.buttons.released(BTN_FUN_LEFT))
						{
							oldPin.remove(oldPin.length() - 1);
							while(!mp.update());
						}
						if (key != NO_KEY && isDigit(key) && oldPin.length() != 4)
							oldPin += key;

						if ((mp.buttons.released(BTN_A) || mp.released(BTN_FUN_RIGHT)) && oldPin.length() == 4)//enter PIN
						{
							while(!mp.update());
							reply = "";
							if (pinLock)
							{
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
									while(!mp.update());
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
										while(!mp.update());
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
									while(!mp.update());
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
							while(!mp.update());
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
					mp.osc->note(75, 0.05);
					mp.osc->play();
					while(!mp.update());
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

			if (mp.buttons.pressed(BTN_UP))
			{
				blinkState = 1;
				blinkMillis = millis();
				mp.osc->note(75, 0.05);
				mp.osc->play();
				while(!mp.update());
				if (cursor == 0 && pinLockBuffer == 1)
					cursor = 1;
				else if (pinLockBuffer == 1 && cursor == 1)
					cursor--;
			}
			if (mp.buttons.pressed(BTN_DOWN))
			{
				blinkState = 1;
				blinkMillis = millis();
				mp.osc->note(75, 0.05);
				mp.osc->play();
				while(!mp.update());
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
	String foo="";
	char key;
	mp.display.setTextWrap(0);
	while(1)
	{
		if(colorSetup == 1)mp.display.fillScreen(0xFFED);
		else mp.display.fillScreen(TFT_WHITE);
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
		mp.display.print(mp.clockYear);

		mp.display.setCursor( 0, 65);
		mp.display.printCenter("Edit time");
		mp.display.drawRect(46,63, 68, 20, TFT_BLACK);
		if(colorSetup == 1) {
			mp.display.drawRect(23, 93, 110, 20, TFT_BLACK);
			mp.display.setCursor( 40, 95);
			mp.display.printCenter("Force time sync");
		}
		else{
			mp.display.drawRect(23, 88, 110, 20, TFT_BLACK);
			mp.display.setCursor( 40, 90);
			mp.display.printCenter("Force time sync");
			mp.display.setCursor(4, 110);
			mp.display.print("Press B to confirm");
		}
		if(cursor == 0)
		{
			if(!blinkState)
			{
				if(colorSetup == 1)
					mp.display.drawRect(46,63, 68, 20, 0xFFED);
				else
					mp.display.drawRect(46,63, 68, 20, TFT_WHITE);
			}
			if(mp.buttons.released(BTN_A))
			{
				mp.buttons.update();
				uint8_t editCursor = 0;
				mp.osc->note(75, 0.05);
				mp.osc->play();
				String inputBuffer;
				if(mp.clockHour == 0)
					inputBuffer = "";
				else
					inputBuffer = String(mp.clockHour);

				while(1)
				{
					if(colorSetup == 1)mp.display.fillScreen(0xFFED);
					else mp.display.fillScreen(TFT_WHITE);
					mp.display.setCursor(2, 110);
					mp.display.print("Erase");
					mp.display.setCursor(120, 110);
					mp.display.print("Save");
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
							mp.display.print(mp.clockYear);
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
							mp.display.print(mp.clockYear);
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
							mp.display.print(mp.clockYear);
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
							mp.display.print(mp.clockYear);
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
							mp.display.print(mp.clockYear);
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
					if (mp.buttons.released(BTN_FUN_LEFT))
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
					if(mp.buttons.pressed(BTN_RIGHT) && editCursor < 5) //RIGHT
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
									mp.clockYear = 2000;
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
									mp.clockYear = 2000 + inputBuffer.toInt();
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
									inputBuffer = String(mp.clockYear % 100);
								else
									inputBuffer = "";
								break;
						}
						editCursor++;

					}
					if(mp.buttons.pressed(BTN_LEFT) && editCursor > 0) //LEFT
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
									mp.clockYear = 2000;
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
									mp.clockYear = 2000+inputBuffer.toInt();
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

					if(mp.buttons.pressed(BTN_UP) && editCursor > 2) //UP
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
									mp.clockYear = 2000;
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
									mp.clockYear = 2000 + inputBuffer.toInt();
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
					if(mp.buttons.pressed(BTN_DOWN) && editCursor < 3) //DOWN
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
									mp.clockYear = 2000;
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
									mp.clockYear = 2000 + inputBuffer.toInt();
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
									inputBuffer = String(mp.clockYear % 100);
								else
									inputBuffer = "";
								break;
						}
						editCursor+=3;
					}

					if(mp.buttons.released(BTN_A) || mp.buttons.released(BTN_FUN_RIGHT))
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
									mp.clockYear = 2000;
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
									mp.clockYear = 2000 + inputBuffer.toInt();
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
				mp.update();
			}
		}
		else if(cursor == 1)
		{
			if(!blinkState)
			{
				if(colorSetup == 1)
					mp.display.drawRect(23,93, 110, 20, 0xFFED);
				else
					mp.display.drawRect(23,88, 110, 20, TFT_WHITE);
			}
			if(mp.buttons.released(BTN_A))
			{
				mp.buttons.update();
				if(mp.sim_module_version == 255)
				{
					mp.display.fillScreen(TFT_BLACK);
					mp.display.setTextColor(TFT_WHITE);
					mp.display.setTextSize(1);
					mp.display.setCursor(0, mp.display.height()/2 - 20);
					mp.display.setTextFont(2);
					mp.display.printCenter(F("No network board!"));
					mp.display.setCursor(0, mp.display.height()/2);
					mp.display.printCenter(F("Couldn't fetch time"));
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
					mp.display.printCenter(F("Couldn't fetch time"));
					uint32_t tempMillis = millis();
					while(millis() < tempMillis + 2000 && !mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
						mp.update();
					while(!mp.update());
				}
				else
				{
					mp.osc->note(75, 0.05);
					mp.osc->play();
					mp.clockYear = 2000;
					previousMillis = millis();
					while(1)
					{
						if(colorSetup == 1)mp.display.fillScreen(0xFFED);
						else mp.display.fillScreen(TFT_WHITE);
						mp.display.setCursor(0, mp.display.height()/2 - 16);
						mp.display.printCenter("Fetching time...");
						if(millis() - previousMillis >= 4000)
						{
							if(colorSetup == 1)mp.display.fillScreen(0xFFED);
							else mp.display.fillScreen(TFT_WHITE);
							mp.display.setCursor(0, mp.display.height()/2 - 20);
							mp.display.printCenter("Couldn't fetch time");
							mp.display.setCursor(0, mp.display.height()/2);
							mp.display.printCenter("Set it manually");
							while(!mp.update());
							delay(2000);
							break;
						}
						mp.updateTimeGSM();
						if(mp.clockYear % 100 < 80 && mp.clockYear % 100 >= 19)
						{
							delay(200);
							if(colorSetup == 1)mp.display.fillScreen(0xFFED);
							else mp.display.fillScreen(TFT_WHITE);
							mp.display.setCursor(0, mp.display.height()/2 - 16);
							mp.display.printCenter("Time fetched over GSM!");
							while(!mp.update());
							delay(1500);
							break;
						}
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
		if (mp.buttons.pressed(BTN_UP) && cursor > 0)
		{
			mp.osc->note(75, 0.05);
			mp.osc->play();
			blinkState = 1;
			previousMillis = millis() + 400;
			while(!mp.update());
			cursor--;
		}
		if (mp.buttons.pressed(BTN_DOWN) && cursor < 1)
		{
			mp.osc->note(75, 0.05);
			mp.osc->play();
			blinkState = 1;
			previousMillis = millis() + 400;
			while(!mp.update());
			cursor++;
		}
		if(colorSetup == 1)
		{  
			if (mp.buttons.released(BTN_B)) //BUTTON BACK
			break;
 		}
		else {
			if (mp.buttons.released(BTN_B)){
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextSize(1);
				mp.display.setTextFont(2);
				mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
				mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
				mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
				mp.display.setCursor(47, 55);
				mp.display.printCenter("Date & time set!");
				mp.update();
				uint32_t tempMillis = millis();
				while(millis() < tempMillis + 1000)
				mp.update();
				break;
			}
		} 
		mp.update();
	}
	colorSetup = 0;

}
bool updateMenu()
{
	bool blinkState = 0;
	uint32_t previousMillis = millis();
	uint8_t cursor = 0;
	String foo="";
	

	while(1)
	{
		mp.display.setTextWrap(0);
		mp.display.setTextFont(2);
		mp.display.fillScreen(0xFD29);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.setCursor(10, 10);
		mp.display.printCenter("Check for update");
		mp.display.setCursor(40, 35);
		mp.display.printCenter("Factory reset");
		mp.display.setCursor(40, 60);
		mp.display.printCenter("Setup wizard");
		mp.display.setCursor(40, 85);
		mp.display.printCenter("SIM module debug");

		mp.display.setCursor(90, 110);
		mp.display.setTextColor(TFT_DARKGREY);
		foo = "Version: " + (String)((int)mp.firmware_version / 100) + "." + (String)((int)mp.firmware_version / 10) + "." + (String)(mp.firmware_version % 10);
		mp.display.printCenter(foo);
		switch (cursor)
		{
			case 0:
				if(blinkState)
					mp.display.drawRect(20,8,117, 20, TFT_BLACK);
				else
					mp.display.drawRect(20,8,117, 20, 0xFD29);
				if(mp.buttons.released(BTN_A))
				{
					if(!mp.SDinsertedFlag)
					{
						mp.display.setTextColor(TFT_BLACK);
						mp.display.setTextSize(1);
						mp.display.setTextFont(2);
						mp.display.drawRect(4, 49, 152, 28, TFT_BLACK);
						mp.display.drawRect(3, 48, 154, 30, TFT_BLACK);
						mp.display.fillRect(5, 50, 150, 26, 0xFD29);
						mp.display.setCursor(47, 54);
						mp.display.printCenter("SD card missing");
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
					{
						// if(mp.wifi)
						// {
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
						// }
						// else
						// {
						// 	mp.display.setTextColor(TFT_BLACK);
						// 	mp.display.setTextSize(1);
						// 	mp.display.setTextFont(2);
						// 	mp.display.drawRect(4, 49, 152, 28, TFT_BLACK);
						// 	mp.display.drawRect(3, 48, 154, 30, TFT_BLACK);
						// 	mp.display.fillRect(5, 50, 150, 26, 0xFD29);
						// 	mp.display.setCursor(47, 54);
						// 	mp.display.printCenter("Wifi turned off!");
						// 	uint32_t tempMillis = millis();
						// 	while(millis() < tempMillis + 2000)
						// 	{
						// 		mp.update();
						// 		if(mp.buttons.pressed(BTN_A) || mp.buttons.pressed(BTN_B))
						// 		{
						// 			while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
						// 				mp.update();
						// 			break;
						// 		}
						// 	}
						// }
					}
					while(!mp.update());
				}
			break;

			case 1:
				if(blinkState)
					mp.display.drawRect(30, 33, 96, 20, TFT_BLACK);
				else
					mp.display.drawRect(30, 33, 96, 20, 0xFD29);
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

						String contacts_default = "[]";
						String settings_default = "{\"airplane_mode\": 0, \"brightness\": 5, \"sleep_time\": 0, \"background_color\": 0, \"notification\" : 0, \"ringtone\" : \"/Music/Default ringtone.wav\" , \"ringVolume\" : 10, \"mediaVolume\" : 10, \"micGain\" : 15}";

						const char contacts_path[] = "/.core/contacts.json";
						const char settings_path[] = "/.core/settings.json";

						mp.jb.clear();
						JsonArray& contacts = mp.jb.parseArray(contacts_default);
						JsonObject& settings = mp.jb.parseObject(settings_default);

						SD.remove(contacts_path);
						SD.remove(settings_path);

						File contacts_file = SD.open(contacts_path, "w");
						contacts.prettyPrintTo(contacts_file);
						contacts_file.close();

						File settings_file = SD.open(settings_path, "w");
						settings.prettyPrintTo(settings_file);
						settings_file.close();

						// mp.wifi = settings["wifi"];
						// mp.bt = settings["bluetooth"];
						mp.airplaneMode = settings["airplane_mode"];
						mp.brightness = settings["brightness"];
						mp.sleepTime = settings["sleep_time"];
						mp.backgroundIndex = settings["background_color"];
						mp.notification = settings["notification"];
						mp.ringtone_path = String(settings["ringtone"].as<char*>());
						mp.ringVolume = settings["ringVolume"];
						mp.micGain = settings["micGain"];
						Serial.println("ERASED");
						delay(5);
						mp.applySettings();
					}
					else
					{
						// mp.wifi = 1;
						// mp.bt = 0;
						mp.airplaneMode = 0;
						mp.brightness = 5;
						mp.sleepTime = 0;
						mp.backgroundIndex = 0;
						mp.ringVolume = 10;
						mp.mediaVolume = 10;
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
					while(!mp.update());
					return 1;

				}
			break;
			
			case 2:
				if(blinkState)
					mp.display.drawRect(33, 58, 93, 20, TFT_BLACK);
				else
					mp.display.drawRect(33, 58, 93, 20, 0xFD29);
				if(mp.buttons.released(BTN_A))
					startupWizard();
			break;

			case 3:
				if(blinkState)
					mp.display.drawRect(20,83,117, 20, TFT_BLACK);
				else
					mp.display.drawRect(20,83,117, 20, 0xFD29);
				if(mp.buttons.released(BTN_A))
				{
					while(!mp.update());
					controlTry();
				}
			break;
		}
		
		if(millis()-previousMillis >= 250)
		{
			previousMillis = millis();
			blinkState = !blinkState;
		}
		if (mp.buttons.pressed(BTN_UP))
		{
			mp.osc->note(75, 0.05);
			mp.osc->play();
			blinkState = 1;
			previousMillis = millis();
			while(!mp.update());
			if(cursor == 0)
				cursor = 3;
			else
				cursor--;
		}
		if (mp.buttons.pressed(BTN_DOWN))
		{
			mp.osc->note(75, 0.05);
			mp.osc->play();
			blinkState = 1;
			previousMillis = millis();
			while(!mp.update());
			if(cursor == 3)
				cursor = 0;
			else
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
	bool blinkState = 0;
	uint32_t blinkMillis = millis();
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
		if(millis() - blinkMillis > 350)
		{
			blinkMillis = millis();
			blinkState = !blinkState;
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
		if(mp.buttons.released(BTN_FUN_RIGHT))
		{
			while(!mp.update());
			mp.playNotificationSound(cursor);
		}

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

		if (mp.buttons.pressed(BTN_DOWN)) { //BUTTON DOWN
			blinkState = 1;
			blinkMillis = millis();
			while(!mp.update());
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
			while(!mp.update());
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
	// if(!mp.wifi)
	// {
	// 	mp.display.setTextColor(TFT_BLACK);
	// 	mp.display.setTextSize(1);
	// 	mp.display.setTextFont(2);
	// 	mp.display.drawRect(4, 49, 152, 28, TFT_BLACK);
	// 	mp.display.drawRect(3, 48, 154, 30, TFT_BLACK);
	// 	mp.display.fillRect(5, 50, 150, 26, 0xFD29);
	// 	mp.display.setCursor(47, 54);
	// 	mp.display.printCenter("WiFi turned off!");
	// 	uint32_t tempMillis = millis();
	// 	while(millis() < tempMillis + 2000)
	// 	{
	// 		mp.update();
	// 		if(mp.buttons.pressed(BTN_A) || mp.buttons.pressed(BTN_B))
	// 		{
	// 			while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
	// 				mp.update();
	// 			break;
	// 		}
	// 	}
	// 	mp.update();
	// }
	bool blinkState = 1;
	unsigned long elapsedMillis = millis();
	bool helpPop;
	String content = ""; //password string
	String prevContent = "";
	delay(500);
	WiFi.begin();
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(1000);
	int n = WiFi.scanNetworks();
	// delay(1000);
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
		WiFi.scanDelete();
		WiFi.disconnect(true); delay(10); // disable WIFI altogether
		WiFi.mode(WIFI_MODE_NULL); delay(10);
		while(!mp.update());
		uint32_t tempMillis = millis();
		WiFi.begin();
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
		while(1)
		{

			int8_t selection = wifiNetworksMenu(networkNames, wifiSignalStrengths, n);
			if(selection < 0)
			{
				while(!mp.update());
				return;
			}
			
			if (wifiPasswordNeeded[selection])
			{
				mp.textInput("");
				prevContent = "";
				content = "";
				mp.textPointer = 0;
				while(!mp.update());
				while (1)
				{
					mp.display.fillScreen(TFT_BLACK);
					mp.display.setCursor(8, 8);
					mp.display.printCenter(networkNames[selection]);
					mp.display.setCursor(4, 30);
					mp.display.printCenter("Enter password:");
					mp.display.setCursor(1, 112);
					mp.display.print("Erase");
					mp.display.setCursor(22, 83);
					mp.display.print("Press A to confirm");
					mp.display.setCursor(133, 112);
					mp.display.print("Help");
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

					if(mp.buttons.released(BTN_FUN_RIGHT)){
							helpPop = !helpPop;
							mp.display.drawIcon(TextHelperPopup, 0, 0, 160, 128, 1, TFT_WHITE);	
							while(!mp.update());
						}
						while (helpPop) {
							if(mp.buttons.released(BTN_FUN_RIGHT) || mp.buttons.released(BTN_B)){
								helpPop = !helpPop;
							}
						mp.update();
						}
					if((mp.buttons.released(BTN_A)) && content.length() > 0)
					{
						Serial.println("PRESSED");
						mp.display.setCursor(20, 50);
						mp.display.fillRect(0, 28, 160, 100, TFT_BLACK);
						mp.display.setCursor(0,40);
						mp.display.printCenter("Connecting");
						mp.display.setCursor(60, 60);
						while(!mp.update());

						char temp[networkNames[selection].length()+1];
						char temp2[content.length()];
						networkNames[selection].toCharArray(temp, networkNames[selection].length()+1);
						content.toCharArray(temp2, content.length()+1);
						WiFi.begin(temp, temp2);
						uint8_t counter = 0;
						while (WiFi.status() != WL_CONNECTED)
						{
							delay(1000);
							mp.display.print(".");
							while(!mp.update());
							counter++;
							if (counter >= 8)
							{
								mp.display.fillRect(0, 40, mp.display.width(), 60, TFT_BLACK);
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
						Serial.print("Wifi status: ");
						Serial.println(WiFi.status());
						// if(WiFi.status() == WL_DISCONNECTED)
						// {
							
						// 	mp.display.fillRect(0, 40, mp.display.width(), 60, TFT_BLACK);
						// 	mp.display.setCursor(0, 45);
						// 	mp.display.printCenter("Wrong password :(");
						// 	uint32_t tempMillis = millis();
						// 	while(millis() < tempMillis + 2000)
						// 	{
						// 		mp.update();
						// 		if(mp.buttons.pressed(BTN_A) || mp.buttons.pressed(BTN_B))
						// 		{
						// 			while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
						// 				mp.update();
						// 			break;
						// 		}
						// 	}
						// 	while(!mp.update());
						// 	break;
						// }
						if(WiFi.status() == WL_CONNECTED)
						{
							// Serial.println(WiFi.status());
							// mp.display.setCursor(20, 50);
							// mp.display.fillRect(0, 28, 160, 100, TFT_BLACK);
							// mp.display.setCursor(0,40);
							// mp.display.printCenter("Connected!");
							// mp.display.setCursor(60, 60);
							// while(!mp.update());
							// delay(1000);
							// Serial.println(WiFi.status());
							// mp.deallocateAudio();
							mp.display.deleteSprite();
							Serial.println("FREE HEAP:");
							Serial.println(ESP.getFreeHeap());
							int8_t selection = checkForUpdate();
							Serial.println("OUT");
							delay(5);
							EEPROM.writeBool(33, 0);
							EEPROM.commit();
							if(selection == 1)
							{
								EEPROM.writeBool(34, 1);
								EEPROM.writeString(35, temp);
								EEPROM.writeString(100, temp2);
								EEPROM.commit();
								mp.tft.fillRect(0,0,160,128,TFT_BLACK);
								mp.tft.setCursor(8,mp.tft.height()/2 - 26);
								mp.tft.print("Downloading firmware...");
								mp.tft.setCursor(32,mp.tft.height()/2 - 5);
								mp.tft.print("Don't turn off!");
								// delay(1000);

								ESP.restart();
								// mp.tft.fillRect(0,0,160,128, TFT_BLACK);
								// mp.tft.setCursor(0,mp.tft.height() / 2 - 16);
								// mp.tft.printCenter(F("Downloading update"));
								// // while(!mp.update());
								// Serial.println("FREE HEAP:");
								// Serial.println(ESP.getFreeHeap());
								// fetchUpdate();
								// mp.tft.fillRect(0,0,160,128, TFT_BLACK);
								// mp.tft.setCursor(0,mp.tft.height() / 2 - 16);
								// mp.tft.printCenter(F("Installing update"));
								// // while(!mp.update());
								// Serial.println("FREE HEAP:");
								// Serial.println(ESP.getFreeHeap());
								// mp.updateFromFS("/.core/LOADER.BIN");
							}

							else if(selection == 0)
							{
								mp.tft.setTextFont(2);
								mp.tft.setTextSize(1);
								mp.tft.setTextColor(TFT_WHITE);
								mp.tft.fillRect(0, 40, mp.tft.width(), 50, TFT_BLACK);
								mp.tft.setCursor(20, 45);
								mp.tft.print("No updates available");
								mp.tft.setCursor(20, 65);
								mp.tft.print("Restarting phone...");
								// while(!mp.update());
								delay(2000);
								ESP.restart();
							}
							else if(selection == -2)
							{
								mp.tft.setTextFont(2);
								mp.tft.setTextSize(1);
								mp.tft.fillRect(0, 40, mp.tft.width(), 50, TFT_BLACK);
								mp.tft.setCursor(30, 45);
								mp.tft.print("Server error!");
								mp.tft.setCursor(30, 65);
								mp.tft.print("Restarting phone...");
								// while(!mp.update());
								delay(2000);
								ESP.restart();
							}
							else
							{
								mp.tft.setTextFont(2);
								mp.tft.setTextSize(1);
								mp.tft.fillRect(0,0,160,128, TFT_BLACK);
								mp.tft.setCursor(30,mp.tft.height() / 2 - 16);
								mp.tft.print(F("Restarting phone"));
								// while(!mp.update());
								delay(750);
								ESP.restart();
							}
						}

					}
					if(mp.buttons.released(BTN_B))
					{
						while(!mp.update());
						break;
					}
					mp.update();
				}
			}
			else
			{
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setCursor(8, 8);
				mp.display.printCenter(networkNames[selection]);
				mp.display.setCursor(0,40);
				mp.display.printCenter("Connecting");
				mp.display.setCursor(60, 60);
				while(!mp.update());

				char temp[networkNames[selection].length()+1];
				networkNames[selection].toCharArray(temp, networkNames[selection].length()+1);
				Serial.println(temp);
				WiFi.begin(temp);
				uint8_t counter = 0;
				while (WiFi.status() != WL_CONNECTED)
				{
					delay(1000);
					mp.display.print(".");
					while(!mp.update());
					counter++;
					if (counter >= 8)
					{
						mp.display.fillRect(0, 40, mp.display.width(), 60, TFT_BLACK);
						mp.display.setCursor(0, 45);
						mp.display.printCenter("Wi-Fi error!");
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
				if(WiFi.status() == WL_DISCONNECTED)
				{
					
					mp.display.fillRect(0, 40, mp.display.width(), 60, TFT_BLACK);
					mp.display.setCursor(0, 45);
					mp.display.printCenter("Wi-Fi error!");
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
				else if(WiFi.status() == WL_CONNECTED)
				{
					mp.display.deleteSprite();
					Serial.println("FREE HEAP:");
					Serial.println(ESP.getFreeHeap());
					int8_t selection = checkForUpdate();
					Serial.println("OUT");
					delay(5);
					
					if(selection == 1)
					{
						EEPROM.writeBool(34, 1);
						EEPROM.writeString(35, temp);
						EEPROM.writeString(100, "");
						EEPROM.commit();
						mp.tft.fillRect(0,0,160,128,TFT_BLACK);
						mp.tft.setCursor(8,mp.tft.height()/2 - 26);
						mp.tft.print("Downloading firmware...");
						mp.tft.setCursor(32,mp.tft.height()/2 - 5);
						mp.tft.print("Don't turn off!");
						// delay(1000);

						ESP.restart();
						// mp.tft.fillRect(0,0,160,128, TFT_BLACK);
						// mp.tft.setCursor(0,mp.tft.height() / 2 - 16);
						// mp.tft.printCenter(F("Downloading update"));
						// // while(!mp.update());
						// Serial.println("FREE HEAP:");
						// Serial.println(ESP.getFreeHeap());
						// fetchUpdate();
						// mp.tft.fillRect(0,0,160,128, TFT_BLACK);
						// mp.tft.setCursor(0,mp.tft.height() / 2 - 16);
						// mp.tft.printCenter(F("Installing update"));
						// // while(!mp.update());
						// Serial.println("FREE HEAP:");
						// Serial.println(ESP.getFreeHeap());
						// mp.updateFromFS("/.core/LOADER.BIN");
					}

					else if(selection == 0)
					{
						mp.tft.setTextFont(2);
						mp.tft.setTextSize(1);
						mp.tft.setTextColor(TFT_WHITE);
						mp.tft.fillRect(0, 40, mp.tft.width(), 50, TFT_BLACK);
						mp.tft.setCursor(20, 45);
						mp.tft.print("No updates available");
						mp.tft.setCursor(20, 65);
						mp.tft.print("Restarting phone...");
						delay(2000);
						ESP.restart();
					}
					else if(selection == -2)
					{
						mp.tft.setTextFont(2);
						mp.tft.setTextSize(1);
						mp.tft.fillRect(0, 40, mp.tft.width(), 50, TFT_BLACK);
						mp.tft.setCursor(30, 45);
						mp.tft.print("Server error!");
						mp.tft.setCursor(30, 65);
						mp.tft.print("Restarting phone...");
						delay(2000);
						ESP.restart();
					}
					else
					{
						mp.tft.setTextFont(2);
						mp.tft.setTextSize(1);
						mp.tft.fillRect(0,0,160,128, TFT_BLACK);
						mp.tft.setCursor(30,mp.tft.height() / 2 - 16);
						mp.tft.print(F("Restarting phone"));
						delay(750);
						ESP.restart();
					}
				}
			}
		}
	}
}
int8_t checkForUpdate()
{
	
	if(WiFi.status() != WL_CONNECTED)
	{
		Serial.println("not connected");
		return -1;
	}
	HTTPClient http;
	const char* ca = \
	"-----BEGIN CERTIFICATE-----\n" \
	"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n" \
	"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
	"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n" \
	"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n" \
	"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n" \
	"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n" \
	"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n" \
	"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n" \
	"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n" \
	"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n" \
	"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n" \
	"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n" \
	"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n" \
	"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n" \
	"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n" \
	"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n" \
	"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n" \
	"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n" \
	"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n" \
	"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n" \
	"+OkuE6N36B9K\n" \
	"-----END CERTIFICATE-----\n";

	Serial.print("[HTTP] begin...\n");
	delay(5);
	// configure traged server and url
	http.begin("https://raw.githubusercontent.com/CircuitMess/CircuitMess-Ringo-firmware/master/README.md", ca); //HTTPS
	//http.begin("http://example.com/index.html"); //HTTP

	Serial.print("[HTTP] GET...\n");
	// start connection and send HTTP header
	int httpCode = http.GET();
	Serial.printf("[HTTP] GET... code: %d\n", httpCode);
	delay(5);
	// httpCode will be negative on error

	if (httpCode > 0) {
		// file found at server
		if (httpCode == HTTP_CODE_OK) {
			String payload = http.getString();
			http.end();
			uint16_t version = payload.substring(payload.indexOf("version=") + 8, payload.indexOf("\r")).toInt();
			if (version > mp.firmware_version)
			{
				Serial.println("HERE");
				String foo = String("Version: " + String((int)version/100) + "." + String((int)version/10)
					+ "." + String(version%10));
				Serial.println(foo);
				mp.tft.fillRect(0,0,160,128,TFT_BLACK);
				mp.tft.setTextFont(2);
				mp.tft.setTextSize(1);
				mp.tft.setTextColor(TFT_WHITE);
				mp.tft.setCursor(21,mp.tft.height()/2 - 26);
				mp.tft.print("UPDATE AVAILABLE!");
				mp.tft.setCursor(30, 70);
				mp.tft.print(foo);
				mp.tft.setCursor(4,110);
				mp.tft.print("Press A to start update");
				while(!mp.buttons.released(BTN_A))
				{
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
	else
		return -2;
}
bool fetchUpdate()
{
	if(WiFi.status() != WL_CONNECTED)
		return 0;
	HTTPClient http;
	const char* ca = \
	"-----BEGIN CERTIFICATE-----\n" \
	"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n" \
	"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
	"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n" \
	"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n" \
	"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n" \
	"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n" \
	"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n" \
	"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n" \
	"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n" \
	"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n" \
	"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n" \
	"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n" \
	"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n" \
	"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n" \
	"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n" \
	"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n" \
	"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n" \
	"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n" \
	"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n" \
	"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n" \
	"+OkuE6N36B9K\n" \
	"-----END CERTIFICATE-----\n";
	SD.remove("/.core/LOADER.BIN");
	File file = SD.open("/.core/LOADER.BIN", "w");
	Serial.print("[HTTP] begin...\n");
	// configure traged server and url
	http.begin("https://raw.githubusercontent.com/CircuitMess/CircuitMess-Ringo-firmware/master/firmware.bin", ca); //HTTPS
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
	bool blinkState = 0;
	uint32_t blinkMillis = millis();
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
		if(millis() - blinkMillis > 350)
		{
			blinkMillis = millis();
			blinkState = !blinkState;
		}
		for (uint8_t i = 0; i < length; i++)
			wifiDrawBox(items[i], signals[i], i, cameraY_actual);
		if(blinkState)
			wifiDrawCursor(cursor, cameraY_actual);

		mp.display.fillRect(0, 0, mp.display.width(), 20, TFT_DARKGREY);
		mp.display.setTextFont(2);
		mp.display.setCursor(2,1);
		mp.display.drawFastHLine(0, 19, mp.display.width(), TFT_WHITE);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.print("Choose a network");
		mp.display.fillRect(0, 111, mp.display.width(), 30, TFT_DARKGREY);
		mp.display.setTextFont(2);
		mp.display.setCursor(2,112);
		mp.display.drawFastHLine(0, 111, mp.display.width(), TFT_WHITE);
		mp.display.printCenter("Cancel            Select");
		if(mp.released(BTN_FUN_RIGHT))
		{
			while(!mp.update());
			mp.playNotificationSound(cursor);
		}

		if (mp.buttons.released(BTN_A) || mp.buttons.released(BTN_FUN_RIGHT)) {   //BUTTON CONFIRM
			while(!mp.update());
			break;
		}
		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
			blinkState = 1;
			blinkMillis = millis();
			while(!mp.update());
			if (cursor == 0) {
				cursor = length - 1;
				if (length > 4) {
					cameraY = -(cursor - 3) * (boxHeight + 1);
				}
			}
			else {
				if (cursor > 0 && (cursor * (boxHeight + 1)  + cameraY + offset) <= 30) {
					cameraY += (boxHeight + 1);
				}
				cursor--;
			}

		}

		if (mp.buttons.pressed(BTN_DOWN)) { //BUTTON DOWN
			blinkState = 1;
			blinkMillis = millis();
			while(!mp.update());
			cursor++;
			if ((cursor * (boxHeight + 1) + cameraY + offset) > 100) {
				cameraY -= (boxHeight + 1);
			}
			if (cursor >= length) {
				cursor = 0;
				cameraY = 0;

			}

		}
		if (mp.buttons.released(BTN_B) || mp.buttons.released(BTN_FUN_LEFT)) //BUTTON BACK
		{
			while(!mp.update());
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
	mp.display.setCursor(5, y+2);
	for(uint16_t i = 0; i < text.length(); i++)
	{
		mp.display.print(text[i]);
		if(mp.display.getCursorX() > 120)
		{
			mp.display.print("...");
			break;
		}
	}
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
	y += i * (boxHeight + 2) + offset;
	mp.display.drawRect(1, y, mp.display.width() - 2, boxHeight + 1, TFT_RED);
	mp.display.drawRect(0, y-1, mp.display.width(), boxHeight + 3, TFT_RED);
}
int16_t ringtoneAudioMenu(String* items, uint16_t length) {
	MPTrack *ringtonePreview = nullptr;
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	String Name;
	uint8_t scale = 2;
	uint8_t offset = 17;
	uint8_t boxHeight = 15;
	uint16_t start = 0;
	int16_t cursor = 0;
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
		mp.display.print("Ringtones");
		mp.display.drawFastHLine(0, 112, BUF2WIDTH, TFT_WHITE);
		mp.display.fillRect(0, 113, mp.display.width(), 30, TFT_DARKGREY);
		mp.display.setCursor(130, 111);
		if(ringtonePreview != nullptr)
		{
			if(ringtonePreview->isPlaying())
				mp.display.print("Stop");
			else
				mp.display.print("Play");
		}
		else
			mp.display.print("Play");

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
				if (length > 5) {
					cameraY = -(cursor - 4) * (boxHeight + 1) - 1;
				}
			}
			else {
				if (cursor > 0 && (cursor * (boxHeight + 1) + cameraY + offset) < 20) {
					cameraY += (boxHeight + 1);
				}
				cursor--;
			}

		}

		if (mp.buttons.pressed(BTN_DOWN)) { //BUTTON DOWN
			blinkState = 1;
			blinkMillis = millis();
			while(!mp.update());
			cursor++;
			if ((cursor * (boxHeight + 1) + cameraY + offset) > 90) {
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
		if(mp.buttons.released(BTN_FUN_RIGHT))
		{
			if(ringtonePreview == nullptr)
			{
				ringtonePreview = new MPTrack(audioFiles[cursor].c_str());
				if(mp.mediaVolume == 0)
					ringtonePreview->setVolume(0);
				else
					ringtonePreview->setVolume(map(mp.mediaVolume, 0, 14, 100, 300));
				ringtonePreview->setRepeat(1);
				ringtonePreview->play();
				addTrack(ringtonePreview);
			}
			else
			{
				if(ringtonePreview->isPlaying())
					ringtonePreview->stop();
				else
				{
					ringtonePreview->reloadFile((char*)audioFiles[cursor].c_str());
					ringtonePreview->setRepeat(1);
					ringtonePreview->play();
				}
			}
			
		}
		mp.update();
	}
	if(ringtonePreview != nullptr)
	{
		ringtonePreview->stop();
		removeTrack(ringtonePreview);
	}
	return cursor;
}