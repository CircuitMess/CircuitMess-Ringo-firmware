#include <Arduino.h>
#include "MAKERphone.h"
#include <FastLED/FastLED.h>
#include <utility/Buttons/Buttons.h>
#include <utility/soundLib/MPWavLib.h>

#define EYE_COLOR    0x0000       /* TFT_BLACK */

MAKERphone mp;

boolean vFlag;
boolean hFlag;
boolean eaten;
boolean borderFlag = 1;
boolean pressed;
boolean snakeMenuInt;
boolean firstTime;
boolean savePresent = 0;
boolean exitFlag;
boolean foodCoolFlag = 0;

Oscillator *osc;

uint8_t speed = 1;
const int maxSnakeLength = 600;
uint16_t snakeLength = 12;
uint8_t hScore;
uint16_t tempScore = 0;
uint16_t tempScore2 = 0;

int snakeX[maxSnakeLength];
int snakeY[maxSnakeLength];

const char *highscoresPath = "/Snake/hiscores.sav";

uint32_t millisOne;
uint32_t millisTwo;

uint8_t foodX;
uint8_t foodY;
uint8_t menuSignal;
boolean snakeColor;

String name = "";

int8_t dirX = 1;
int8_t dirY = 0;

uint8_t tileSize = 5;
uint8_t foodSize = 4;

void mm();
void control();
void enterScore();
void highScores();

void defaultInt(){
  for(int i = 0; i < maxSnakeLength; i++){
    snakeX[i] = -6;
    snakeY[i] = -6;
  }
}
void drawFood(){

  while(!foodCoolFlag){
  foodX = random(3, 154);
  foodY = random(3, 122);
  for(uint8_t chX = 0; chX < 4; chX++){
    for(uint8_t chY = 0; chY < 4; chY++){
      if(mp.display.readPixelRGB(chX+foodX, chY+foodY) == 0){
      foodCoolFlag = 1;
      }
      else {
        foodCoolFlag = 0;
        break;
       }
      }
      if(foodCoolFlag == 0) break;
    }
  }
  foodCoolFlag = 0;
  eaten = false;
}
void drawSnake()
{
  for (int i = (snakeLength) - 1; i >= 0; i--)
  {
 	if ( i > (snakeLength) - 5) snakeColor = 0;	  
	else if(i < 4 || (i <= (snakeLength) - 6 &&  i % 2 == 1)) snakeColor = 1;
	else snakeColor = 0; 
	/* if (!foodX == snakeX[i] && !foodY == snakeY[i]) drawFood(); */
    //else snakeColor = 0;
	if(snakeColor == 0) mp.display.fillRect(snakeX[i], snakeY[i], tileSize, tileSize,TFT_DARKGREEN);
    else mp.display.fillRect(snakeX[i], snakeY[i], tileSize, tileSize,TFT_GREEN);
  }
  if(dirX == 1*speed)  {
	mp.display.drawRect(snakeX[0]+3, snakeY[0]+1, 2, 3,TFT_RED);
    mp.display.drawPixel(snakeX[0], snakeY[0]+1, EYE_COLOR);
	mp.display.drawPixel(snakeX[0]+1, snakeY[0]+1,  EYE_COLOR);
    mp.display.drawPixel(snakeX[0], snakeY[0]+3,  EYE_COLOR);
	mp.display.drawPixel(snakeX[0]+1, snakeY[0]+3,  EYE_COLOR);
  }
  else if(dirX == -1*speed)  {
    mp.display.fillRect(snakeX[0], snakeY[0]+1, 2, 3,TFT_RED);
    mp.display.drawPixel(snakeX[0]+3, snakeY[0]+1,  EYE_COLOR);
    mp.display.drawPixel(snakeX[0]+3, snakeY[0]+3,  EYE_COLOR);
    mp.display.drawPixel(snakeX[0]+4, snakeY[0]+1,  EYE_COLOR);
    mp.display.drawPixel(snakeX[0]+4, snakeY[0]+3,  EYE_COLOR);
  }
  else if(dirY == 1*speed) {
     mp.display.fillRect(snakeX[0]+1, snakeY[0]+3, 3, 2,TFT_RED);
     mp.display.drawPixel(snakeX[0]+1, snakeY[0],  EYE_COLOR);
     mp.display.drawPixel(snakeX[0]+3, snakeY[0],  EYE_COLOR);
     mp.display.drawPixel(snakeX[0]+1, snakeY[0]+1,  EYE_COLOR);
     mp.display.drawPixel(snakeX[0]+3, snakeY[0]+1,  EYE_COLOR);
  }
  else if(dirY == -1*speed) {
    mp.display.fillRect(snakeX[0]+1, snakeY[0], 3, 2,TFT_RED);
    mp.display.drawPixel(snakeX[0]+1, snakeY[0]+3,  EYE_COLOR);
    mp.display.drawPixel(snakeX[0]+3, snakeY[0]+3,  EYE_COLOR);
    mp.display.drawPixel(snakeX[0]+1, snakeY[0]+4,  EYE_COLOR);
    mp.display.drawPixel(snakeX[0]+3, snakeY[0]+4,  EYE_COLOR);
  }
}

