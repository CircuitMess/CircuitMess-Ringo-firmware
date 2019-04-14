#include "calendarApp.h"
void calendarApp()
{
	int startDay = 0; // Sunday's value is 0, Saturday is 6
	int8_t days[5][7] = {
		{-1, -1, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1, -1, -1}
		};
	String week6 = "";
	int newWeekStart = 0; // used to show start of next week of the month
	String monthNames PROGMEM = "JanFebMarAprMayJunJulAugSepOctNovDec";
	static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	int monthLength = 0;
	uint8_t offset = 7;
	mp.updateTimeRTC();
	uint8_t month = mp.clockMonth;
	uint16_t year = mp.clockYear + 2000;

	while(!mp.buttons.released(BTN_B))
	{	
		//printing the month
		{
			mp.display.setTextFont(2);
			mp.display.fillScreen(TFT_WHITE);
			mp.display.setTextColor(TFT_BLACK);
			mp.display.drawString("Mo Tu We Th Fr Sa Su", 5,9 - offset); 
			if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12){
				monthLength = 31;
			}
			else {monthLength = 30;}
			if(month == 2){monthLength = 28;}

			int y = year -  (month < 3);
			startDay = (y +y/4 -y/100 + y/400 + t[month-1] + 1)% 7; // Sunday's value is 0
			// now build first week string
			switch (startDay){
				case 1:
				// Monday
				for (int i = 1; i < 8; i++)
					days[0][i-1] = i;
				break;

				case 2:
				// Tuesday
				for (int i = 1; i < 7; i++)
					days[0][i] = i;

				break;   

				case 3:
				// Wednesday
				for (int i = 1; i < 6; i++)
					days[0][i+1] = i;

				break;  

				case 4:
				// Thursday
				for (int i = 1; i < 5; i++)
					days[0][i+2] = i;
				break; 

				case 5:
				// Friday
				for (int i = 1; i < 4; i++)
					days[0][i+3] = i;
				break;

				case 6:
				// Saturday
				for (int i = 1; i < 3; i++)
					days[0][i+4] = i;
				if(monthLength == 31)
					week6 = "31";
				break; 

				case 0:
				// Sunday
				days[0][6] = 1;
				if(monthLength == 30){week6 = "30";}      
				else if(monthLength == 31){week6 = "30 31";}     

				break;           
			} // end first week
			startDay += 6;
			if(startDay > 6)
				startDay %= 7;
		
			newWeekStart = (7-startDay) + 1;
			for (int f = newWeekStart; f < newWeekStart + 7; f++)
				days[1][f - newWeekStart] = f;  

			newWeekStart = (14-startDay)+1; 
			for (int f = newWeekStart; f < newWeekStart + 7; f++)
				days[2][f - newWeekStart] = f;

			newWeekStart = (21-startDay)+1; 
			for (int f = newWeekStart; f < newWeekStart + 7; f++)
				days[3][f - newWeekStart] = f;

			newWeekStart = (28-startDay)+1;   
			// is is February?
			if(newWeekStart > 28 && month == 2){
			// do nothing unless its a leap year
				if (year==(year/4)*4) // its a leap year
					days[4][0] = 29;
			}
			else{ // print up to 30 anyway
				if(month == 2){  // its February
					for (int f = newWeekStart; f < 29; f++)
						days[4][f - newWeekStart] = f;
					// is it a leap year
					if (year==(year/4)*4) // its a leap year
						days[4][29 - newWeekStart] = 29;
				}
				else{
					for (int f = newWeekStart; f < 31; f++)
						days[4][f - newWeekStart] = f;
					// are there 31 days
					if (monthLength == 31 && days[4][6] == -1)
						days[4][31 - newWeekStart] = 31;
				} 
			}
			mp.display.setCursor(6, 100 - offset);
			if(week6 != "")
			{
				mp.display.print(week6.c_str());
				week6 = "";
			}
			mp.display.setCursor(0, 112);
			mp.display.printCenter(String(monthNames.substring((month - 1) * 3, month * 3)) + " " + year);
			mp.display.fillRect(2 + 111, 18, 44, 90, TFT_LIGHTGREY);
			for (int y = 0; y < 5;y++)
			{
				mp.display.drawFastHLine(2, 18 + 15 * y, 155, TFT_BLACK);
				for(int x = 0; x<7;x++)
				{
					mp.display.drawFastVLine(2 + 22 * x, 18, 90, TFT_BLACK);
					if(month == mp.clockMonth && days[y][x] == mp.clockDay && year == mp.clockYear + 2000 && millis()%500 >= 250)
						mp.display.fillRect(3 + 22 * x, 19 + 15 * y, 21, 14, TFT_RED);
					mp.display.setCursor(6 + 22 * x, 18 + 15 * y);
					if(days[y][x] < 10)
						mp.display.setCursor(6 + 4 + 22 * x, 18 + 15 * y);
					if(days[y][x] > 0)
						mp.display.print(days[y][x]);
					// Serial.print(days[y][x]);
					// Serial.print("|");
					days[y][x] = -1;
				}
				// Serial.print("\n-----------\n");
				// delay(5);
			}
			mp.display.drawFastVLine(2 + 154, 18, 90, TFT_BLACK);
			mp.display.drawFastHLine(2, 18 + 75, 155, TFT_BLACK);
			mp.display.drawFastHLine(2, 18 + 90, 155, TFT_BLACK);

		} 
		 if(mp.buttons.released(BTN_LEFT))
		{
			while(!mp.update());
			if(month > 1)
				month--;
			else
			{
				month = 12;
				year--;
			}
		}
		if(mp.buttons.released(BTN_RIGHT))
		{
			while(!mp.update());
			if(month < 12)
				month++;
			else
			{
				month = 1;
				year++;
			}
		}
		mp.update();
	}
	while(!mp.update());
}
