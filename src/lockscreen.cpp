#include "lockscreen.h"
String monthsList[] PROGMEM = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
void drawNotificationWindow(uint8_t y, uint8_t index) {
	mp.display.setTextFont(2);
	mp.display.setTextSize(1);
	mp.display.setCursor(22, y + 2);
	mp.display.fillRoundRect(2, y + 2, 154, 20, 2, TFT_DARKGREY);
	mp.display.fillRoundRect(4, y, 154, 20, 2, TFT_WHITE);
	mp.display.print(mp.notificationDescriptionList[index]);
	switch (mp.notificationTypeList[index])
	{
		case 1:
			mp.display.drawBitmap(5, y + 2, missedCallIcon, TFT_RED, 2);
			break;
		case 2:
			mp.display.drawBitmap(5, y + 2, newMessageIcon, TFT_BLACK, 2);
			break;
		case 3:
			mp.display.drawBitmap(5, y + 2, systemNotification, TFT_RED, 2);
			break;
	}
	mp.display.setTextFont(1);
	mp.display.setCursor(129, y + 2);
	String temp = "";
	DateTime now = mp.notificationTimeList[index];
	if (now.hour() < 10)
		temp.concat("0");
	temp.concat(now.hour());
	mp.display.print(temp);
	mp.display.setCursor(mp.display.cursor_x - 2, mp.display.cursor_y);
	mp.display.print(":");
	mp.display.setCursor(mp.display.cursor_x - 2, mp.display.cursor_y);
	temp = "";
	if (now.minute() < 10)
		temp.concat("0");
	temp.concat(now.minute());
	mp.display.print(temp);
	mp.display.setCursor(125, y + 11);
	temp = "";
	mp.display.print(monthsList[now.month() - 1]);
	mp.display.setCursor(mp.display.cursor_x + 2, mp.display.cursor_y);
	if (now.day() < 10)
		temp.concat("0");
	temp.concat(now.day());
	mp.display.print(temp);
}
void lockscreen() {
	mp.dataRefreshFlag = 1;
    bool blinkState = 0;
    // bool goOut = 0;
	uint32_t elapsedMillis = millis();
    uint32_t buttonHeld;
	// FastLED.clear();

	while (1)
	{
		mp.display.fillScreen(backgroundColors[mp.backgroundIndex]);

		mp.display.setFreeFont(TT1);
		mp.display.setTextSize(6);
		//Hour shadow
		mp.display.setTextColor(TFT_DARKGREY);
		if (mp.clockHour == 11)
			mp.display.setCursor(10*2, 28*2);
		else if (mp.clockHour % 10 == 1 || (int)mp.clockHour / 10 == 1)
			mp.display.setCursor(7*2, 28*2);
		else
			mp.display.setCursor(4*2, 28*2);
		if (mp.clockHour < 10)
			mp.display.print("0");
		mp.display.print(mp.clockHour);

		//minute shadow
		mp.display.setCursor(36*2, 28*2);
		if (mp.clockMinute < 10)
			mp.display.print("0");
		mp.display.print(mp.clockMinute);

		//Hour black
		mp.display.setTextColor(TFT_BLACK);
		if (mp.clockHour == 11)
			mp.display.setCursor(9*2, 27*2);
		else if (mp.clockHour % 10 == 1 || (int)mp.clockHour / 10 == 1)
			mp.display.setCursor(6*2, 27*2);
		else
			mp.display.setCursor(3*2, 27*2);
		if (mp.clockHour < 10)
			mp.display.print("0");
		mp.display.print(mp.clockHour);

		//Minute black
		mp.display.setCursor(35*2, 27*2);
		if (mp.clockMinute < 10)
			mp.display.print("0");
		mp.display.print(mp.clockMinute);

		mp.display.setTextSize(1*2);
		mp.display.setCursor(60*2, 19*2);
		mp.display.setTextWrap(false);
		if (mp.clockDay < 10)
			mp.display.print("0");
		mp.display.print(mp.clockDay);
		mp.display.print("/");
		if (mp.clockMonth < 10)
			mp.display.print("0");
		mp.display.print(mp.clockMonth);
		mp.display.setCursor(62*2, 25*2);
		mp.display.print(mp.clockYear);
		/*mp.display.setTextSize(2);
		mp.display.setCursor(10, 50);
		mp.display.print("12:00");*/
		uint8_t helper = 11;
		if (mp.simInserted && !mp.airplaneMode)
		{
			if (mp.signalStrength <= 3)
				mp.display.drawBitmap(2, 2, noSignalIcon, TFT_BLACK, 2);
			else if (mp.signalStrength > 3 && mp.signalStrength <= 10)
				mp.display.drawBitmap(2, 2, signalLowIcon, TFT_BLACK, 2);
			else if (mp.signalStrength > 10 && mp.signalStrength <= 20)
				mp.display.drawBitmap(2, 2, signalHighIcon, TFT_BLACK, 2);
			else if (mp.signalStrength > 20 && mp.signalStrength <= 31)
				mp.display.drawBitmap(2, 2, signalFullIcon, TFT_BLACK, 2);
			else if (mp.signalStrength == 99)
				mp.display.drawBitmap(2, 2, signalErrorIcon, TFT_BLACK, 2);
		}
		else if(!mp.simInserted && !mp.airplaneMode)
			mp.display.drawBitmap(2, 2, signalErrorIcon, TFT_BLACK, 2);
		else if(mp.airplaneMode)
		{
			mp.display.drawBitmap(2, 2, airplaneModeIcon, TFT_BLACK, 2);
		}
		if (mp.ringVolume == 0)
		{
			mp.display.drawBitmap(helper*2, 2, silentModeIcon, TFT_BLACK, 2);
			helper += 10;
		}
		//mp.display.drawBitmap(31, 1, missedcall);
		//mp.display.drawBitmap(41, 1, newtext);
		// if (!mp.airplaneMode)
		// {
		// 	if (mp.wifi == 1)
		// 		mp.display.drawBitmap(helper * 2, 2, wifiOnIcon, TFT_BLACK, 2);
		// 	else
		// 		mp.display.drawBitmap(helper * 2, 2, wifiOffIcon, TFT_BLACK, 2);
		// 	helper += 10;
		// 	if (mp.bt)
		// 		mp.display.drawBitmap(helper * 2, 2, BTonIcon, TFT_BLACK, 2);
		// 	else
		// 		mp.display.drawBitmap(helper * 2, 2, BToffIcon, TFT_BLACK, 2);
		// 	helper += 10;
		// }
		

		if(!mp.SDinsertedFlag)
		{
			mp.display.drawBitmap(helper * 2, 2, noSDIcon, TFT_BLACK, 2);
			helper+=10;
		}
		if(mp.carrierName != "")
		{
			// mp.display.setFreeFont(TT1);
			// mp.display.setTextSize(2);
			// mp.display.setCursor(helper*2, 15);

			mp.display.setTextFont(2);
			mp.display.setTextSize(1);
			mp.display.setCursor(helper*2, 4);
			mp.display.print(mp.carrierName);
		}

		if (mp.batteryVoltage > 4100)
			mp.display.drawBitmap(148, 2, batteryChargingIcon, TFT_BLACK, 2);
			
		else if (mp.batteryVoltage <= 4100 && mp.batteryVoltage >= 3850)
			mp.display.drawBitmap(148, 2, batteryFullIcon, TFT_BLACK, 2);

		else if (mp.batteryVoltage < 3850 && mp.batteryVoltage >= 3750)
			mp.display.drawBitmap(148, 2, batteryHighIcon, TFT_BLACK, 2);

		else if (mp.batteryVoltage < 3750 && mp.batteryVoltage >= 3650)
			mp.display.drawBitmap(148, 2, batteryMidIcon, TFT_BLACK, 2);

		else if (mp.batteryVoltage < 3650 && mp.batteryVoltage >= 3600)
			mp.display.drawBitmap(148, 2, batteryLowIcon, TFT_BLACK, 2);

		else if (mp.batteryVoltage < 3600)
			mp.display.drawBitmap(148, 2, batteryEmptyIcon, TFT_BLACK, 2);

		uint8_t temp = 0;
		for(int i = 0; i< sizeof(mp.notificationTypeList);i++)
		{
			if(mp.notificationTypeList[i] == 0)
			{
				temp = i;
				break;
			}
		}
		if(temp > 0)
			drawNotificationWindow(64,temp-1);
		if(temp > 1)
			drawNotificationWindow(88,temp-2);
		mp.display.setFreeFont(TT1);
		mp.display.setTextSize(4);
		if (millis() - elapsedMillis >= 500) {
			elapsedMillis = millis();
			blinkState = !blinkState;
			if (mp.clockYear%100 != 4 && mp.clockYear%100 != 80)
				mp.updateTimeRTC();
		}
		if (blinkState == 1)
		{
			mp.display.setCursor(2, 111);
			mp.display.setTextFont(2);
			mp.display.setTextSize(1);
			mp.display.setTextColor(TFT_BLACK);
			mp.display.print("Hold \"A\" to unlock");
			mp.display.setTextSize(6);
			mp.display.setTextColor(TFT_DARKGREY);
			mp.display.setFreeFont(TT1);
			mp.display.setCursor(58, 56);
			mp.display.print(":");
			mp.display.setTextColor(TFT_BLACK);
			mp.display.setCursor(56, 54);
			mp.display.print(":");
		}
		mp.update();

		mp.display.setTextSize(2);

		if (mp.buttons.pressed(BTN_A)) {
			mp.display.setTextSize(1);

			mp.display.fillRect(0, 112, mp.display.width(), 14, backgroundColors[mp.backgroundIndex]);
			mp.display.setCursor(2, 111);
			mp.display.setTextFont(2);
			mp.display.print("Unlocking");
			buttonHeld = millis();
			while (!mp.buttons.released(BTN_A))
			{

				if (mp.buttons.timeHeld(BTN_A) > 5 && mp.buttons.timeHeld(BTN_A) < 12) {
					mp.display.fillRect(0, 114, mp.display.width(), 14, backgroundColors[mp.backgroundIndex]);
					mp.display.setCursor(2, 111);
					mp.display.setTextFont(2);
					mp.display.print("Unlocking *");

					mp.leds[0] = CRGB::Red;
					mp.leds[7] = CRGB::Red;
					// FastLED.show();
				}
				else if (mp.buttons.timeHeld(BTN_A) >= 12 && mp.buttons.timeHeld(BTN_A) < 18)
				{
					mp.display.fillRect(0, 114, mp.display.width(), 14, backgroundColors[mp.backgroundIndex]);
					mp.display.setCursor(2, 111);
					mp.display.setTextFont(2);
					mp.display.print("Unlocking * *");
					mp.leds[0] = CRGB::Red;
					mp.leds[7] = CRGB::Red;
					mp.leds[1] = CRGB::Red;
					mp.leds[6] = CRGB::Red;
					// FastLED.show();
				}
				else if (mp.buttons.timeHeld(BTN_A) >= 18 && mp.buttons.timeHeld(BTN_A) < 24)
				{
					mp.display.fillRect(0, 114, mp.display.width(), 14, backgroundColors[mp.backgroundIndex]);
					mp.display.setCursor(2, 111);
					mp.display.setTextFont(2);
					mp.display.print("Unlocking * * *");
					mp.leds[0] = CRGB::Red;
					mp.leds[7] = CRGB::Red;
					mp.leds[1] = CRGB::Red;
					mp.leds[6] = CRGB::Red;
					mp.leds[2] = CRGB::Red;
					mp.leds[5] = CRGB::Red;
					// FastLED.show();
				}
				else if (mp.buttons.timeHeld(BTN_A) >= 24)
				{
					mp.display.fillRect(0, 114, mp.display.width(), 14, backgroundColors[mp.backgroundIndex]);
					mp.display.setCursor(2, 111);
					mp.display.setTextFont(2);
					mp.display.print("Unlocking * * * *");

					// FastLED.show();
					while(!mp.buttons.released(BTN_A))
					{
						mp.leds[0] = CRGB::Red;
						mp.leds[7] = CRGB::Red;
						mp.leds[1] = CRGB::Red;
						mp.leds[6] = CRGB::Red;
						mp.leds[2] = CRGB::Red;
						mp.leds[5] = CRGB::Red;
						mp.leds[3] = CRGB::Red;
						mp.leds[4] = CRGB::Red;
						while(!mp.update());
					}
					Serial.println(millis() - buttonHeld);
					return;
				}
				mp.update();
			}
		}
		// if (goOut == 1 && mp.buttons.released(BTN_A))
			// break;
	}
}
