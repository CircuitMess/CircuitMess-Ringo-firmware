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
//Media app

void mediaApp() {
	mp.dataRefreshFlag = 0;
	while(1)
	{
		int8_t input = mediaMenu(mediaItems, 2);

		if(input == 0) //music
		{
			listAudio("/Music", 1);
			if(audioCount > 0)
			{
				int16_t index =0;
				while (1)
				{
					index = audioPlayerMenu("Select file to play:", audioFiles, audioCount, index);
					if (index == -1)
						break;
					mp.display.fillScreen(TFT_LIGHTGREY);
					audioPlayer(index);
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
					Serial.println(index);
					while(!mp.update());
					while(1)
					{
						
						if(mp.buttons.released(BTN_A) || mp.buttons.released(BTN_B))
							break;
						if(mp.buttons.released(BTN_LEFT))
						{
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

	uint8_t boxHeight;
	boxHeight = 54; //actually 2 less than that
	while (1) {
		while (!mp.update());
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(0, 0);
		cameraY_actual = (cameraY_actual + cameraY) / 2;
		if (cameraY_actual - cameraY == 1) {
			cameraY_actual = cameraY;
		}

		for (uint8_t i = 0; i < length; i++) {
			mediaMenuDrawBox(title[i], i, cameraY_actual);
		}
		mediaMenuDrawCursor(cursor, cameraY_actual, pressed);

		if (!mp.buttons.pressed(BTN_DOWN) && !mp.buttons.pressed(BTN_UP))
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
	if (millis() % 500 <= 250 && pressed == 0) {
		return;
	}
	y += i * boxHeight + offset;
	mp.display.drawRect(0, y-1, mp.display.width()-1, boxHeight+2, TFT_RED);
	mp.display.drawRect(1, y, mp.display.width()-3, boxHeight, TFT_RED);
}



void audioPlayer(uint16_t index) {
	uint8_t scale= 2;
	bool out = 0;
	char c = NO_KEY;
	bool playState = 1;
	bool loop = 0;
	bool shuffleReset = 0;
	bool allTrue = 0;
	bool shuffle = 0;
	bool shuffleList[audioCount];
	MPTrack* trackArray[audioCount];
	MPTrack* mp3;
	for(int i = 0; i < audioCount;i++)
	{
		String songName = audioFiles[i];
		char test[songName.length() + 1];
		songName.toCharArray(test, songName.length() + 1);
		Serial.println(test);
		delay(5);
		trackArray[i] = new MPTrack(test);
	}
	while(1)
	{
		uint8_t x = 1;
		uint8_t y = 53;
		int8_t i = 0;

		
		mp.display.fillScreen(backgroundColors[mp.backgroundIndex]);
		//draw bitmaps
		mp.display.drawBitmap(2*scale, 20*scale, previous, TFT_BLACK, scale);
		mp.display.drawBitmap(65*scale, 20*scale, next, TFT_BLACK, scale);
		mp.display.drawBitmap(2*scale, 75, repeatSprite, TFT_BLACK, scale);
		mp.display.drawBitmap(66*scale, 37*scale, shuffleIcon, TFT_BLACK, scale);
		if(playState)
			mp.display.drawBitmap(37*scale, 19*scale, play, TFT_BLACK, scale);
		else
			mp.display.drawBitmap(72, 40, pause2, TFT_BLACK, scale);
		mp.display.drawBitmap(17*scale, 2*scale, cover2, TFT_BLACK, scale);
		//prepare for text printing
		mp.display.setTextColor(TFT_BLACK);


		mp.display.setTextSize(1);
		mp.display.setTextWrap(0);
		//drawtext
		mp.display.setCursor(4, 2);
		mp.display.print(mp.volume);
		mp.display.setCursor(1, 111);
		if(audioFiles[index].length() > 20)
			mp.display.print(audioFiles[index]);
		else
			mp.display.printCenter(audioFiles[index]);
		mp.display.fillRect(141,74, 4,4, shuffle ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
		mp.display.fillRect(14,73, 4,4, loop ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);

		while(!mp.update());
		mp3 = trackArray[index];
		if(addTrack(mp3))
			Serial.println("OK");
		else
			Serial.println("ERROR");
		delay(5);
		if(playState)
			mp3->play();
		mp3->setVolume(256*14/mp.volume);
		while (1) 
		{
			if (mp.buttons.released(BTN_B))
			{

				mp3->stop();
				removeTrack(mp3);
				Serial.println(F("Stopped"));
				delay(5);
				while (!mp.update());
				out = 1;
				break;
			}

			if (mp.buttons.released(BTN_A)) //PLAY/PAUSE BUTTON
			{
				
				if(playState)
				{
					mp3->pause();
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
					mp.display.setCursor(4, 2);
					mp.display.print(mp.volume);
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
					mp.display.setCursor(4, 2);
					mp.display.print(mp.volume);
					mp.display.fillRect(141,74, 4,4, shuffle ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
					mp.display.fillRect(14,73, 4,4, loop ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
					while(!mp.update());
					mp3->resume();
				}
				playState = !playState;
				while (!mp.update());
			}

			if (mp.buttons.released(BTN_DOWN) && mp.volume > 1) //DOWN
			{
				
				mp.volume--;
				mp3->setVolume(256*14/mp.volume);
				//prepare for text printing
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextFont(2);
				mp.display.setTextSize(1);
				mp.display.setTextWrap(0);
				mp.display.fillRect(4,2, 15, 15, backgroundColors[mp.backgroundIndex]);
				//drawtext
				mp.display.setCursor(4, 2);
				mp.display.print(mp.volume);

				//prepare for text printing
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextFont(2);
				mp.display.setTextSize(1);
				mp.display.setTextWrap(0);
				mp.display.fillRect(4,2, 15, 15, backgroundColors[mp.backgroundIndex]);
				//drawtext
				mp.display.setCursor(4, 2);
				mp.display.print(mp.volume);
				while (!mp.update());
			}

			if (mp.buttons.released(BTN_UP) && mp.volume < 14) //UP
			{
				
				mp.volume++;
				mp3->setVolume(256*14/mp.volume);
				//prepare for text printing
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextFont(2);
				mp.display.setTextSize(1);
				mp.display.setTextWrap(0);
				mp.display.fillRect(4,2, 15, 15, backgroundColors[mp.backgroundIndex]);
				//drawtext
				mp.display.setCursor(4, 2);
				mp.display.print(mp.volume);
				//prepare for text printing
				mp.display.setTextColor(TFT_BLACK);
				mp.display.setTextFont(2);
				mp.display.setTextSize(1);
				mp.display.setTextWrap(0);
				mp.display.fillRect(4,2, 15, 15, backgroundColors[mp.backgroundIndex]);
				//drawtext
				mp.display.setCursor(4, 2);
				mp.display.print(mp.volume);
				while (!mp.update());
			}
			if(mp.buttons.released(BTN_LEFT)) //previous
			{
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
							playState = 0;
							shuffleReset = 1;
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
							playState = 0;
							index = audioCount-1;
						}
					}
				}
				if(shuffleReset)
					playState = 0;
				mp3->stop();
				removeTrack(mp3);
				while(!mp.update());
				break;
			}
			if(mp.buttons.released(BTN_RIGHT)) //next
			{
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
							playState = 0;
							shuffleReset = 1;
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
							shuffleReset = 1;
							index = 0;
							playState = 0;
						}
					}
					// index = (index < audioCount - 1) ? index++ : 0;
				}
				if(shuffleReset)
					playState = 0;
				mp3->stop();
				removeTrack(mp3);
				while(!mp.update());
				break;
			}
			if(mp.buttons.released(BTN_FUN_RIGHT)) //shuffle button
			{
				while(!mp.update());
				if(!shuffle)
					memset(shuffleList, 0, sizeof(shuffleList));
				shuffle = !shuffle;
				mp.display.fillRect(141,74, 4,4, shuffle ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
				mp.display.fillRect(141,74, 4,4, shuffle ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
				shuffleList[index] = 1;

			}
			if(mp.buttons.released(BTN_FUN_LEFT)) //loop button
			{
				while(!mp.update());
				loop = !loop;
				mp.display.fillRect(14,73, 4,4, loop ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
				mp.display.fillRect(14,73, 4,4, loop ? TFT_BLACK : backgroundColors[mp.backgroundIndex]);
			}
			mp.update();
			if(mp3->isPlaying() < 1 && playState) //if the current song is finished, play the next one
			{
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
							playState = 0;
							shuffleReset = 1;
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
							playState = 0;
						}
					}
				}
				break;
			}
		}
		if(out)
			break;
	}
	for(int i = 0; i < audioCount;i++)
	{
		removeTrack(trackArray[i]);
		delete trackArray[i];
	}
}
void listPhotos(const char * dirname, uint8_t levels) {
	photoCount = 0;
	
	Serial.printf("Listing directory: %s\n", dirname);
	SDAudioFile root = mp.SD.open(dirname);
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
	SDAudioFile file = root.openNextFile();
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