void snakeMenu(){
  if(snakeMenuInt == 0) {
      snakeX[0] = 20;
      snakeY[0] = 1;
      snakeLength = 30;
      snakeMenuInt = 1;
  }
  if(snakeX[0] < 154 && snakeY[0]<=2){
    dirX = 1*speed;
    dirY = 0;
  }  
  else if(snakeX[0] >= 154 && snakeY[0] < 121){
    dirX = 0;
    dirY = 1*speed;
  } 
  else if(snakeX[0] > 3 && snakeY[0] >= 122){
    dirX = -1*speed;
    dirY = 0;
  } 
  else if(snakeX[0] <= 2 && snakeY[0]>3){
    dirX = 0;
    dirY = -1*speed;
  } 
}

void foodCheck(){


  if((snakeX[0] == foodX || snakeX[0] == foodX+3) && (snakeY[0] == foodY || snakeY[0] == foodY+3 )) eaten = true;
  else if ((snakeX[0]+3 == foodX || snakeX[0]+3 == foodX+1) && (snakeY[0]+3 == foodY || snakeY[0]+3 == foodY+3 )) eaten = true;
  else if ((snakeX[0]+3 == foodX || snakeX[0]+3 == foodX+1) && (snakeY[0] == foodY || snakeY[0] == foodY+3 )) eaten = true;
  else if ((snakeX[0] == foodX || snakeX[0] == foodX+3) && (snakeY[0]+3 == foodY || snakeY[0]+3 == foodY+3 )) eaten = true; 
  else if ((snakeX[0]+1 == foodX || snakeX[0]+1 == foodX+3) && (snakeY[0] == foodY || snakeY[0] == foodY+3 )) eaten = true;
  else if ((snakeX[0]+2 == foodX || snakeX[0]+2 == foodX+3) && (snakeY[0] == foodY || snakeY[0] == foodY+3 )) eaten = true; 
  else if ((snakeX[0] == foodX || snakeX[0] == foodX+3) && (snakeY[0]+1 == foodY || snakeY[0]+1 == foodY+3 )) eaten = true;
  else if ((snakeX[0] == foodX || snakeX[0] == foodX+3) && (snakeY[0]+2 == foodY || snakeY[0]+2 == foodY+3 )) eaten = true;
  else if ((snakeX[0]+4 == foodX || snakeX[0]+4 == foodX+3) && (snakeY[0] == foodY || snakeY[0] == foodY+3 )) eaten = true;
  else if ((snakeX[0]+4 == foodX || snakeX[0]+4 == foodX+3) && (snakeY[0]+4 == foodY || snakeY[0]+4 == foodY+3 )) eaten = true;

  else if ((snakeX[0] == foodX || snakeX[0] == foodX+3) && (snakeY[0] == foodY || snakeY[0] == foodY+3 )) eaten = true;
  else if ((snakeX[0]+3 == foodX || snakeX[0]+3 == foodX+3) && (snakeY[0]+3 == foodY || snakeY[0]+3 == foodY+3 )) eaten = true;
  else if ((snakeX[0]+3 == foodX || snakeX[0]+3 == foodX+3) && (snakeY[0] == foodY || snakeY[0] == foodY+3 )) eaten = true;
  else if ((snakeX[0] == foodX || snakeX[0] == foodX+3) && (snakeY[0]+3 == foodY || snakeY[0]+3 == foodY+3 )) eaten = true; 
  else if ((snakeX[0]+1 == foodX || snakeX[0]+1 == foodX+3) && (snakeY[0] == foodY || snakeY[0] == foodY+3 )) eaten = true;
  else if ((snakeX[0]+2 == foodX || snakeX[0]+2 == foodX+3) && (snakeY[0] == foodY || snakeY[0] == foodY+3 )) eaten = true; 
  else if ((snakeX[0] == foodX || snakeX[0] == foodX+3) && (snakeY[0]+1 == foodY || snakeY[0]+1 == foodY+3 )) eaten = true;
  else if ((snakeX[0] == foodX || snakeX[0] == foodX+3) && (snakeY[0]+2 == foodY || snakeY[0]+2 == foodY+3 )) eaten = true;  
  if(eaten){
    snakeLength+=6/speed;
    hScore+=(1*speed);
	osc->setWaveform(SQUARE);
	osc->note(90,0.03);
	FastLED.showColor(CRGB::Yellow);
 // drawFood();
  }
}
void control()
{
  for(int i = snakeLength; i  > 0; i--)
  {
    snakeX[i] = snakeX[i-1];
    snakeY[i] = snakeY[i-1];
  }
  snakeX[0] += dirX;
  snakeY[0] += dirY;
}
void reset(){
    if(firstTime) {
      mm();
      while(menuSignal == 2)mm();
    }
    else firstTime = 1;
    defaultInt();
    snakeX[0] = 78;
    snakeY[0] = 63;
    mp.display.fillScreen(TFT_BLACK);
    dirX = 1*speed;
    dirY = 0;
    hFlag = true;
    vFlag = false;
    snakeLength = 12/speed;
    hScore = 0;
  //  mp.display.drawRect(2,1,156,126,TFT_WHITE);
    while(!mp.update());
    drawFood();
    delay(1000);
}
void endScreen(){
		    FastLED.showColor(CRGB::DarkRed);  
		    osc->setWaveform(SAW);
		    osc->note(40,0.4);
        delay(1000);
		    mp.display.fillScreen(TFT_BLACK);
        mp.display.setTextSize(2);
        mp.display.setCursor(25, 5, 1);
        mp.display.setTextColor(TFT_RED);
        mp.display.print(F("GAME OVER"));
        mp.display.setCursor(5, 55, 2);
        mp.display.setTextColor(TFT_YELLOW);
        mp.display.setTextSize(1);
        mp.display.printCenter(F("Your score:"));
        mp.display.println();
        mp.display.setTextSize(2);
        mp.display.printCenter(hScore);
        while(!mp.update());
        delay(2000);
        enterScore();
        highScores();
        reset();
      }
