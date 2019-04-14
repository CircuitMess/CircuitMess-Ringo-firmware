#include "phoneApp.h"
void phoneApp() {
    mp.dataRefreshFlag = 0;
    String callBuffer = "";
	char key = NO_KEY;
	mp.display.setTextWrap(0);
    mp.display.setTextFont(2);

	while (1)
	{
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setTextSize(1);
        mp.display.fillRect(0, 79, mp.display.width(), 26, TFT_DARKGREY);
        mp.display.setCursor(2, 112);
        mp.display.print("Press A to call");
        mp.display.setCursor(2, -1);
        mp.display.setTextColor(TFT_LIGHTGREY);
        mp.display.print("Dialer");
		mp.display.setTextColor(TFT_WHITE);


		key = mp.buttons.kpdNum.getKey();
		if (key == 'A') //clear number
			callBuffer = "";
		else if (key == 'C')
			callBuffer.remove(callBuffer.length()-1);
		else if (key != NO_KEY && key!= 'A' && key != 'C' && key != 'B' && key != 'D')
		{
			switch (key)
			{
				case '1':
					osc->note(C5,0.05);
					osc->play();
					break;
				case '2':
					osc->note(D5,0.05);
					osc->play();
					break;
				case '3':
					osc->note(E5,0.05);
					osc->play();
					break;
				case '4':
					osc->note(F5,0.05);
					osc->play();
					break;
				case '5':
					osc->note(G5,0.05);
					osc->play();
					break;
				case '6':
					osc->note(A5,0.05);
					osc->play();
					break;
				case '7':
					osc->note(B5,0.05);
					osc->play();
					break;
				case '8':
					osc->note(C6,0.05);
					osc->play();
					break;
				case '9':
					osc->note(D6,0.05);
					osc->play();
					break;
				case '*':
					osc->note(E6,0.05);
					osc->play();
					break;
				case '0':
					osc->note(F6,0.05);
					osc->play();
					break;
				case '#':
					osc->note(G6,0.05);
					osc->play();
					break;
			
				default:
					break;
			}
			callBuffer += key;
		}
        mp.display.setCursor(0, 76);
		mp.display.setTextSize(2);
		mp.display.print(callBuffer);

		if (mp.display.cursor_x + 4  >= mp.display.width())
		{
            mp.display.fillRect(0, 79, mp.display.width(), 26, TFT_DARKGREY);
            mp.display.setCursor(mp.display.width() - mp.display.cursor_x - 14, 76);
			mp.display.print(callBuffer);
		}


		if (mp.buttons.pressed(BTN_A))//initate call
		{
			callNumber(callBuffer);
			while (!mp.update());
			callBuffer = "";
		}
		if (mp.buttons.released(BTN_B)) //BACK BUTTON
			break;

		mp.update();
	}
}