#include "calculatorApp.h"
void calculatorApp()
{
	String input = "0";
	double result = 0;
	double helper = 0;
	char key;
	uint8_t tempCursor;
	uint8_t cursor = 0;
	uint32_t blinkMillis = millis();
	uint32_t helpMillisOne;
	int8_t operation = -1;
	bool blinkState = 0;
	bool clear = 0;
	bool set = 0;

	while(1)
	{
		mp.display.fillScreen(0xA794);
		mp.display.drawRect(6,5, 149, 40, TFT_BLACK);
		mp.display.fillRect(7,6,147,38, TFT_LIGHTGREY);
		for (int y = 0; y < 2; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				mp.display.drawRect(8 + 37 * x, 49 + 27 * y, 33, 23, TFT_BLACK);
				mp.display.fillRect(9 + 37 * x, 50 + 27 * y, 31, 21, TFT_LIGHTGREY);
				switch (y*4 + x)
				{
					case 0:
						mp.display.drawBitmap(18, 54, calculatorPlus);
						break;
					case 1:
						mp.display.drawBitmap(55, 59, calculatorMinus);
						break;
					case 2:
						mp.display.drawBitmap(92, 54, calculatorMultiply);
						break;
					case 3:
						mp.display.drawBitmap(128, 54, calculatorDivide);
						break;
					case 4:
						mp.display.drawBitmap(15, 79, calculatorRoot);
						break;
					case 5:
						mp.display.drawBitmap(55, 80, calculatorPotency);
						break;
					case 6:
						mp.display.drawBitmap(87, 79, calculatorReciprocal);
						break;
					case 7:
						mp.display.drawBitmap(133, 91, calculatorDecimalPoint);
						break;
				}
			}
		}
		// mp.display.drawRect(119, 103, 33, 23, TFT_BLACK);
		// mp.display.fillRect(120, 104, 31, 21, 0x4D42);
		mp.display.setCursor(7, 110);
		mp.display.setTextFont(2);
		mp.display.setTextSize(1);
		mp.display.setTextColor(TFT_BLACK);
		mp.display.print("Erase");
		mp.display.setCursor(110, 110);
		mp.display.println("Equals");
		// key = mp.buttons.getKey();

		if(input.length() < 8 || clear) {
			for(int i = 0; i < 11; i++) {
				if(i == 9) {
					continue;
				}

				if(mp.buttons.released(i)) {
					if(i == 10) {
						key = '0';
					} else {
						key = '1' + i;
					}

					if(clear)	{
						input = "0";
						clear = 0;
					}
					if(input == "0") {
						input = "";
					}

					input.concat(key);
				}
			}
		}


		// if(key != NO_KEY && key > 47 && key < 58 && input.length() < 8)
		// {
		// 	if(clear)
		// 	{
		// 		input = "0";
		// 		clear = 0;
		// 	}
		// 	if(input == "0")
		// 		input = "";
		// 	input.concat(key);
		// }

		if(millis() - blinkMillis > 250)
		{
			blinkState = !blinkState;
			blinkMillis = millis();
		}
		mp.display.fillRect(7,6,147,38, TFT_LIGHTGREY);
		mp.display.setTextSize(2);
		mp.display.setTextFont(2);
		mp.display.setTextWrap(0);
		mp.display.setCursor(200, 200);
		tempCursor = mp.display.cursor_x;
		mp.display.print(input);
		tempCursor = mp.display.cursor_x - tempCursor;
		mp.display.setCursor(145-tempCursor, 7);
		mp.display.print(input);

		mp.display.drawRect(7 + 37 * (cursor % 4), 48 + 27 * (int)(cursor / 4), 35, 25, blinkState ? TFT_RED : 0xA794);
		mp.display.drawRect(6 + 37 * (cursor % 4), 47 + 27 * (int)(cursor / 4), 37, 27, blinkState ? TFT_RED : 0xA794);

		if(mp.buttons.held(BTN_FUN_LEFT, 40))
		{
			while(!mp.update());
			result = 0;
			operation = -1;
			input = "0";
			clear = 0;
			helper = 0;
		}
		if(mp.buttons.released(BTN_FUN_LEFT))
		{
			if(input == "Error") input = "";
			else input.remove(input.length() - 1);
			if(input == "")
				input = "0";
		}
		if(mp.buttons.released(BTN_HOME)) {
			mp.exitedLockscreen = true;
			mp.lockscreen();
		}
		if(mp.buttons.pressed(BTN_LEFT))
		{
			mp.display.drawRect(7 + 37 * (cursor % 4), 48 + 27 * (int)(cursor / 4), 35, 25, 0xA794);
			mp.display.drawRect(6 + 37 * (cursor % 4), 47 + 27 * (int)(cursor / 4), 37, 27, 0xA794);
			if(cursor%4 > 0)
				cursor--;
			else
				cursor += 3;
			blinkState = 1;
			blinkMillis = millis();
			helpMillisOne = millis();
		while(!mp.update());
		//while (helpMillisOne + 50 > millis());
		}
		if(mp.buttons.pressed(BTN_RIGHT))
		{
			mp.display.drawRect(7 + 37 * (cursor % 4), 48 + 27 * (int)(cursor / 4), 35, 25, 0xA794);
			mp.display.drawRect(6 + 37 * (cursor % 4), 47 + 27 * (int)(cursor / 4), 37, 27, 0xA794);
			if(cursor%4 < 3)
				cursor++;
			else
				cursor -= 3;
			blinkState = 1;
			blinkMillis = millis();
			while(!mp.update());
		}
		if(mp.buttons.pressed(BTN_DOWN))
		{
			mp.display.drawRect(7 + 37 * (cursor % 4), 48 + 27 * (int)(cursor / 4), 35, 25, 0xA794);
			mp.display.drawRect(6 + 37 * (cursor % 4), 47 + 27 * (int)(cursor / 4), 37, 27, 0xA794);
			if((int)(cursor/4) < 1)
				cursor += 4;
			else
				cursor -= 4;
			blinkState = 1;
			blinkMillis = millis();
			while(!mp.update());
		}
		if(mp.buttons.pressed(BTN_UP))
		{
			mp.display.drawRect(7 + 37 * (cursor % 4), 48 + 27 * (int)(cursor / 4), 35, 25, 0xA794);
			mp.display.drawRect(6 + 37 * (cursor % 4), 47 + 27 * (int)(cursor / 4), 37, 27, 0xA794);
			if((int)(cursor/4) > 0)
				cursor -= 4;
			else
				cursor += 4;
			blinkState = 1;
			blinkMillis = millis();
			while(!mp.update());
		}
		if(mp.buttons.released(BTN_A))
		{
			if(cursor == 4)
			{
				result = input.toDouble();
				// Serial.println(sqrt((double)(result)));
				result = sqrt(result);
				if(result == int(result))
					input = String((int)(round(result)));
				else
					input = String(result);
			}
			else if(cursor == 6)
			{
				result = input.toDouble();
				result = 1/result;
				if(result == int(result))
					input = String((int)(round(result)));
				else
					input = String(result);
			}
			else if(cursor == 7 && input.indexOf('.') == -1 && input.length() < 8)
			{
				if(input == "" || clear)
				{
					input = "0";
					clear = 0;
				}
				input.concat('.');
			}
			else if(operation == -1 && cursor != 4 && cursor != 6)
			{
				clear = 1;
				result = input.toDouble();
				set = 0;
				operation = cursor;
			}
			else if(operation > -1 && cursor != 4 && cursor != 6)
			{
				if(!set)
				{
					helper = input.toDouble();
					set = 1;
				}

				set = 0;
				if(operation == cursor && !clear)
				{
					switch (operation)
					{
						case 0:
							result += helper;
							break;
						case 1:
							result -= helper;
							break;
						case 2:
							result *= helper;
							break;
						case 3:
							result /= helper;
							break;
						case 5:
							result = pow(result, helper);
							break;
					}
					if(result == int(result))
						input = String((int)(round(result)));
					else
						input = String(result);
					set = 1;
					clear = 1;
				}
				else
					operation = cursor;

			}

			while(!mp.update());
			Serial.println(operation);
		}
		if(mp.buttons.released(BTN_FUN_RIGHT))
		{
			while(!mp.update());
			if(operation != -1 && !clear)
			{
				if(!set)
				{
					helper = input.toDouble();
					set = 1;
				}
				set = 0;
				switch (operation)
				{
					case 0:
						result += input.toDouble();
						break;
					case 1:
						result -= input.toDouble();
						break;
					case 2:
						result *= input.toDouble();
						break;
					case 3:
						result /= input.toDouble();
						break;
					case 5:
						result = pow(result, helper);
						break;
				}
				if(result == int(result))
					input = String((int)(round(result)));
				else
					input = String(result);
				operation = -1;
			}
		}
		if(input.length() > 8 || input == "inf")
		{
			input = "Error";
			clear = 1;
		}
		if(mp.buttons.released(BTN_B))
			break;
		helpMillisOne = millis();
		mp.update();
		while (helpMillisOne + 50 > millis());
	}
	while(!mp.update());
}