void touchedItSelf()
{
  for (int i = 1; i < snakeLength; i++)
  {
    // if ((snakeX[0] == snakeX[i]) && (snakeY[0] == snakeY[i]))
    if (
    ((snakeX[0] == snakeX[i+10] || snakeX[0] == snakeX[i+10]+4) && (snakeY[0] == snakeY[i+10] || snakeY[0] == snakeY[i+10]+4))
    || ((snakeX[0]+4 == snakeX[i+10] || snakeX[0]+4 == snakeX[i+10]+4) && (snakeY[0]+4 == snakeY[i+10] || snakeY[0]+4 == snakeY[i+10]+4)))
    endScreen();
  }
}
void crash(){
    if(borderFlag) {
      if ((snakeX[0] <= 0 || snakeY[0] <= 0 || snakeX[0] >= 155 || snakeY[0] >= 124)) endScreen();
    }
    else {
      for(int i = 0; i < snakeLength; i++){
        if(snakeX[i] < 0 && snakeX[i] > -5) snakeX[i] = 159;
        else if (snakeX[i] < -5);
        else snakeX[i] = snakeX[i]%159;
        if (snakeY[i] < 0 && snakeY[i] > -5 ) snakeY[i] = 127;
        else if (snakeY[i] < -5);
        else snakeY[i] = snakeY[i]%127;
    }
  }
}
void highScores(){
  File file = SD.open(highscoresPath);
	JsonArray &hiscores = mp.jb.parseArray(file);
	file.close();
	while(1)
	{
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(20, 1);
		mp.display.setTextSize(2);
		mp.display.setTextFont(1);
		mp.display.setTextColor(TFT_YELLOW);
		mp.display.printCenter(F("HIGHSCORES"));
    mp.display.setTextColor(TFT_GREEN);
		mp.display.setCursor(3, 110);
		for (int i = 1; i < 6;i++)
		{
			mp.display.setCursor(18, i * 19);
			if(i <= hiscores.size())
			{
				for(JsonObject& element:hiscores)
				{
					if(element["Rank"] == i)
						mp.display.printf("%d. %.3s %3d", i, element["Name"].as<char*>(), element["Score"].as<uint16_t>());
				}
			}
			else
				mp.display.printf("%d. --- ---", i);
		}
    if(mp.buttons.released(BTN_B)) break;
    mp.display.setTextSize(1);
    mp.display.setTextFont(2);
    mp.display.setCursor(110, 110);
		mp.display.print("B: Back");
		mp.update();
	}
//	mp.homePopupEnable(1);
}
void buttonDir(){
//	mp.buttons.update();
      if ((mp.buttons.pressed(BTN_4) || mp.buttons.pressed(BTN_LEFT)) && !hFlag){
      dirX = -1*speed;
      dirY = 0;
      hFlag = true;
      vFlag = false;  
      pressed = true;
    }
    else if ((mp.buttons.pressed(BTN_8) || mp.buttons.pressed(BTN_DOWN))  && !vFlag){
      dirX = 0;
      dirY = 1*speed;
      vFlag = true;
      hFlag = false; 
      pressed  = true;
    }
    else if ((mp.buttons.pressed(BTN_2) || mp.buttons.pressed(BTN_UP)) && !vFlag){
      dirX = 0;
      dirY = -1*speed;
      vFlag = true; 
      hFlag = false; 
      pressed = true;
    }
    else if ((mp.buttons.pressed(BTN_6) || mp.buttons.pressed(BTN_RIGHT)) && !hFlag){
      dirX = 1*speed;
      dirY = 0;
      hFlag = true;
      vFlag = false; 
      pressed = true; 
    }
    if(pressed) millisOne = millis();
    pressed = false;
   // mp.buttons.update();
}
void startAnimation(){
  uint8_t stSize = 1;
  for(uint8_t i = 60; i >= 20; i-=12 ){
  mp.display.fillScreen(TFT_BLACK);
  mp.display.setCursor(i, 45, 1);
  mp.display.setTextColor(TFT_WHITE);
  mp.display.setTextSize(stSize);
  stSize++;
  mp.display.println(F("SNAKE"));
  while(!mp.update());
  FastLED.showColor(CRGB::Green);
  delay(100);
  }
  mp.display.fillScreen(TFT_BLACK);
  mp.display.setCursor(6, 45, 1);
  mp.display.setTextColor(TFT_GREEN);
  mp.display.setTextSize(5);
  mp.display.println(F("SNAKE"));
  while(!mp.update());
  FastLED.showColor(CRGB::Green);
  delay(400);
  defaultInt();
  snakeLength = 50;
  snakeX[0] = -5;
  snakeY[0] = 90;
  dirX = 1;
  dirY = 0;
  for(int i = 0; i < 210; i++){
    mp.display.fillScreen(TFT_BLACK);
    mp.display.setCursor(6, 45, 1);
    mp.display.setTextColor(TFT_GREEN);
    mp.display.setTextSize(5);
    mp.display.println(F("SNAKE"));
    control();
    drawSnake();
    mp.update();
  }
}
void mm(){

  defaultInt();
  osc->setWaveform(SINE);
  while(!mp.buttons.pressed(BTN_A)){
    mp.display.fillScreen(TFT_BLACK);
	snakeMenu();
    control();
    drawSnake();
    mp.display.setCursor(50, 9, 1);
    mp.display.setTextColor(TFT_GREEN);
    mp.display.setTextSize(2);
    mp.display.println(F("SNAKE"));
 
    if(mp.buttons.pressed(BTN_DOWN)){
		//osc->setVolume(80);
  		osc->note(60,0.05);
    	menuSignal++;
    	menuSignal%=4;
    }
    if(mp.buttons.pressed(BTN_UP)){
		osc->note(60,0.05);
	   	if(menuSignal !=0)menuSignal--;
    	else menuSignal = 3;
    }
    if(mp.buttons.pressed(BTN_RIGHT) && menuSignal == 1 && speed == 1){
		osc->note(50,0.05);
		FastLED.showColor(CRGB::DarkGreen);
    	speed = 2;
    }
    if(mp.buttons.pressed(BTN_LEFT) && menuSignal == 1 && speed == 2){
		osc->note(50,0.05);
		FastLED.showColor(CRGB::LimeGreen);
        speed = 1;
    }
	if(mp.buttons.pressed(BTN_RIGHT) && menuSignal == 0 && borderFlag == 1){
		osc->note(50,0.05);
		FastLED.showColor(CRGB::DarkGreen);
    	borderFlag = 0;
    }
    if(mp.buttons.pressed(BTN_LEFT) && menuSignal == 0 && borderFlag == 0){
		osc->note(50,0.05);
		FastLED.showColor(CRGB::LimeGreen);
        borderFlag = 1;
    }
    mp.display.setTextSize(2);
    //mp.display.fillScreen(TFT_BLACK);
    mp.display.setTextColor(TFT_DARKGREY);
    if(borderFlag == 1 && menuSignal == 0)  mp.display.setTextColor(TFT_YELLOW);
    mp.display.setCursor(9, 22, 2);
    mp.display.print(F("WALL"));
    mp.display.setTextColor(TFT_DARKGREY);
	mp.display.print(F("/"));
    if(menuSignal == 0 && borderFlag == 0)mp.display.setTextColor(TFT_YELLOW);
    mp.display.setCursor(88, 22, 2);
    mp.display.print(F("FREE"));
    mp.display.setTextColor(TFT_DARKGREY);
    if(menuSignal == 1)mp.display.setTextColor(TFT_YELLOW);
    mp.display.setCursor(25, 46, 2);
	mp.display.print("SPEED: ");
    mp.display.print(speed);
    mp.display.setTextColor(TFT_DARKGREY);
    if(menuSignal == 2)mp.display.setTextColor(TFT_YELLOW);
    mp.display.setCursor(29, 70, 2);
    mp.display.print(F("SCORES"));
	mp.display.setTextColor(TFT_DARKGREY);
	if(menuSignal == 3)mp.display.setTextColor(TFT_YELLOW);
    mp.display.setCursor(54, 94, 2);
    mp.display.print(F("EXIT"));
	delayMicroseconds(100);
    mp.update();
  }
  snakeMenuInt = 0;
  if(menuSignal == 2)highScores();
  if(menuSignal == 3) mp.loader();
  //else;
  delay(1000);
}
void enterInitials() {
  name = "";
  String previous = "";
  uint32_t elapsedMillis = millis();
  uint32_t hiscoreMillis = millis();
  bool blinkState = 1;
  bool hiscoreBlink = 0;
  mp.textInput("");
  mp.textPointer = 0;
  while (!mp.buttons.released(BTN_A) || name.length() != 3) {
    if(mp.buttons.released(BTN_HOME));
    //if(mp.update())
    mp.update();
    name = mp.textInput(name, 3);
    if(mp.buttons.released(BTN_B)){
      exitFlag = true;
      break;
    } 
    if (millis() - elapsedMillis >= multi_tap_threshold) //cursor blinking routine
		{
			elapsedMillis = millis();
			blinkState = !blinkState;
		}
    if(millis()-hiscoreMillis >= 500)
    {
      hiscoreMillis = millis();
      hiscoreBlink = !hiscoreBlink;
    }
    previous = name;
   
    if(name.indexOf(' ') != -1)
      name = name.substring(0, name.length() - 1);
    if (previous != name)
    {
      blinkState = 1;
      elapsedMillis = millis();
    }
    
    mp.display.fillScreen(TFT_BLACK);
    mp.display.setCursor(16, 8);
    mp.display.setTextFont(2);
    mp.display.setTextColor(TFT_GREEN);
    mp.display.setTextSize(1);
    mp.display.printCenter(F("ENTER NAME"));
    mp.display.setCursor(48, 80);
    
    if(hiscoreBlink && hScore > tempScore)
      mp.display.printCenter(F("NEW HIGH!"));
    else
    mp.display.printf("SCORE: %3d", hScore);
    mp.display.setCursor(2, 110);
		mp.display.print("Erase");
    mp.display.setCursor(110, 110);
		mp.display.print("B: Back");
    mp.display.setCursor(50, 40);
    mp.display.printCenter(name);
    if(blinkState)
      mp.display.drawFastVLine(mp.display.getCursorX(), mp.display.getCursorY()+3, 10, TFT_GREEN);
    mp.display.drawRect(30, 38, 100, 20, TFT_GREEN);
    //mp.update();
  }
  mp.update();
}

