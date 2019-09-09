#include <Arduino.h>
#include "MAKERphone.h"
#include <FastLED/FastLED.h>
#include <utility/Buttons/Buttons.h>

#define EYE_COLOR    0x0000       /* TFT_GREEN */

MAKERphone mp;

boolean vFlag;
boolean hFlag;
boolean eaten;
boolean borderFlag;
boolean pressed;
boolean snakeMenuInt;
boolean firstTime;

uint8_t speed = 2;
const int maxSnakeLength = 600;
uint8_t snakeLength = 8;
uint8_t hScore;

int snakeX[maxSnakeLength];
int snakeY[maxSnakeLength];

uint32_t millisOne;
uint32_t millisTwo;

uint8_t foodX;
uint8_t foodY;
uint8_t menuSignal;
boolean snakeColor;

int8_t dirX = 1;
int8_t dirY = 0;

uint8_t tileSize = 5;
uint8_t foodSize = 4;

void mm();
void control();

void defaultInt(){
  for(int i = 0; i < maxSnakeLength; i++){
    snakeX[i] = -6;
    snakeY[i] = -6;
  }
}
void drawFood(){

  foodX = random(3, 154);
  foodY = random(3, 122);
  eaten = false;
}
void drawSnake()
{
  for (int i = (snakeLength/speed) - 1; i >= 0; i--)
  {
/* 	if ( i > snakeLength - 5) snakeColor = 0;	  
	else if(i < 4 || (i <= snakeLength-6 &&  i % 2 == 1)) snakeColor = 1;
	else snakeColor = 0; */
	if (foodX == snakeX[i] && foodY == snakeY[i]) drawFood();
    //else snakeColor = 0;
	//if(snakeColor == 0) mp.display.fillRect(snakeX[i], snakeY[i], tileSize, tileSize,TFT_DARKGREEN);
    mp.display.fillRect(snakeX[i], snakeY[i], tileSize, tileSize,TFT_GREEN);
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
    mp.display.fillRect(foodX,foodY, foodSize, foodSize, TFT_BLACK);
    snakeLength+=6;
    hScore+=(1*(speed-1));
    drawFood();
  }
}
void control()
{
  for(int i = snakeLength/speed; i  > 0; i--)
  {
    snakeX[i] = snakeX[i-1];
    snakeY[i] = snakeY[i-1];
  }
  snakeX[0] += dirX;
  snakeY[0] += dirY;
}
void reset(){
    if(firstTime) mm();
    else firstTime = 1;
    defaultInt();
    snakeX[0] = 78;
    snakeY[0] = 63;
    mp.display.fillScreen(TFT_BLACK);
    dirX = 1*speed;
    dirY = 0;
    hFlag = true;
    vFlag = false;
    snakeLength = 8;
    hScore = 0;
  //  mp.display.drawRect(2,1,156,126,TFT_WHITE);
    while(!mp.update());
    drawFood();
    delay(1000);
}
void touchedItSelf()
{
  for (int i = 1; i < snakeLength/speed; i++)
  {
    // if ((snakeX[0] == snakeX[i]) && (snakeY[0] == snakeY[i]))
    if (
    ((snakeX[0] == snakeX[i+10] || snakeX[0] == snakeX[i+10]+4) && (snakeY[0] == snakeY[i+10] || snakeY[0] == snakeY[i+10]+4))
    || ((snakeX[0]+4 == snakeX[i+10] || snakeX[0]+4 == snakeX[i+10]+4) && (snakeY[0]+4 == snakeY[i+10] || snakeY[0]+4 == snakeY[i+10]+4)))
    {
      delay(1000);
      reset();
    }
  }
}
void crash(){
    if(borderFlag) {
      if ((snakeX[0] <= 0 || snakeY[0] <= 0 || snakeX[0] >= 155 || snakeY[0] >= 124)){
        delay(1000);
        mp.display.setCursor(20, 40, 2);
        mp.display.setTextSize(2);
        mp.display.setTextColor(TFT_YELLOW);
        mp.display.print(F("Your score"));
        mp.display.setCursor(45, 70);
        mp.display.print(hScore);
        while(!mp.update());
        delay(2000);
        reset();
      }
    }
    else {
      for(int i = 0; i < snakeLength/speed; i++){
      if(snakeX[i] < 0 && snakeX[i] > -5) snakeX[i] = 159;
      else if (snakeX[i] < -5);
      else snakeX[i] = snakeX[i]%159;
      if (snakeY[i] < 0 && snakeY[i] > -5 ) snakeY[i] = 127;
      else if (snakeY[i] < -5);
      else snakeY[i] = snakeY[i]%127;

      }
    }
}
void menuSnake(){
  snakeLength = 30;

  dirX = 1*speed;
  dirY = 0;
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
void mm(){
  mp.display.fillScreen(TFT_BLACK);
  mp.display.setCursor(30, 30, 1);
  mp.display.setTextColor(TFT_GREEN);
  mp.display.setTextSize(3);
  mp.display.println(F("SNAKE"));
  while(!mp.update());
  delay(1000);
  defaultInt();
  while(!mp.buttons.pressed(BTN_A)){
    mp.display.fillScreen(TFT_BLACK);
    mp.display.setCursor(50, 8, 1);
    mp.display.setTextColor(TFT_GREEN);
    mp.display.setTextSize(2);
    mp.display.println(F("SNAKE"));
    snakeMenu();
    control();
    drawSnake();
    if(mp.buttons.pressed(BTN_DOWN)){
      menuSignal++;
      menuSignal%=4;
    }
    if(mp.buttons.pressed(BTN_UP)){
      if(menuSignal !=0)menuSignal--;
      else menuSignal = 3;
    }
    if(mp.buttons.pressed(BTN_RIGHT) && menuSignal==3){
      speed = 3;
    }
    if(mp.buttons.pressed(BTN_LEFT) && menuSignal ==3){
      speed = 2;
    }
    mp.display.setTextSize(2);
    //mp.display.fillScreen(TFT_BLACK);
    mp.display.setTextColor(TFT_DARKGREY);
    if(menuSignal == 0)  mp.display.setTextColor(TFT_YELLOW);
    mp.display.setCursor(30, 18, 2);
    mp.display.print(F("BORDER"));
    mp.display.setTextColor(TFT_DARKGREY);
    if(menuSignal == 1)mp.display.setTextColor(TFT_YELLOW);
    mp.display.setCursor(12, 42, 2);
    mp.display.print(F("NO BORDER"));
    mp.display.setTextColor(TFT_DARKGREY);
    if(menuSignal == 2)mp.display.setTextColor(TFT_YELLOW);
    mp.display.setCursor(54, 68, 2);
    mp.display.print(F("EXIT"));
    mp.display.setTextColor(TFT_DARKGREY);
    if(menuSignal == 3)mp.display.setTextColor(TFT_YELLOW);
    mp.display.setCursor(25, 92, 2);
    mp.display.print("SPEED: ");
    mp.display.print(speed-1);

    while(!mp.update());
  }
  snakeMenuInt = 0;
  if(menuSignal == 0) borderFlag = 1;
  if(menuSignal == 1) borderFlag = 0;
  if(menuSignal == 2) mp.loader();


  delay(1000);

}
void setup() {
  Serial.begin(115200);
  mp.begin();
  mm();
  reset();
}
void loop() {
  while(!mp.update());
 // mp.update();
  //----------------------------------
  millisTwo = millis();
  if(millisTwo - millisOne > 100) buttonDir();
  // ---------------------------------
  mp.display.fillScreen(TFT_BLACK);
  if(borderFlag) mp.display.drawRect(0,0,160,128,TFT_WHITE);
  mp.display.setCursor(6, 110, 2);
  mp.display.setTextSize(1);
  mp.display.setTextColor(TFT_DARKGREY);
  mp.display.print("SCORE:");
  mp.display.print(hScore);
  // ---------------------------------
  foodCheck();
  control();
  crash();
  drawSnake();
  touchedItSelf();
	//----------------------------------
  mp.display.fillRect(foodX, foodY, foodSize, foodSize, TFT_YELLOW);

}