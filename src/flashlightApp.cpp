#include "flashlightApp.h"
void flashlightApp()
{
	char key = NO_KEY;
	bool state = 0;
	uint8_t color = 4;
	uint8_t localBrightness = 5;
	while(1)
	{
		key = mp.buttons.kpdNum.getKey();
		for(int i = 0; i < 8; i++)
		{
			switch(color)
			{
				case 0:
					mp.leds[i] = CRGB::Cyan;
					break;
				case 1:
					mp.leds[i] = CRGB::Green;
					break;
				case 2:
					mp.leds[i] = CRGB::Red;
					break;
				case 3:
					mp.leds[i] = CRGB::Yellow;
					break;
				case 4:
					mp.leds[i] = CRGB::White;
					break;
				case 5:
					mp.leds[i] = CRGB::Orange;
					break;
				case 6:
					mp.leds[i] = CRGB::Fuchsia;
					break;
			}
		}
		mp.pixelsBrightness = localBrightness;
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setTextColor(TFT_WHITE);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setCursor(2, 110);
		mp.display.print("Color");
		// mp.display.setCursor(95, 110);
		// mp.display.print("Brightness");
		if(!state)
		{
			mp.pixelsBrightness = 0;
			mp.display.drawIcon(flashlightOff, 46, 10, 34, 50, 2, TFT_GREEN);
		}
		else
			mp.display.drawIcon(flashlightOn, 46, 10, 34, 50, 2, TFT_GREEN);

		if(mp.buttons.released(BTN_A))
		{
			state = !state;
			Serial.println(state);
			delay(5);
			while(!mp.update());
		}
		if(mp.buttons.released(BTN_B))
			break;
		if(key == 'C')
		{
			while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
			{
				mp.display.fillRect(15, 51, 130, 24, backgroundColors[color]);
				mp.display.drawRect(14, 50, 132, 26, TFT_BLACK);
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setCursor(0, mp.display.height() / 2 - 10);
				mp.display.printCenter(backgroundColorsNames[color]);
				mp.display.drawBitmap(22, 57, arrowLeft, TFT_BLACK, 2);
				mp.display.drawBitmap(130, 57, arrowRight, TFT_BLACK, 2);
				if (millis() % 1000 <= 500)
				{
					if (color == 0)
					{
						mp.display.drawBitmap(130, 57, arrowRight, backgroundColors[color], 2);
						mp.display.drawBitmap(11*2, 57, arrowLeft, TFT_BLACK, 2);
						mp.display.drawBitmap(66*2, 57, arrowRight, TFT_BLACK, 2);
					}
					else if (color == 6)
					{
						mp.display.drawBitmap(22, 57, arrowLeft, backgroundColors[color], 2);
						mp.display.drawBitmap(10*2, 57, arrowLeft, TFT_BLACK, 2);
						mp.display.drawBitmap(65*2, 57, arrowRight, TFT_BLACK, 2);
					}
					else
					{
						mp.display.drawBitmap(130, 57, arrowRight, backgroundColors[color], 2);
						mp.display.drawBitmap(22, 57, arrowLeft, backgroundColors[color], 2);
						mp.display.drawBitmap(10*2, 57, arrowLeft, TFT_BLACK, 2);
						mp.display.drawBitmap(66*2, 57, arrowRight, TFT_BLACK, 2);
					}
				}
				if (mp.buttons.released(BTN_LEFT) && color > 0)
				{
					osc->note(75, 0.05);
					osc->play();
					color--;
					while(!mp.update());
				}
				if (mp.buttons.released(BTN_RIGHT) && color < 6)
				{
					osc->note(75, 0.05);
					osc->play();
					color++;
					while(!mp.update());
				}
				mp.update();
			}
			while(!mp.update());
		}
		mp.update();
	}
}