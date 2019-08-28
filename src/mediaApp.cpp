#include "mediaApp.h"
// uint8_t cursor = 0;
// int32_t cameraY = 0;
// int32_t cameraY_actual = 0;
String mediaItems[3] PROGMEM = {
    "Music",
    "Photo",
    "Video",
};
String photoFiles[100];
uint8_t photoCount = 0;
bool firstPass = 0;
MPTrack* mp3;

//Media app

void mediaApp() {
	mp.dataRefreshFlag = 0;
	while(1)
	{
		int8_t input = mediaMenu(mediaItems, 2);

		if(input == 0) //music
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
			
			if(audioCount > 0)
			{
				int16_t index =0;
				while (1)
				{
					index = audioPlayerMenu("Select file to play:", audioFiles, audioCount, index);
					if (index == -1)
						break;
					mp.display.fillScreen(TFT_LIGHTGREY);
					Serial.print("before audio player: ");
					Serial.println(ESP.getFreeHeap());
					delay(5);
					index = audioPlayer(index);
					if(!mp.SDinsertedFlag)
						break;
					Serial.print("after audio player: ");
					Serial.println(ESP.getFreeHeap());
					delay(5);
				} 
			}
			else
			{
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setTextColor(TFT_WHITE);
				mp.display.setCursor(0, mp.display.height()/2 - 16);
				mp.display.setTextFont(2);
				mp.display.printCenter(F("No audio files!"));
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
		else if(input == 1) //photos
		{
			mp.display.setTextColor(TFT_BLACK);
			mp.display.setTextSize(1);
			mp.display.setTextFont(2);
			mp.display.drawRect(14, 45, 134, 38, TFT_BLACK);
			mp.display.drawRect(13, 44, 136, 40, TFT_BLACK);
			mp.display.fillRect(15, 46, 132, 36, TFT_WHITE);
			mp.display.setCursor(47, 55);
			mp.display.printCenter(F("Loading photos"));
			while(!mp.update());
			listPhotos("/Images", 0);
			if(photoCount > 0)
			{
				int16_t index = 0;
				while (1)
				{
					index = audioPlayerMenu("Select photo to open:", photoFiles, photoCount, index);
					if (index == -1)
						break;
	
					if(photoFiles[index].endsWith(".bmp") || photoFiles[index].endsWith(".BMP"))
						mp.display.drawBmp(photoFiles[index], 0,0);
					else
						mp.drawJpeg(photoFiles[index], 0, 0);
					while(!mp.update());
					bool firstRefresh = 1;
					while(1)
					{
						if(mp.buttons.released(BTN_HOME)) {
						mp.exitedLockscreen = true;
						mp.lockscreen(); // Robert
						}
						if(mp.buttons.released(BTN_A) || mp.buttons.released(BTN_B) || !mp.SDinsertedFlag)
							break;
						if(mp.buttons.released(BTN_LEFT))
						{
							firstRefresh = 1;
							if(index > 0)
								index--;
							else
								index = photoCount - 1;
							if(photoFiles[index].endsWith(".bmp") || photoFiles[index].endsWith(".BMP"))
								mp.display.drawBmp(photoFiles[index], 0,0);
							else
								mp.drawJpeg(photoFiles[index], 0, 0);
							while(!mp.update());
						}
						if(mp.buttons.released(BTN_RIGHT))
						{
							firstRefresh = 1;
							if(index < photoCount - 1)
								index++;
							else
								index = 0;
							if(photoFiles[index].endsWith(".bmp") || photoFiles[index].endsWith(".BMP"))
								mp.display.drawBmp(photoFiles[index], 0,0);
							else
								mp.drawJpeg(photoFiles[index], 0, 0);
							while(!mp.update());
						}
						if(mp.buttons.holdForUnlock && firstRefresh)
						{
							firstRefresh = 0;
							if(photoFiles[index].endsWith(".bmp") || photoFiles[index].endsWith(".BMP"))
								mp.display.drawBmp(photoFiles[index], 0,0);
							else
								mp.drawJpeg(photoFiles[index], 0, 0);
							while(!mp.update());
						}
						mp.update();
					}
					Serial.println(F("out"));
					while(!mp.update());
				} 
			}
			else
			{
				mp.display.fillScreen(TFT_BLACK);
				mp.display.setTextColor(TFT_WHITE);
				mp.display.setCursor(0, mp.display.height()/2 - 16);
				mp.display.setTextFont(2);
				mp.display.printCenter(F("No photos found!"));
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
			while(!mp.update());
		}
		else if(input == -1)
			break;
	}
}
int8_t mediaMenu(String* title, uint8_t length) {
	bool pressed = 0;
	int8_t cursor = 0;
	int32_t cameraY = 0;
	int32_t cameraY_actual = 0;
	mp.dataRefreshFlag = 0;
	bool blinkState = 0;
	uint32_t blinkMillis = millis();
	uint8_t boxHeight;
	boxHeight = 54; //actually 2 less than that
	while (1) {
		if(!mp.SDinsertedFlag)
			return -1;
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
			mediaMenuDrawBox(title[i], i, cameraY_actual);
		}
		if(blinkState)
			mediaMenuDrawCursor(cursor, cameraY_actual, pressed);

		if(mp.buttons.released(BTN_HOME)) {
			mp.exitedLockscreen = true;
			mp.lockscreen(); // Robert
		}
		if (!mp.buttons.pressed(BTN_DOWN) && !mp.buttons.pressed(BTN_UP))
			pressed = 0;

		if (mp.buttons.released(BTN_A)) {   //BUTTON CONFIRM
			mp.osc->note(75, 0.05);
			mp.osc->play();
			while(!mp.update());// Exit when pressed
			break;
		}

		if (mp.buttons.released(BTN_UP)) {  //BUTTON UP
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

		if (mp.buttons.released(BTN_DOWN)) { //BUTTON DOWN
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


		if (mp.buttons.released(BTN_B) == 1) //BUTTON BACK
		{
			return -1;
		}
	}

	return cursor;

}
void mediaMenuDrawBox(String title, uint8_t i, int32_t y) {
	uint8_t scale;
	uint8_t boxHeight;
	uint8_t offset = 11;
	scale = 2;
	boxHeight = 54;
	y += i * boxHeight + offset;
	if (y < 0 || y > mp.display.width()) {
		return;
	}


	if (title == "Music") //red
	{
		mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2, 0xFC92);
		mp.display.drawIcon(mediaMusicIcon, 4, y + 3, 24,24,2);
	}
	if (title == "Photo") //blue
	{
		mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2, 0x963F);
		mp.display.drawIcon(mediaPhotoIcon, 4, y + 3, 24,24,2, 0xFC92);
	}
	// if (title == "Video") //yellow
	// {
	// 	mp.display.fillRect(2, y + 1, mp.display.width() - 4, boxHeight-2, TFT_DARKGREY);
	// }
	mp.display.setTextColor(TFT_BLACK);
	mp.display.setTextSize(2);
	mp.display.setTextFont(2);
	mp.display.drawString(title, 60, y +  10);
	mp.display.setTextColor(TFT_WHITE);
	mp.display.setTextSize(1);
}
void mediaMenuDrawCursor(uint8_t i, int32_t y, bool pressed) {
	uint8_t boxHeight;
	boxHeight = 54;
	uint8_t offset = 11;
	y += i * boxHeight + offset;
	mp.display.drawRect(0, y-1, mp.display.width()-1, boxHeight+2, TFT_RED);
	mp.display.drawRect(1, y, mp.display.width()-3, boxHeight, TFT_RED);
}



int16_t audioPlayer(uint16_t index) {
	uint8_t scale= 2;
	bool playState = 1;
	bool loop = 0;
	bool shuffleReset = 0;
	bool allTrue = 0;
	bool shuffle = 0;
	bool shuffleList[audioCount];
	if(!firstPass)
	{
		mp3 = new MPTrack((char*)audioFiles[index].c_str());
		addTrack(mp3);
	}
	uint8_t isPlaying[100];
	while(1)
	{
		bool trackAdded = 0;
		for(uint8_t i = 0; i < 4;i++)
		{
			if(tracks[i] == mp3)
			{
				trackAdded = 1;
				break;
			}
		}
		if(!trackAdded && mp.SDinsertedFlag)
		{
			addTrack(mp3);
			Serial.println("Track added");
		}
		else if(!mp.SDinsertedFlag)
			return 0;
		uint8_t x = 1;
		uint8_t y = 53;
		int8_t i = 0;
		
		if(firstPass)
		{
			if(mp3->reloadFile((char*)audioFiles[index].c_str()))
				Serial.println("OK");
			else
				Serial.println("ERROR");
		}
		else
			firstPass = 1;
		
		if(playState);
			mp3->play(); // Robert v2 dodao if
			mp.osc->setVolume(0); // Robert v1 -- turn off menu sounds while music is playing
		if(mp.mediaVolume == 0)
			mp3->setVolume(0);
		else
			mp3->setVolume(mp.mediaVolume*21);

		while (1) 
		{
			mp.display.fillScreen(backgroundColors[mp.backgroundIndex]);
			mp.display.drawBitmap(2*scale, 20*scale, previous, TFT_BLACK, scale);
			mp.display.drawBitmap(65*scale, 20*scale, next, TFT_BLACK, scale);
			mp.display.drawBitmap(2*scale, 75, repeatSprite, TFT_BLACK, scale);
			mp.display.drawBitmap(66*scale, 37*scale, shuffleIcon, TFT_BLACK, scale);
			if(playState)
				mp.display.drawBitmap(37*scale, 19*scale, play, TFT_BLACK, scale);
			else
				mp.display.drawBitmap(72, 40, pause2, TFT_BLACK, scale);
			mp.display.drawBitmap(17*scale, 2*scale, cover2, TFT_BLACK, scale);
			mp.display.setTextColor(TFT_BLACK);


			mp.display.setTextSize(1);
			mp.display.setTextWrap(0);
			mp.display.drawBitmap(4,2, fullSound, TFT_BLACK, 2);
			mp.display.setCursor(24, 2);
			mp.display.print(mp.mediaVolume);
			mp.display.setCursor(1, 111);
			if(audioFiles[index].length() > 20)
				mp.display.print(audioFiles[index]);
			else
				mp.display.printCenter(audioFiles[index]);
			mp.display.fillRect(141,74, 4,4, shuffle ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
			mp.display.fillRect(14,73, 4,4, loop ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
			
			bool trackAdded = 0;
			for(uint8_t i = 0; i < 4;i++)
			{
				if(tracks[i] == mp3)
				{
					trackAdded = 1;
					break;
				}
			}
			if(!trackAdded && mp.SDinsertedFlag)
			{
				addTrack(mp3);
				Serial.println("Track added");
			}
			else if(!mp.SDinsertedFlag)
				return 0;
			if (mp.buttons.released(BTN_B))
			{
				mp.buttons.update();
			//	mp3->stop();
				Serial.println(F("Stopped"));
				delay(5);
				return index;
			}
			if(mp.buttons.released(BTN_HOME)) {
			mp.exitedLockscreen = true;
			mp.lockscreen(); // Robert
			}

			if (mp.buttons.released(BTN_A)) //PLAY/PAUSE BUTTON
			{
				mp.buttons.update();
				if(playState)
				{
					mp3->pause();
					//mp.osc->setVolume(mp.oscillatorVolumeList[mp.mediaVolume]); // Robert v1 -- turns on menu sounds when song stops playing
					mp.display.fillScreen(backgroundColors[mp.backgroundIndex]);

					//draw bitmaps
					mp.display.drawBitmap(2*scale, 20*scale, previous, TFT_BLACK, scale);
					mp.display.drawBitmap(65*scale, 20*scale, next, TFT_BLACK, scale);
					mp.display.drawBitmap(2*scale, 75, repeatSprite, TFT_BLACK, scale);
					mp.display.drawBitmap(66*scale, 37*scale, shuffleIcon, TFT_BLACK, scale);
					mp.display.drawBitmap(72, 40, pause2, TFT_BLACK, scale);
					mp.display.drawBitmap(17*scale, 2*scale, cover2, TFT_BLACK, scale);

					//prepare for text printing
					mp.display.setTextColor(TFT_BLACK);
					mp.display.setTextSize(1);
					mp.display.setTextWrap(0);

					//drawtext
					mp.display.setCursor(1, 111);
					if(audioFiles[index].length() > 20)
						mp.display.print(audioFiles[index]);
					else
						mp.display.printCenter(audioFiles[index]);
					mp.display.drawBitmap(4,2, fullSound, TFT_BLACK, 2);
					mp.display.setCursor(24, 2);
					mp.display.print(mp.mediaVolume);
					mp.display.fillRect(141,74, 4,4, shuffle ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
					mp.display.fillRect(14,73, 4,4, loop ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
				
				}
				else
				{
					shuffleReset = 0;
					mp.display.fillScreen(backgroundColors[mp.backgroundIndex]);

					//draw bitmaps
					mp.display.drawBitmap(2*scale, 20*scale, previous, TFT_BLACK, scale);
					mp.display.drawBitmap(65*scale, 20*scale, next, TFT_BLACK, scale);
					mp.display.drawBitmap(2*scale, 75, repeatSprite, TFT_BLACK, scale);
					mp.display.drawBitmap(66*scale, 37*scale, shuffleIcon, TFT_BLACK, scale);
					mp.display.drawBitmap(37*scale, 19*scale, play, TFT_BLACK, scale);
					mp.display.drawBitmap(17*scale, 2*scale, cover2, TFT_BLACK, scale);



					//prepare for text printing
					mp.display.setTextColor(TFT_BLACK);
					mp.display.setTextSize(1);
					mp.display.setTextWrap(0);

					//drawtext
					mp.display.setCursor(1, 111);
					if(audioFiles[index].length() > 20)
						mp.display.print(audioFiles[index]);
					else
						mp.display.printCenter(audioFiles[index]);
					mp.display.drawBitmap(4,2, fullSound, TFT_BLACK, 2);
					mp.display.setCursor(24, 2);
					mp.display.print(mp.mediaVolume);
					mp.display.fillRect(141,74, 4,4, shuffle ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
					mp.display.fillRect(14,73, 4,4, loop ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
					mp3->resume();
				}
				playState = !playState;
				isPlaying[index] = 1; // Robert v2 zadnje
			}

			if (mp.buttons.released(BTN_DOWN) && mp.mediaVolume > 0) //DOWN
			{
				mp.buttons.update();
				mp.mediaVolume--;
				if(mp.mediaVolume == 0)
					mp3->setVolume(0);
				else
				mp3->setVolume(mp.mediaVolume*21);
				
				// mp3->setVolume(256/14*mp.mediaVolume);
				mp.osc->setVolume(mp.oscillatorVolumeList[mp.mediaVolume]);
				//prepare for text printing
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextFont(2);
				mp.display.setTextSize(1);
				mp.display.setTextWrap(0);
				mp.display.fillRect(4,2, 15, 15, backgroundColors[mp.backgroundIndex]);
				//drawtext
				mp.display.drawBitmap(4,2, fullSound, TFT_BLACK, 2);
				mp.display.setCursor(24, 2);
				mp.display.print(mp.mediaVolume);
			}
			if (mp.buttons.repeat(BTN_DOWN,2) && mp.mediaVolume > 0) //DOWN
			{
				mp.buttons.update();
				mp.mediaVolume--;
				if(mp.mediaVolume == 0)
					mp3->setVolume(0);
				else
				mp3->setVolume(mp.mediaVolume*21);
				
				// mp3->setVolume(256/14*mp.mediaVolume);
				mp.osc->setVolume(mp.oscillatorVolumeList[mp.mediaVolume]);
				//prepare for text printing
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextFont(2);
				mp.display.setTextSize(1);
				mp.display.setTextWrap(0);
				mp.display.fillRect(4,2, 15, 15, backgroundColors[mp.backgroundIndex]);
				//drawtext
				mp.display.drawBitmap(4,2, fullSound, TFT_BLACK, 2);
				mp.display.setCursor(24, 2);
				mp.display.print(mp.mediaVolume);
			}

			if (mp.buttons.released(BTN_UP) && mp.mediaVolume < 14) //UP
			{
				mp.buttons.update();
				mp.mediaVolume++;
				if(mp.mediaVolume == 0)
					mp3->setVolume(0);
				else
					mp3->setVolume(mp.mediaVolume*21);
				// mp3->setVolume(256/14*mp.mediaVolume);
				mp.osc->setVolume(mp.oscillatorVolumeList[mp.mediaVolume]);
				//prepare for text printing
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextFont(2);
				mp.display.setTextSize(1);
				mp.display.setTextWrap(0);
				mp.display.fillRect(4,2, 15, 15, backgroundColors[mp.backgroundIndex]);
				//drawtext
				mp.display.drawBitmap(4,2, fullSound, TFT_BLACK, 2);
				mp.display.setCursor(24, 2);
				mp.display.print(mp.mediaVolume);
			}
			if (mp.buttons.repeat(BTN_UP,2) && mp.mediaVolume < 14) //UP
			{
				mp.buttons.update();
				mp.mediaVolume++;
				if(mp.mediaVolume == 0)
					mp3->setVolume(0);
				else
					mp3->setVolume(mp.mediaVolume*21);
				// mp3->setVolume(256/14*mp.mediaVolume);
				mp.osc->setVolume(mp.oscillatorVolumeList[mp.mediaVolume]);
				//prepare for text printing
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextFont(2);
				mp.display.setTextSize(1);
				mp.display.setTextWrap(0);
				mp.display.fillRect(4,2, 15, 15, backgroundColors[mp.backgroundIndex]);
				//drawtext
				mp.display.drawBitmap(4,2, fullSound, TFT_BLACK, 2);
				mp.display.setCursor(24, 2);
				mp.display.print(mp.mediaVolume);
			}
			if(mp.buttons.released(BTN_LEFT)) //previous
			{
				Serial.println("previous");
				mp.buttons.update();
				isPlaying[index] = 0; // Robert v2
				playState = 1;
				if(shuffle && !shuffleReset)
				{
					bool allTrue=1;
					for(int i = 0; i < audioCount;i++)
						if(!shuffleList[i])
							allTrue = 0;
					if(allTrue)
					{
						memset(shuffleList, 0, sizeof(shuffleList));
						if(!loop)
						{
							playState = 0; // Robert -- added 1
							shuffleReset = 1; // Robert -- added 0
						}
					}
					uint16_t randNumber = random(0,audioCount);
					
					while(shuffleList[randNumber])
						randNumber = random(0,audioCount);
					index = randNumber;
					shuffleList[randNumber] = 1;
				}
				else
				{
					if(!loop && !shuffleReset)
					{
						if(index > 0)
							index--;
						else
						{
							playState = 1; // Robert dodao 1
							index = audioCount-1;
						}
					}
				}
				if(shuffleReset)
					playState = 1;
				mp3->stop();
				break;
			}
			if(mp.buttons.released(BTN_RIGHT)) //next
			{
				Serial.println("next");
				mp.buttons.update();
				playState = 1;
				isPlaying[index] = 0; // Robert v2
				if(shuffle && !shuffleReset)
				{
					bool allTrue=1;
					for(int i = 0; i < audioCount;i++)
						if(!shuffleList[i])
							allTrue = 0;
					if(allTrue)
					{
						memset(shuffleList, 0, sizeof(shuffleList));
						if(!loop)
						{
							playState = 1;
							shuffleReset = 0; // Robert - added 0
						}
					}
					uint16_t randNumber = random(0,audioCount);
					
					while(shuffleList[randNumber] == 1 || index == randNumber)
						randNumber = random(0,audioCount);
					index = randNumber;
					shuffleList[randNumber] = 1;
				}
				else if(!shuffle && !shuffleReset)
				{
					if(!loop && !shuffleReset)
					{
						if(index < audioCount - 1)
							index++;
						else
						{
							shuffleReset = 0; // Robert - was 1, now 0
							index = 0;
							playState = 1; // Robert - added 1
						}
					}
					// index = (index < audioCount - 1) ? index++ : 0;
				}
				if(shuffleReset)
					playState = 1;
				mp3->stop();
				break;
			}
			if(mp.buttons.released(BTN_FUN_RIGHT)) //shuffle button
			{
				mp.buttons.update();
				if(!shuffle)
					memset(shuffleList, 0, sizeof(shuffleList));
				shuffle = !shuffle;
				mp.display.fillRect(141,74, 4,4, shuffle ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
				mp.display.fillRect(141,74, 4,4, shuffle ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
				shuffleList[index] = 1;

			}
			if(mp.buttons.released(BTN_FUN_LEFT)) //loop button
			{
				mp.buttons.update();
				loop = !loop;
				mp.display.fillRect(14,73, 4,4, loop ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
				mp.display.fillRect(14,73, 4,4, loop ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
			}
			// if(mp.buttons.getJoystickX() == 1104)
			// 	Serial.print("joystick X break ");
			// if(mp.buttons.getJoystickY() == 1104)
			// 	Serial.print("joystick Y break ");
			Serial.print("joystick X: ");
			Serial.println(mp.buttons.getJoystickX());
			Serial.print("joystick Y: ");
			Serial.println(mp.buttons.getJoystickY());
			Serial.println("------------------");
			mp.update(1);
			if(mp3->isPlaying() < 1 && playState) //if the current song is finished, play the next one
			{
				Serial.println("song end");
				if(shuffle && !shuffleReset)
				{
					allTrue=1;
					for(int i = 0; i < audioCount;i++)
						if(!shuffleList[i])
							allTrue = 0;
					if(allTrue)
					{
						memset(shuffleList, 0, sizeof(shuffleList));
						if(!loop)
						{
							playState = 1; // Robert added 1
							shuffleReset = 0; // Robert addded 0
						}
					}
					uint16_t randNumber = random(0,audioCount);
					
					while(shuffleList[randNumber] || index == randNumber)
						randNumber = random(0,audioCount);
					index = randNumber;
					shuffleList[randNumber] = 1;
				}
				else if(!shuffle && !shuffleReset)
				{
					if(!loop)
					{
						if(index < audioCount - 1)
							index++;
						else
						{
							index = 0;
							playState = 1; // Robert -- was 0 -- trying to countinue at the end of the playlist
						}
					}
				}
				break;
			}
		}
	}
}
void listPhotos(const char * dirname, uint8_t levels) {
	photoCount = 0;
	
	Serial.printf("Listing directory: %s\n", dirname);
	File root = SD.open(dirname);
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
	File file = root.openNextFile();
	while (file) {
		char temp[100];
		// file.getName(temp, 100);
		String Name(file.name());
		Serial.println(Name);
		if (Name.endsWith(F(".jpeg")) || Name.endsWith(F(".JPEG"))
		|| Name.endsWith(F(".jpg")) || Name.endsWith(F(".JPG"))
		|| Name.endsWith(F(".bmp")) || Name.endsWith(F(".BMP")))
		{
			Serial.print(counter);
			Serial.print(F(".   "));
			Serial.println(Name);
			photoFiles[counter - 1] = Name;
			Serial.println(Name);
			photoCount++;
			counter++;
		}
		file = root.openNextFile();
	}
}
