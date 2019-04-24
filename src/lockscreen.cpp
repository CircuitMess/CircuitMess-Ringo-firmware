#include "lockscreen.h"
String monthsList[] PROGMEM = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
void drawNotificationWindow(uint8_t y, uint8_t index) {
	mp.display.setTextFont(2);
	mp.display.setTextSize(1);
	mp.display.setCursor(30, y + 2);
	mp.display.fillRoundRect(2, y + 2, 154, 20, 2, TFT_DARKGREY);
	mp.display.fillRoundRect(4, y, 154, 20, 2, TFT_WHITE);
	mp.display.print(mp.notificationDescriprionList[index]);
	switch (mp.notificationTypeList[index])
	{
		case NotificationType::Phone:
			mp.display.drawBitmap(7, y + 2, missedcall, TFT_RED, 2);
			break;
		case NotificationType::Messages:
			mp.display.drawBitmap(7, y + 2, newtext, TFT_RED, 2);
			break;
		case NotificationType::System:
			mp.display.drawBitmap(7, y + 2, systemNotification, TFT_RED, 2);
			break;
	}
	mp.display.setTextFont(1);
	mp.display.setCursor(120, y + 1);
	String temp = "";
	DateTime now = mp.notificationTimeList[index];
	if (now.hour() < 10)
		temp.concat("0");
	temp.concat(now.hour());
	temp.concat(":");
	if (now.minute() < 10)
		temp.concat("0");
	temp.concat(now.minute());
	mp.display.print(temp);
	mp.display.setCursor(119, y + 11);
	temp = "";
	temp.concat(monthsList[now.month() - 1]);
	temp.concat(" ");
	if (now.day() < 10)
		temp.concat("0");
	temp.concat(now.day());
	mp.display.print(temp);
}
void lockscreen() {
	mp.dataRefreshFlag = 1;
    bool blinkState = 0;
    bool goOut = 0;
	uint32_t elapsedMillis = millis();
    uint32_t buttonHeld;
    uint8_t pixelState;
    uint8_t scale;
	if(mp.resolutionMode)
		scale = 1;
	else
		scale = 2;

	FastLED.clear();

	while (1)
	{
		mp.display.fillScreen(backgroundColors[mp.backgroundIndex]);

		mp.display.setFreeFont(TT1);
		mp.display.setTextSize(3*scale);//
		//Hour shadow
		mp.display.setTextColor(TFT_DARKGREY);
		if (mp.clockHour == 11)
			mp.display.setCursor(10*scale, 28*scale);
		else if (mp.clockHour % 10 == 1 || (int)mp.clockHour / 10 == 1)
			mp.display.setCursor(7*scale, 28*scale);
		else
			mp.display.setCursor(4*scale, 28*scale);
		if (mp.clockHour < 10)
			mp.display.print("0");
		mp.display.print(mp.clockHour);

		//minute shadow
		mp.display.setCursor(36*scale, 28*scale);
		if (mp.clockMinute < 10)
			mp.display.print("0");
		mp.display.print(mp.clockMinute);

		//Hour black
		mp.display.setTextColor(TFT_BLACK);
		if (mp.clockHour == 11)
			mp.display.setCursor(9*scale, 27*scale);
		else if (mp.clockHour % 10 == 1 || (int)mp.clockHour / 10 == 1)
			mp.display.setCursor(6*scale, 27*scale);
		else
			mp.display.setCursor(3*scale, 27*scale);
		if (mp.clockHour < 10)
			mp.display.print("0");
		mp.display.print(mp.clockHour);

		//Minute black
		mp.display.setCursor(35*scale, 27*scale);
		if (mp.clockMinute < 10)
			mp.display.print("0");
		mp.display.print(mp.clockMinute);

		mp.display.setTextSize(1*scale);
		mp.display.setCursor(60*scale, 19*scale);
		mp.display.setTextWrap(false);
		if (mp.clockDay < 10)
			mp.display.print("0");
		mp.display.print(mp.clockDay);
		mp.display.print("/");
		if (mp.clockMonth < 10)
			mp.display.print("0");
		mp.display.print(mp.clockMonth);
		mp.display.setCursor(62*scale, 25*scale);
		mp.display.print(2000 + mp.clockYear);
		/*mp.display.setTextSize(2);
		mp.display.setCursor(10, 50);
		mp.display.print("12:00");*/
		uint8_t helper = 11;
		if (mp.simInserted && !mp.airplaneMode)
		{
			if (mp.signalStrength <= 3)
				mp.display.drawBitmap(1*scale, 1*scale, noSignalIcon, TFT_BLACK, scale);
			else if (mp.signalStrength > 3 && mp.signalStrength <= 10)
				mp.display.drawBitmap(1*scale, 1*scale, signalLowIcon, TFT_BLACK, scale);
			else if (mp.signalStrength > 10 && mp.signalStrength <= 20)
				mp.display.drawBitmap(1*scale, 1*scale, signalHighIcon, TFT_BLACK, scale);
			else if (mp.signalStrength > 20 && mp.signalStrength <= 31)
				mp.display.drawBitmap(1*scale, 1*scale, signalFullIcon, TFT_BLACK, scale);
			else if (mp.signalStrength == 99)
				mp.display.drawBitmap(1*scale, 1*scale, signalErrorIcon, TFT_BLACK, scale);
		}
		else if(!mp.simInserted && !mp.airplaneMode)
			mp.display.drawBitmap(1*scale, 1*scale, signalErrorIcon, TFT_BLACK, scale);
		if (mp.volume == 0)
		{
			mp.display.drawBitmap(helper*scale, 1*scale, silentmode, TFT_BLACK, scale);
			helper += 10;
		}
		//mp.display.drawBitmap(31, 1, missedcall);
		//mp.display.drawBitmap(41, 1, newtext);
		if (!mp.airplaneMode)
		{
			if (mp.wifi == 1)
				mp.display.drawBitmap(helper*scale, 1*scale, wifion, TFT_BLACK, scale);
			else
				mp.display.drawBitmap(helper*scale, 1*scale, wifioff, TFT_BLACK, scale);
			helper += 10;
			if (mp.bt)
				mp.display.drawBitmap(helper*scale, 1*scale, BTon, TFT_BLACK, scale);
			else
				mp.display.drawBitmap(helper*scale, 1*scale, BToff, TFT_BLACK, scale);
			helper += 10;
		}
		else
		{
			mp.display.drawBitmap(scale, scale, airplaneModeIcon, TFT_BLACK, scale);
			helper += 10;
		}

		if(!mp.SDinsertedFlag)
			mp.display.drawBitmap(helper*scale, 1*scale, noSDIcon, TFT_BLACK, scale);

		if (mp.batteryVoltage > 4000)
			mp.display.drawBitmap(74*scale, 1*scale, batteryCharging, TFT_BLACK, scale);
		else if (mp.batteryVoltage <= 4000 && mp.batteryVoltage >= 3800)
			mp.display.drawBitmap(74*scale, 1*scale, batteryFull, TFT_BLACK, scale);
		else if (mp.batteryVoltage < 3800 && mp.batteryVoltage >= 3700)
			mp.display.drawBitmap(74*scale, 1*scale, batteryMid, TFT_BLACK, scale);
		else if (mp.batteryVoltage < 3700 && mp.batteryVoltage >= 3600)
			mp.display.drawBitmap(74*scale, 1*scale, batteryMidLow, TFT_BLACK, scale);
		else if (mp.batteryVoltage < 3600 && mp.batteryVoltage >= 3500)
			mp.display.drawBitmap(74*scale, 1*scale, batteryLow, TFT_BLACK, scale);
		else if (mp.batteryVoltage < 3500)
			mp.display.drawBitmap(74*scale, 1*scale, batteryEmpty, TFT_BLACK, scale);
		bool anyNotifications = 0;
		uint8_t temp = 0;
		for(int i = 0; i< sizeof(mp.notificationTypeList);i++)
		{
			if(mp.notificationTypeList[i] != NotificationType::NONE)
			{
				temp++;
				if(temp == 1)
					drawNotificationWindow(64,i);
				else if(temp == 2)
					drawNotificationWindow(88,i);
				anyNotifications = 1;
			}
		}

		if(!anyNotifications)
		{
			           
		}
		// drawNotificationWindow(64,"Missed call from Dad");
		// drawNotificationWindow(88, "Text from Jack");
		mp.display.setFreeFont(TT1);
		mp.display.setTextSize(2*scale);
		if (millis() - elapsedMillis >= 500) {
			elapsedMillis = millis();
			blinkState = !blinkState;
			if (mp.clockYear != 4 && mp.clockYear != 80)
				mp.updateTimeRTC();
		}


		if (blinkState == 1)
		{
			mp.display.setTextSize(1);
			if(scale == 1)
			{
				mp.display.setCursor(1, 63);
				mp.display.setFreeFont(TT1);
			}
			else
			{
				/* mp.display.setCursor(2, 63*scale);
				mp.display.setFreeFont(FSS9); */
				mp.display.setCursor(2, 111);
				mp.display.setTextFont(2);
			}
			mp.display.setTextColor(TFT_BLACK);
			mp.display.print("Hold \"A\" to unlock");
			mp.display.setTextSize(3*scale);
			mp.display.setTextColor(TFT_DARKGREY);
			mp.display.setFreeFont(TT1);
			mp.display.setCursor(29*scale, 28*scale);
			mp.display.print(":");
			mp.display.setTextColor(TFT_BLACK);
			mp.display.setCursor(28*scale, 27*scale);
			mp.display.print(":");
		}
		mp.update();

		mp.display.setTextSize(1*scale);

		if (mp.buttons.pressed(BTN_A)) {
			mp.display.setTextSize(1);
			// vibration(200);

			mp.display.fillRect(0, 56*scale, mp.display.width(), 7*scale, backgroundColors[mp.backgroundIndex]);
			if(mp.resolutionMode)
			{
				mp.display.setCursor(1, 63);
				mp.display.setFreeFont(TT1);
			}
			else
			{
				mp.display.setCursor(2, 111);
				mp.display.setTextFont(2);
			}
			mp.display.print("Unlocking");
			mp.update();
			buttonHeld = millis();
			pixelState = 0;
			while (mp.buttons.kpd.pin_read(BTN_A) == 0)
			{

				if (millis() - buttonHeld > 250 && millis() - buttonHeld < 500) {
					mp.display.fillRect(0, 57*scale, mp.display.width(), 7*scale, backgroundColors[mp.backgroundIndex]);
					if(mp.resolutionMode)
					{
						mp.display.setCursor(1, 63);
						mp.display.setFreeFont(TT1);
					}
					else
					{
						mp.display.setCursor(2, 111);
						mp.display.setTextFont(2);
					}
					mp.leds[0] = CRGB::Red;
					mp.leds[7] = CRGB::Red;
					mp.display.print("Unlocking *");
				}
				else if (millis() - buttonHeld > 500 && millis() - buttonHeld < 750)
				{
					mp.display.fillRect(0, 57*scale, mp.display.width(), 7*scale, backgroundColors[mp.backgroundIndex]);
					if(mp.resolutionMode)
					{
						mp.display.setCursor(1, 63);
						mp.display.setFreeFont(TT1);
					}
					else
					{
						mp.display.setCursor(2, 111);
						mp.display.setTextFont(2);
					}
					mp.display.print("Unlocking * *");
					mp.leds[0] = CRGB::Red;
					mp.leds[7] = CRGB::Red;
					mp.leds[1] = CRGB::Red;
					mp.leds[6] = CRGB::Red;
				}
				else if (millis() - buttonHeld > 750 && millis() - buttonHeld < 1000)
				{
					mp.display.fillRect(0, 57*scale, mp.display.width(), 7*scale, backgroundColors[mp.backgroundIndex]);
					if(mp.resolutionMode)
					{
						mp.display.setCursor(1, 63);
						mp.display.setFreeFont(TT1);
					}
					else
					{
						mp.display.setCursor(2, 111);
						mp.display.setTextFont(2);
					}
					mp.display.print("Unlocking * * *");
					mp.leds[0] = CRGB::Red;
					mp.leds[7] = CRGB::Red;
					mp.leds[1] = CRGB::Red;
					mp.leds[6] = CRGB::Red;
					mp.leds[2] = CRGB::Red;
					mp.leds[5] = CRGB::Red;
				}
				else if (millis() - buttonHeld > 1000)
				{
					mp.leds[0] = CRGB::Red;
					mp.leds[7] = CRGB::Red;
					mp.leds[1] = CRGB::Red;
					mp.leds[6] = CRGB::Red;
					mp.leds[2] = CRGB::Red;
					mp.leds[5] = CRGB::Red;
					mp.leds[3] = CRGB::Red;
					mp.leds[4] = CRGB::Red;

					FastLED.show();

					while(!mp.update());
					if(mp.buttons.released(BTN_A))
					{
						// vibration(200);
						Serial.println(millis() - buttonHeld);
						goOut = 1;
						FastLED.clear();
						delay(10);
						break;
					}
				}
				mp.update();
			}
		}
		if (mp.buttons.released(BTN_A))
			FastLED.clear();
		if (mp.buttons.released(BTN_B)) {
			mp.sleep();
		}

		if (goOut == 1 && mp.buttons.released(BTN_A))
			break;
	}
}