void enterScore(){
      mp.jb.clear();
      File file;
			file = SD.open(highscoresPath);
      JsonArray &hiscores = mp.jb.parseArray(file);
      /* if(!hiscores.success()){
        file = SD.open(highscoresPath);
       JsonArray &hiscores = mp.jb.parseArray(file);
      } */
			file.close();
			for (JsonObject& element : hiscores)
			{
				if(element["Rank"] == 1)
					tempScore = element["Score"].as<int>();
        if(element["Rank"] == 5)
          tempScore2 = element["Score"].as<int>();
      }
			Serial.println("HERE");
			delay(5);
			

			if(hScore != 0 && hScore >= tempScore2) enterInitials();
      else {
        return;
      }
      if(exitFlag){
        exitFlag = 0;
        return;
      }
			JsonObject &newHiscore = mp.jb.createObject();
			newHiscore["Name"] = name;
			newHiscore["Score"] = hScore;
			newHiscore["Rank"] = 1;

			if(savePresent && hiscores.size() > 0)
			{
				newHiscore["Rank"] = 999;
				Serial.println(hiscores.size());
				uint16_t tempSize = hiscores.size();
				for (int16_t i = 0; i < tempSize;i++)//searching for a place in the leaderboard for our new score
				{
					Serial.printf("i: %d\n", i);
					Serial.println((uint16_t)(hiscores[i]["Rank"]));
					Serial.println((uint16_t)(hiscores[i]["Score"]));
					delay(5);
					if(hScore >= (uint16_t)(hiscores[i]["Score"]))
					{
					Serial.println("ENTERED");
					delay(5);
					if((uint16_t)(newHiscore["Rank"]) >  (uint16_t)(hiscores[i]["Rank"]))
					{
						newHiscore["Rank"] = (uint16_t)(hiscores[i]["Rank"]);
					}
					JsonObject &tempObject = mp.jb.createObject();
					tempObject["Name"] = (const char *)(hiscores[i]["Name"]);
					tempObject["Score"] = (uint16_t)(hiscores[i]["Score"]);
					tempObject["Rank"] = (uint16_t)(hiscores[i]["Rank"]) + 1;
					tempObject.prettyPrintTo(Serial);
					delay(5);
					hiscores.remove(i);
					hiscores.add(tempObject);
					tempSize--;
					i--;
					}
					else
					{
					if(newHiscore["Rank"] <= (uint16_t)(hiscores[i]["Rank"]) || newHiscore["Rank"] == 999)
						newHiscore["Rank"] = (uint16_t)(hiscores[i]["Rank"]) + 1;
					}
				}
			}
      hiscores.add(newHiscore);
			file = SD.open(highscoresPath, "w");
			hiscores.prettyPrintTo(file);
			file.close();
			while(!mp.update());	


			
}
void paused(){
  while(1){
    mp.display.fillScreen(TFT_BLACK);
    drawSnake();
    mp.display.fillRect(foodX, foodY, foodSize, foodSize, TFT_YELLOW);
    if(borderFlag) mp.display.drawRect(0,0,160,128,TFT_WHITE);
    mp.display.setCursor(6, 110, 2);
    mp.display.setTextSize(1);
    mp.display.setTextColor(TFT_DARKGREY);
    mp.display.print("SCORE:");
    mp.display.print(hScore);
    mp.display.setTextColor(TFT_LIGHTGREY);
    mp.display.setCursor(35,35,2);
    mp.display.setTextSize(2);
    mp.display.print(F("PAUSED"));
    mp.display.setCursor(34,65,2);
    mp.display.setTextSize(1);
    mp.display.print(F("Press A to play")); 
    mp.display.setCursor(35,80,2);
    mp.display.print(F("Press B to exit")); 
    while(!mp.update());
    if(mp.buttons.released(BTN_A)){
      delay(500);
      break;
    }
    if(mp.buttons.released(BTN_B)){
      delay(800);
      reset();
      break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  mp.begin();
  osc = new Oscillator(SINE);
  addOscillator(osc);
  osc->setVolume(60);
  File file = SD.open(highscoresPath);
	JsonArray &hiscores = mp.jb.parseArray(file);
	file.close();
	if(!SD.exists("/Snake"))
		SD.mkdir("/Snake");
	if(hiscores.size() > 0)
		savePresent = 1;
	else
	{
		JsonArray &hiscores = mp.jb.createArray();
		JsonObject &test = mp.jb.createObject();
    test["Name"] = "ABC";
		test["Score"] = 0;
		test["Rank"] = 1;
		hiscores.add(test);
		File file = SD.open(highscoresPath, "w");
		hiscores.prettyPrintTo(file);
		file.close();
		mp.readFile(highscoresPath);
    savePresent = 1;
	}
  startAnimation();
  mm();
  while(menuSignal == 2)mm();
  reset();
}
void loop() {
  //while(!mp.update());

  mp.update();
  mp.sleepTimer = millis();
  //----------------------------------
  millisTwo = millis();
  if(millisTwo - millisOne > 90) buttonDir();
  // ---------------------------------
  mp.display.fillScreen(TFT_BLACK);
  if(borderFlag) mp.display.drawRect(0,0,160,128,TFT_WHITE);
  mp.display.setCursor(6, 110, 2);
  mp.display.setTextSize(1);
  mp.display.setTextColor(TFT_DARKGREY);
  mp.display.print("SCORE:");
  mp.display.print(hScore);
  // ---------------------------------
  control();
  crash();
  touchedItSelf();
  if(eaten) mp.display.fillRect(foodX,foodY, foodSize, foodSize, TFT_BLACK);
  drawSnake();
  if(eaten)  drawFood();
  mp.display.fillRect(foodX, foodY, foodSize, foodSize, TFT_YELLOW);
  foodCheck();
  //----------------------------------
  if(mp.buttons.released(BTN_B)) paused();

}