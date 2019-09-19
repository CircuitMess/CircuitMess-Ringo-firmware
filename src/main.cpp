#include <stdio.h>
#include <MAKERphone.h>
#include "Sprites.h"
MAKERphone mp;
// MPTrack *shoot;
MPTrack *collide;
// MPTrack *hit;
MPTrack *bgmusic;
MPTrack *titleMusic;
MPTrack *gameoverMusic;
MPTrack *destroyed;
Oscillator *shoot;
Oscillator *hit;
File file;
/*
    Space Rocks
    Copyright (C) 2019 CircuitMess

    original Arduboy game:
    ASTEROIDS
    Copyright (C) 2018 Xavier
    https://github.com/CDRXavier/ASTEROID/tree/master

    Colorized and ported to MAKERphone by CircuitMess

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
float shipX;
float shipY;
float velocityX;
float velocityY;
int16_t HDG;
//bool released;
int16_t bullet[8][5];
float asteroids[8][4];
uint16_t asteroidDraw[8];
float rocks[16][4];
uint16_t rockDraw[16];
float pebbles[32][4];
uint16_t pebbleDraw[32];
int bulletCount;
int asteroidCount;
int rockCount;
int pebbleCount;
int16_t life = 3;
uint16_t level = 0;
uint16_t score;
uint16_t tempScore = 0;
char tick;
bool turnC;
bool invincibility = 0;
#define INVINCIBILITY_PERIOD 2000
uint32_t invincMillis = 0;
uint32_t pixelsTimer = millis();
bool pixelsState = 0;
enum class ProgState : uint8_t {Main, Simulation,  DataEntry, DataDisplay, Pause, DataErasure};
String name;
static const PROGMEM float heading[24][2] = {
  { +0.00, -0.20}, { +0.05, -0.19},
  { +0.10, -0.17}, { +0.14, -0.14},
  { +0.17, -0.10}, { +0.19, -0.05},
  { +0.20, +0.00}, { +0.19, +0.05},
  { +0.17, +0.10}, { +0.14, +0.14},
  { +0.10, +0.17}, { +0.05, +0.19},
  { +0.00, +0.20}, { -0.05, +0.19},
  { -0.10, +0.17}, { -0.14, +0.14},
  { -0.17, +0.10}, { -0.19, +0.05},
  { -0.20, +0.00}, { -0.19, -0.05},
  { -0.17, -0.10}, { -0.14, -0.14},
  { -0.10, -0.17}, { -0.05, -0.19}
};

static const int8_t shipTbl[6][24] PROGMEM = {
  //HDG
  //  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23
  {  +3, +3, +2, +1, -1, -2, -3, -2, -1, +1, +2, +3, +3, -3, -2, -1, +1, +2, +3, +2, +1, -1, -2, -3,},
  {  +3, +3, +3, +3, +3, +3, +3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, +3, +3, +3, +3, +3, +3,},
  {  +0, +2, +3, +3, +3, +4, +4, +4, +3, +3, +3, +2, +0, -2, -3, -3, -3, -4, -4, -4, -3, -3, -3, -2,},
  {  -4, -4, -3, -3, -3, -2, +0, +2, +3, +3, +3, +4, +4, +4, +3, +3, +3, +2, +0, -2, -3, -3, -3, -4,},
  {  -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, +3, +3, +3, +3, +3, +3, +3, +3, +3, +3, +3,},
  {  +3, +2, +1, -1, -2, -3, -3, +3, +2, +1, -1, -2, -3, -2, -1, +1, +2, +3, -3, -3, -2, -1, +1, +2,},
};
const char *highscoresPath = "/SpaceRocks/hiscores.sav";
bool savePresent = 0;
#define shipX0  shipX + (int8_t)(pgm_read_byte(&shipTbl[0][HDG]))
#define shipY0  shipY + (int8_t)(pgm_read_byte(&shipTbl[1][HDG]))
#define shipX1  shipX + (int8_t)(pgm_read_byte(&shipTbl[2][HDG]))
#define shipY1  shipY + (int8_t)(pgm_read_byte(&shipTbl[3][HDG]))
#define shipX2  shipX + (int8_t)(pgm_read_byte(&shipTbl[4][HDG]))
#define shipY2  shipY + (int8_t)(pgm_read_byte(&shipTbl[5][HDG]))
#define ADC_TEMP (_BV(REFS0) | _BV(REFS1) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0))
#define ADC_VOLTAGE (_BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1))
ProgState simState = ProgState::Main;
void collision() {
  collide->rewind();
  collide->play();
  for (uint8_t i = 0; i < 8; i++)
  {
    mp.leds[i] = CRGB(255,0,0);
    pixelsTimer=millis();
    pixelsState=1;  
  }
	//   sound.tone(150, 50);
	life = life - 1;
  if(life>0)
  {
    invincibility = 1;
    invincMillis = millis();
    shipX = mp.display.width()/2;
	  shipY = mp.display.height()/2;
  }
	velocityX = 0;
	velocityY = 0;
}
void guidance() {

  //stopping gradually
  if (velocityY > 0)velocityY -= 0.01;
  if (velocityY < 0)velocityY += 0.01;

  if (velocityX > 0)velocityX -= 0.01;
  if (velocityX < 0)velocityX += 0.01;

  //stop completely if going slow enough
  if (velocityX <= 0.01 && velocityX >= -0.01) velocityX = 0;
  if (velocityY <= 0.01 && velocityY >= -0.01) velocityY = 0;

  //max speed reached?
  if (velocityY > 2)velocityY = 2;
  if (velocityY < -2)velocityY = -2;

  if (velocityX > 2)velocityX = 2;
  if (velocityX < -2)velocityX = -2;
}
void ship() {
  
  // if (abs(velocityX) > 0.02)
    shipX = shipX + velocityX;
  // if (abs(velocityY) > 0.02)
    shipY = shipY + velocityY;
  if (shipX < 5) shipX = mp.display.width() - 5;
  if (shipX > mp.display.width() - 5) shipX = 5;
  if (shipY < 4) shipY = mp.display.height() - 4;
  if (shipY > mp.display.height() - 4) shipY = 4;
  if(!invincibility || (invincibility && (millis()-invincMillis)%250 >= 100))
  {
    mp.display.fillTriangle(shipX0*2, shipY0*2, shipX1*2, shipY1*2, shipX2*2, shipY2*2, TFT_LIGHTGREY);
    mp.display.drawTriangle(shipX0*2, shipY0*2, shipX1*2, shipY1*2, shipX2*2, shipY2*2, TFT_LIGHTGREY);
    // mp.display.drawTriangle(shipX0*2, shipY0*2, shipX1*2, shipY1*2, shipX2*2, shipY2*2, TFT_DARKGREY);
    mp.display.fillTriangle(shipX0, shipY0, shipX1, shipY1, shipX2, shipY2, TFT_NAVY);
  }
}
void navigation() {
    if (mp.buttons.repeat(BTN_LEFT, 1))
	{
		HDG--;
	}
    if (mp.buttons.repeat(BTN_RIGHT, 1))
	{
		HDG++;
	}
    if (HDG > 23) HDG = HDG - 24;
    if (HDG < 0) HDG = HDG + 24;
    if (mp.buttons.repeat(BTN_UP, 1)) {
		
		velocityX += heading[HDG][0];
		velocityY += heading[HDG][1];

    }
    // if (mp.buttons.repeat(BTN_DOWN, 1)) {
		// if(velocityX!=0)
    // {
		//   velocityX -= heading[HDG][0];
    //   if((heading[HDG][0] > 0 && velocityX < 0) || (heading[HDG][0] < 0 && velocityX > 0))
    //     velocityX = 0;
    // }
    // if(velocityY!=0)
    // {
	  // 	velocityY -= heading[HDG][1];
    //   if((heading[HDG][1] > 0 && velocityY < 0) || (heading[HDG][1] < 0 && velocityY > 0))
    //     velocityY = 0;
    // }

    // }
  // if (mp.buttons.pressed(BTN_DOWN)) {
  //   delay(300);
  //   if (random(0, 3) == 2) {
  //     delay(200);
  //     collision();
  //   } else {
  //     velocityX = velocityY = 0;
  //     shipX = random(0, 128);
  //     shipY = random(0, 64);
  //   }
  // }
}

void firing() {
  bullet[bulletCount][1] = (short)(round)(pgm_read_float(&heading[HDG][0]) * 20);
  bullet[bulletCount][3] = (short)(round)(pgm_read_float(&heading[HDG][1]) * 20);
  bullet[bulletCount][0] = shipX1;
  bullet[bulletCount][2] = shipY1;
  bullet[bulletCount][4] = 120;
  bulletCount = bulletCount + 1;
  shoot->note(75, 0.05);
  
  
  // shoot->rewind();
  // shoot->play();
}
void trajectory() {
  	for (uint8_t t = 0; t < bulletCount; t++) {
		bullet[t][0] = bullet[t][0] + bullet[t][1];
		bullet[t][2] = bullet[t][2] + bullet[t][3];
		if (bullet[t][0] < -5) bullet[t][0] = bullet[t][0] + mp.display.width()+10;
		if (bullet[t][0] > mp.display.width()+5) bullet[t][0] = bullet[t][0] - mp.display.width()-10;
		if (bullet[t][2] < -5) bullet[t][2] = bullet[t][2] + mp.display.height()+10;
		if (bullet[t][2] > mp.display.height()+5) bullet[t][2] = bullet[t][2] - mp.display.height()-10;
		if (bullet[t][4] < 1) {
			bullet[t][4] = 0;
			for (int8_t a = 0; a < bulletCount - 1; a++)
				for (int8_t b = 0; b < 5; b++)
					bullet[a][b] = bullet[a + 1][b];
			bullet[bulletCount][0] = 0;
			bullet[bulletCount][1] = 0;
			bullet[bulletCount][2] = 0;
			bullet[bulletCount][3] = 0;
			bullet[bulletCount][4] = 0;
			bulletCount--;
		}
		mp.display.fillRect(bullet[t][0], bullet[t][2], 2, 2, TFT_WHITE);
		bullet[t][4] = bullet[t][4] - 4;
  	}
}
void asteroid() {
  for (uint8_t t = 0; t < asteroidCount; t++) {
    asteroids[t][0] = asteroids[t][0] + asteroids[t][1];
    asteroids[t][2] = asteroids[t][2] + asteroids[t][3];
    if (asteroids[t][0] < 5) asteroids[t][0] = mp.display.width() - 5;
    if (asteroids[t][0] > mp.display.width() - 5) asteroids[t][0] = 5;
    if (asteroids[t][2] < 5) asteroids[t][2] = mp.display.height() - 5;
    if (asteroids[t][2] > mp.display.height() - 5) asteroids[t][2] = 5;
    for (uint8_t b = 0; b < bulletCount; b++) {
      if ((abs(bullet[b][0] - asteroids[t][0]) <= 9 && abs(bullet[b][2] - asteroids[t][2]) <= 9) || (abs(bullet[b][0] - asteroids[t][0]) + abs(bullet[b][2] - asteroids[t][2]) < 13))
      {
        for (uint8_t a = b; a < bulletCount - 1; a++) {
          bullet[a][0] = bullet[a + 1][0];
          bullet[a][1] = bullet[a + 1][1];
          bullet[a][2] = bullet[a + 1][2];
          bullet[a][3] = bullet[a + 1][3];
          bullet[a][4] = bullet[a + 1][4];
        }
        bullet[bulletCount][0] = 0;
        bullet[bulletCount][1] = 0;
        bullet[bulletCount][2] = 0;
        bullet[bulletCount][3] = 0;
        bullet[bulletCount][4] = 0;
        bulletCount = bulletCount - 1;
        // hit->rewind();
        // hit->play();
        hit->note(10, 0.05);
        
        // sound.tone(150, 75);
        score = score + 2;

        rocks[rockCount][0] = asteroids[t][0];
        rocks[rockCount][2] = asteroids[t][2];
        rocks[rockCount][1] = random(-10, 10) * 0.05;
        rocks[rockCount][3] = random(-10, 10) * 0.05;
        rockDraw[rockCount] = random(0, 3);
        rockCount = rockCount + 1;
        rocks[rockCount][0] = asteroids[t][0];
        rocks[rockCount][2] = asteroids[t][2];
        rocks[rockCount][1] = random(-10, 10) * 0.05;
        rocks[rockCount][3] = random(-10, 10) * 0.05;
        rockDraw[rockCount] = random(0, 3);
        rockCount = rockCount + 1;

        for (uint8_t c = t; c < asteroidCount - 1; c++) {
          asteroidDraw[c] = asteroidDraw[c + 1];
          for (uint8_t d = 0; d < 4; d++)
            asteroids[c][d] = asteroids[c + 1][d];
        }
        asteroidCount = asteroidCount - 1;
      }
    }
    switch (asteroidDraw[t]) {
      case 0: mp.display.drawIcon(asteroid1, asteroids[t][0] - 8, asteroids[t][2] - 8, 24,24, 1, TFT_WHITE); break;
      case 1: mp.display.drawIcon(asteroid2, asteroids[t][0] - 8, asteroids[t][2] - 8, 24,24, 1, TFT_WHITE); break;
      case 2: mp.display.drawIcon(asteroid3, asteroids[t][0] - 8, asteroids[t][2] - 8, 24,24, 1, TFT_WHITE); break;
      default: mp.display.drawCircle(asteroids[t][0], asteroids[t][2], 5, TFT_WHITE);
    }
  }
}
void rock() {
  for (uint8_t r = 0; r < rockCount; r++) {
    rocks[r][0] = rocks[r][0] + rocks[r][1];
    rocks[r][2] = rocks[r][2] + rocks[r][3];
    if (rocks[r][0] < 4) rocks[r][0] = mp.display.width() - 4;
    if (rocks[r][0] > mp.display.width() - 4) rocks[r][0] = 4;
    if (rocks[r][2] < 4) rocks[r][2] = mp.display.height() - 4;
    if (rocks[r][2] > mp.display.height() - 4) rocks[r][2] = 4;
    for (uint8_t b = 0; b < bulletCount; b++) {
      if ((abs(bullet[b][0] - rocks[r][0]) <= 7 && abs(bullet[b][2] - rocks[r][2]) <= 7) ||
          (abs(bullet[b][0] - rocks[r][0]) + abs(bullet[b][2] - rocks[r][2]) < 11)) {
        for (uint8_t a = b; a < bulletCount - 1; a++) {
          bullet[a][0] = bullet[a + 1][0];
          bullet[a][1] = bullet[a + 1][1];
          bullet[a][2] = bullet[a + 1][2];
          bullet[a][3] = bullet[a + 1][3];
          bullet[a][4] = bullet[a + 1][4];
        }
        bullet[bulletCount][0] = 0;
        bullet[bulletCount][1] = 0;
        bullet[bulletCount][2] = 0;
        bullet[bulletCount][3] = 0;
        bullet[bulletCount][4] = 0;
        bulletCount = bulletCount - 1;

        // sound.tone(150, 75);
        hit->note(10, 0.05);
        
        // hit->rewind();
        // hit->play();
        score = score + 4;

        pebbles[pebbleCount][0] = rocks[r][0];
        pebbles[pebbleCount][2] = rocks[r][2];
        pebbles[pebbleCount][1] = random(-10, 10) * 0.05;
        pebbles[pebbleCount][3] = random(-10, 10) * 0.05;
        pebbleDraw[pebbleCount] = random(0, 3);
        pebbleCount = pebbleCount + 1;
        pebbles[pebbleCount][0] = rocks[r][0];
        pebbles[pebbleCount][2] = rocks[r][2];
        pebbles[pebbleCount][1] = random(-10, 10) * 0.05;
        pebbles[pebbleCount][3] = random(-10, 10) * 0.05;
        pebbleDraw[pebbleCount] = random(0, 3);
        pebbleCount = pebbleCount + 1;

        for (uint8_t c = r; c < rockCount - 1; c++) {
          rockDraw[c] = rockDraw[c + 1];
          for (uint8_t d = 0; d < 4; d++)
            rocks[c][d] = rocks[c + 1][d];
        }
        rockCount = rockCount - 1;
      }
    }
    switch (rockDraw[r]) {
      case 0: mp.display.drawIcon(rock1, rocks[r][0] - 4, rocks[r][2] - 4, 16, 16, 1, TFT_WHITE); break;
      case 1: mp.display.drawIcon(rock2, rocks[r][0] - 4, rocks[r][2] - 4, 16, 16, 1, TFT_WHITE); break;
      case 2: mp.display.drawIcon(rock3, rocks[r][0] - 4, rocks[r][2] - 4, 16, 16, 1, TFT_WHITE); break;
      default: mp.display.drawCircle(rocks[r][0], rocks[r][2], 4, TFT_WHITE);
    }
  }
}
void pebble() {
  for (uint8_t p = 0; p < pebbleCount; p++) {
    pebbles[p][0] = pebbles[p][0] + pebbles[p][1];
    pebbles[p][2] = pebbles[p][2] + pebbles[p][3];
    if (pebbles[p][0] < 3) pebbles[p][0] = mp.display.width() - 3;
    if (pebbles[p][0] > mp.display.width() - 3) pebbles[p][0] = 3;
    if (pebbles[p][2] < 3) pebbles[p][2] = mp.display.height() - 3;
    if (pebbles[p][2] > mp.display.height() - 3) pebbles[p][2] = 3;
    for (uint8_t b = 0; b < bulletCount; b++) {
      if (abs(bullet[b][0] - pebbles[p][0]) <= 3 && abs(bullet[b][2] - pebbles[p][2]) <= 3) {
        for (uint8_t a = b; a < bulletCount - 1; a++) {
          bullet[a][0] = bullet[a + 1][0];
          bullet[a][1] = bullet[a + 1][1];
          bullet[a][2] = bullet[a + 1][2];
          bullet[a][3] = bullet[a + 1][3];
          bullet[a][4] = bullet[a + 1][4];
        }
        bulletCount = bulletCount - 1;
        // hit->rewind();
        // hit->play();
        hit->note(10, 0.05);
        
        // sound.tone(150, 75);
        score = score + 8;

        for (uint8_t c = p; c < pebbleCount - 1; c++) {
          pebbleDraw[c] = pebbleDraw[c + 1];
          for (uint8_t d = 0; d < 4; d++)
            pebbles[c][d] = pebbles[c + 1][d];
        }
        pebbleCount = pebbleCount - 1;
      }
    }
    switch (pebbleDraw[p]) {
      case 0: mp.display.drawIcon(pebble1, pebbles[p][0] - 2, pebbles[p][2] - 2, 10, 10, 1, TFT_WHITE); break;
      case 1: mp.display.drawIcon(pebble2, pebbles[p][0] - 2, pebbles[p][2] - 2, 10, 10, 1, TFT_WHITE); break;
      case 2: mp.display.drawIcon(pebble3, pebbles[p][0] - 2, pebbles[p][2] - 2, 10, 10, 1, TFT_WHITE); break;
      default: mp.display.drawCircle(pebbles[p][0], pebbles[p][2], 2, TFT_WHITE);
    }
  }
}
void radar() {
  if(invincibility)
  {
    if(millis() - invincMillis >= INVINCIBILITY_PERIOD)
    {
      invincibility = 0;
    }
    else
      return;
  }

  for (uint8_t a = 0; a < asteroidCount; a ++) {
    if (abs(asteroids[a][0] - shipX) < 17 && abs(asteroids[a][2] - shipY) < 17) {
    //   sound.tone(150, 50);
      collision();
      rocks[rockCount][0] = asteroids[a][0] + asteroids[a][1];
      rocks[rockCount][2] = asteroids[a][2] + asteroids[a][3];
      rocks[rockCount][1] = - asteroids[a][1];
      rocks[rockCount][3] = - asteroids[a][3];
      rockDraw[rockCount] = random(0, 3);
      rockCount = rockCount + 1;
      for (uint8_t c = a; c < asteroidCount - 1; c++)
        for (uint8_t d = 0; d < 4; d++)
          asteroids[c][d] = asteroids[c + 1][d];
      asteroidCount = asteroidCount - 1;
    }
  }
  for (uint8_t r = 0; r < rockCount; r ++) {
    if (abs(rocks[r][0] - shipX) < 13 && abs(rocks[r][2] - shipY) < 13) {
    //   sound.tone(150, 50);
      collision();
      pebbles[pebbleCount][0] = rocks[r][0] + rocks[r][1];
      pebbles[pebbleCount][2] = rocks[r][2] + rocks[r][3];
      pebbles[pebbleCount][1] = - rocks[r][1];
      pebbles[pebbleCount][3] = - rocks[r][3];
      pebbleDraw[pebbleCount] = random(0, 3);
      pebbleCount = pebbleCount + 1;
      for (uint8_t c = r; c < rockCount - 1; c++)
        for (uint8_t d = 0; d < 4; d++)
          rocks[c][d] = rocks[c + 1][d];
      rockCount = rockCount - 1;
    }
  }
  for (uint8_t p = 0; p < pebbleCount; p ++) {
    if (abs(pebbles[p][0] - shipX) < 9 && abs(pebbles[p][2] - shipY) < 9) {
      // delay(200);
      collision();
      for (uint8_t c = p; c < pebbleCount - 1; c++)
        for (uint8_t d = 0; d < 4; d++)
          pebbles[c][d] = pebbles[c + 1][d];
      pebbleCount = pebbleCount - 1;
    }
  }
  if (asteroidCount == 0 && rockCount == 0 && pebbleCount == 0) {
    tick = tick - 1;
    if (tick <= 0) {
      tick = 20;
      if (level < 8) level = level + 1;

      for (int8_t s = 0; s < level; s++) {
        uint8_t tempX = 112 + random(0, 106);
        uint8_t tempY = 80 + random(0, 96);
        while(abs(tempX - shipX) < 40 && abs(tempY-shipY) < 40)
        {
          tempX = random(10, 150);
          tempY = random(10, 118);
        }
        asteroids[asteroidCount][0] = tempX;
        asteroids[asteroidCount][2] = tempY;
        if (asteroids[asteroidCount][0] < 5) asteroids[asteroidCount][0] = mp.display.width() - 5;
        if (asteroids[asteroidCount][0] > mp.display.width() - 5) asteroids[asteroidCount][0] = 5;
        if (asteroids[asteroidCount][2] < 5) asteroids[asteroidCount][2] = mp.display.height() - 5;
        if (asteroids[asteroidCount][2] > mp.display.height() - 5) asteroids[asteroidCount][2] = 5;

        asteroids[asteroidCount][1] = random(-10, 10) * 0.05;
        asteroids[asteroidCount][3] = random(-10, 10) * 0.05;
        asteroidDraw[asteroidCount] = random(0, 3);
        asteroidCount = asteroidCount + 1;
      }
    }
  }
}
void dyingAnimation()
{
  uint32_t elapsedMillis = millis();
  uint8_t passes = 1;
  while(passes < 7)
  {
    
    if(millis()-elapsedMillis >= 200)
    {
      mp.display.drawIcon(backdrop, 0, 0, 160, 128);
      mp.display.drawLine(shipX, shipY + passes*2, shipX, shipY + passes*2 + 3, TFT_WHITE);
      mp.display.drawLine(shipX, shipY - passes*2, shipX, shipY - passes*2 - 3, TFT_WHITE);
      mp.display.drawLine(shipX + passes*2, shipY, shipX + passes*2 + 3, shipY, TFT_WHITE);
      mp.display.drawLine(shipX - passes*2, shipY, shipX - passes*2 - 3, shipY, TFT_WHITE);
      mp.display.drawLine(shipX + passes*2, shipY + passes*2, shipX + passes*2 + 2, shipY + passes*2 + 2, TFT_WHITE);
      mp.display.drawLine(shipX - passes*2, shipY - passes*2, shipX - passes*2 - 2, shipY - passes*2 - 2, TFT_WHITE);
      mp.display.drawLine(shipX + passes*2, shipY - passes*2, shipX + passes*2 + 2, shipY - passes*2 - 2, TFT_WHITE);
      mp.display.drawLine(shipX - passes*2, shipY + passes*2, shipX - passes*2 - 2, shipY + passes*2 + 2, TFT_WHITE);
      passes++;
      elapsedMillis = millis();
    }
    while(!mp.update());
  }
}
void resetField() {
  level = 0;
  velocityX = velocityY = 0;
  tick = 80;
  bulletCount = 0;
  asteroidCount = rockCount = pebbleCount = 0;
}
void resetSim() {
	HDG = 0;
	shipY = mp.display.height()/2;
	shipX = mp.display.width()/2;
	resetField();
	// if(bgmusic->isPlaying())
	// {
		// bgmusic->stop();
  for(uint8_t i = 0; i < 4; i++)
    if(tracks[i] != nullptr)
		  removeTrack(tracks[i]);
	// }
	addTrack(titleMusic);
  titleMusic->rewind();
  if(mp.mediaVolume == 0)
    titleMusic->setVolume(0);
  else
	  titleMusic->setVolume(map(mp.mediaVolume, 0, 14, 100, 300));
	titleMusic->setRepeat(1);
	titleMusic->play();
	simState = ProgState::Main;
	score = 0;
}
void enterInitials() {
  name = "";
  String previous = "";
  uint32_t elapsedMillis = millis();
  uint32_t hiscoreMillis = millis();
  bool blinkState = 1;
  bool hiscoreBlink = 0;
  mp.textPointer = 0;
  while ((!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_FUN_RIGHT)) || name.length() != 3) {
	if(mp.buttons.released(BTN_HOME)) mp.lockscreen();
	mp.update();
    name = mp.textInput(name, 3);
    if (millis() - elapsedMillis >= multi_tap_threshold) //cursor blinking routine
	{
		elapsedMillis = millis();
		blinkState = !blinkState;
	}
    if(millis()-hiscoreMillis >= 1000)
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
    
    mp.display.drawIcon(backdrop, 0, 0, 160, 128);
    mp.display.setCursor(16, 8);
    mp.display.setTextFont(2);
    mp.display.setTextColor(TFT_WHITE);
    mp.display.setTextSize(1);
    mp.display.printCenter("ENTER NAME");
    mp.display.setCursor(39, 80);
    
    if(hiscoreBlink && score > tempScore)
		mp.display.printCenter("NEW HIGH!");
    else
		mp.display.printf("SCORE: %04d", score);

    mp.display.setCursor(50, 40);
    mp.display.printCenter(name);
    if(blinkState)
		mp.display.drawFastVLine(mp.display.getCursorX(), mp.display.getCursorY()+3, 10, TFT_WHITE);
    mp.display.drawRect(30, 38, 100, 20, TFT_WHITE);
	mp.display.setCursor(118, 110);
	mp.display.setTextFont(2);
	mp.display.setTextSize(1);
	mp.display.setTextColor(TFT_WHITE);
	mp.display.printCenter("Erase            Confirm");

  }
  while(!mp.update());
}

void setup() {
	Serial.begin(115200);
	mp.begin();
	mp.dataRefreshFlag = 0;
	// shoot = new MPTrack("/SpaceRocks/shoot.wav");
	collide = new MPTrack("/SpaceRocks/collide.wav");
	hit = new Oscillator(NOISE);
	bgmusic = new MPTrack("/SpaceRocks/bgmusic.wav");
	titleMusic = new MPTrack("/SpaceRocks/title.wav");
	gameoverMusic = new MPTrack("/SpaceRocks/gameover.wav");
	destroyed = new MPTrack("/SpaceRocks/destroyed.wav");
	shoot = new Oscillator(SQUARE);
	addOscillator(shoot);
	addOscillator(hit);
		// addTrack(shoot);
	// addOscillator(shoot);
		addTrack(collide);
		// addTrack(hit);
	file = SD.open(highscoresPath);
	JsonArray &hiscores = mp.jb.parseArray(file);
	file.close();
	if(hiscores.success())
		savePresent = 1;
	else
	{
		Serial.println("No save present");
		JsonObject &test = mp.jb.createObject();
		test["Name"] = "ABC";
		test["Score"] = 0;
		test["Rank"] = 1;
		hiscores.add(test);
		hiscores.prettyPrintTo(Serial);
		file = SD.open(highscoresPath, "w");
		hiscores.prettyPrintTo(file);
		hiscores.prettyPrintTo(Serial);
		file.close();
		savePresent = 1;
	}
	hiscores.prettyPrintTo(Serial);
	shoot->setVolume(mp.oscillatorVolumeList[mp.mediaVolume]);
	hit->setVolume(mp.oscillatorVolumeList[mp.mediaVolume]/2);
	// shoot->setADSR(5, 20.5, 20.5, 20.5);
	if(mp.mediaVolume == 0)
	{
		titleMusic->setVolume(0);
		// shoot->setVolume(0);
		// hit->setVolume(0);
		gameoverMusic->setVolume(0);
		bgmusic->setVolume(0);
		destroyed->setVolume(0);
	}
	else
	{
		// shoot->setVolume(map(mp.mediaVolume, 0, 14, 100, 300));
		collide->setVolume(map(mp.mediaVolume, 0, 14, 100, 300));
		// hit->setVolume(map(mp.mediaVolume, 0, 14, 100, 300));
		titleMusic->setVolume(map(mp.mediaVolume, 0, 14, 100, 300));
		gameoverMusic->setVolume(map(mp.mediaVolume, 0, 14, 100, 300));
		bgmusic->setVolume(map(mp.mediaVolume, 0, 14, 100, 300));
		destroyed->setVolume(map(mp.mediaVolume, 0, 14, 100, 300));
	}
	randomSeed(millis());
	resetSim();
}

uint32_t randMillis = millis();
uint32_t shootMillis = millis();
uint32_t turnMillis = millis();
long unsigned int startMillis;
const char *titleMenu[3] PROGMEM = {"Start", "Hiscores", "Quit"};
uint8_t titleCursor = 0;
bool blinkState = 0;
uint32_t blinkMillis = millis();
void loop() 
{
 	if(mp.update())
	{
		mp.display.fillScreen(TFT_BLACK);
    	switch (simState)
		{
			case ProgState::Main: {
				invincibility = 0;
				mp.display.drawIcon(backdrop, 0, 0, 160, 128);
				//begin homescreen
				mp.display.setCursor(10, 10);
				mp.display.setTextSize(2);
				
				turnC = random(0, 2) == 1;
				if (millis() - turnMillis >= 300) {
					turnMillis = millis();
					if (turnC)
						HDG = HDG + 1;
					else
						HDG = HDG - 1;
				}
				if (HDG > 23) HDG = HDG - 24;
				if (HDG < 0) HDG = HDG + 24;
				velocityX = 1;
				shipY = 98;
				ship();
				if (millis()-shootMillis >= 150 && (bulletCount < 8)) {
					shootMillis = millis();
					bullet[bulletCount][1] = (short)(round)(pgm_read_float(&heading[HDG][0]) * 20);
					bullet[bulletCount][3] = (short)(round)(pgm_read_float(&heading[HDG][1]) * 20);
					bullet[bulletCount][0] = shipX1;
					bullet[bulletCount][2] = shipY1;
					bullet[bulletCount][4] = 100;
					bulletCount = bulletCount + 1;
				}
				if(millis() - blinkMillis > 200)
				{
					blinkMillis = millis();
					blinkState = !blinkState;
				}
				trajectory();
				mp.display.drawIcon(title, 5, 5, 150, 75, 1, TFT_WHITE);
				mp.display.setCursor(118, 110);
				mp.display.setTextFont(2);
				mp.display.setTextSize(1);
				mp.display.setTextColor(TFT_WHITE);
				mp.display.drawRect(34, 110, 92, 18, TFT_WHITE);
				mp.display.drawRect(33, 109, 94, 20, TFT_WHITE);

				mp.display.setCursor(18*2, 110);
				mp.display.printCenter(titleMenu[titleCursor]);
				if(blinkState)
				{
					if (titleCursor == 0)
					{
						mp.display.drawBitmap(11*2, 112, arrowLeft, TFT_WHITE, 2);
						mp.display.drawBitmap(66*2, 112, arrowRight, TFT_WHITE, 2);
					}
					else if (titleCursor == 2)
					{
						mp.display.drawBitmap(10*2, 112, arrowLeft, TFT_WHITE, 2);
						mp.display.drawBitmap(65*2, 112, arrowRight, TFT_WHITE, 2);
					}
					else
					{
						mp.display.drawBitmap(10*2, 112, arrowLeft, TFT_WHITE, 2);
						mp.display.drawBitmap(66*2, 112, arrowRight, TFT_WHITE, 2);
					}
				}
				else
				{
					mp.display.drawBitmap(11*2, 112, arrowLeft, TFT_WHITE, 2);
					mp.display.drawBitmap(65*2, 112, arrowRight, TFT_WHITE, 2);
				}
				if (mp.buttons.released(BTN_A))
				{
					mp.buttons.update();
					switch (titleCursor)
					{
						case 0:
							life = 3;
							resetSim();
							resetField();
							// addOscillator(shoot);
							addTrack(collide);
							// addTrack(hit);
							simState = ProgState::Simulation;
							titleMusic->stop();
							removeTrack(titleMusic);
							addTrack(bgmusic);
							if(mp.mediaVolume == 0)
								bgmusic->setVolume(0);
							else
								bgmusic->setVolume(map(mp.mediaVolume, 0, 14, 100, 300));
							bgmusic->setRepeat(1);
							bgmusic->rewind();
							bgmusic->play();
							break;
						
						case 1:

							simState = ProgState::DataDisplay;
							break;
						case 2:
							mp.loader();
							break;
					}
					titleCursor = 0;
				}
				if(mp.buttons.released(BTN_LEFT) && titleCursor > 0)
				{
					mp.buttons.update();
					titleCursor--;
				}
				if(mp.buttons.released(BTN_RIGHT) && titleCursor < 2)
				{
					mp.buttons.update();
					titleCursor++;
				}
			}
			break;
			case ProgState::Simulation: {

				mp.display.drawIcon(backdrop, 0, 0, 160, 128);

				if (life > 0) {
					navigation();
					guidance();
					ship();
					trajectory();

					
					if(millis()-pixelsTimer >= 50 && pixelsState==1)
						FastLED.clear();
					asteroid();
					rock();
					pebble();
					radar();
					mp.display.setTextFont(2);
					mp.display.setTextSize(1);
					mp.display.setTextColor(TFT_GREEN);
					mp.display.setCursor(4, 2);
					mp.display.printf("LV:%d      %04d       X%d", level, score, life);
					int16_t tempHDG = HDG;
					float tempX = shipX;
					float tempY = shipY;
					HDG = 0;
					shipX = 130;
					shipY = 10;
					mp.display.fillTriangle(shipX0*2, shipY0*2, shipX1*2, shipY1*2, shipX2*2, shipY2*2, TFT_LIGHTGREY);
					mp.display.drawTriangle(shipX0*2, shipY0*2, shipX1*2, shipY1*2, shipX2*2, shipY2*2, TFT_LIGHTGREY);
					mp.display.fillTriangle(shipX0, shipY0, shipX1, shipY1, shipX2, shipY2, TFT_NAVY);
					HDG = tempHDG;
					shipX = tempX;
					shipY = tempY;
					// if (life == 0) 
					//   resetField();
					if (mp.buttons.released(BTN_A) && (bulletCount < 7))
						firing();
					if (mp.buttons.released(BTN_B)) 
					{
						bgmusic->pause();
						simState = ProgState::Pause;
					}
				}
				else 
				{
					delay(500);
					bgmusic->stop();
					removeTrack(bgmusic);
					addTrack(destroyed);
					if(mp.mediaVolume == 0)
						destroyed->setVolume(0);
					else
						destroyed->setVolume(map(mp.mediaVolume, 0, 14, 100, 300));
					destroyed->play();
					dyingAnimation();
					mp.display.drawIcon(backdrop, 0, 0, 160, 128);
					while(!mp.update());
					for (int i = 0; i < mp.display.height(); i+=4)
					{
						mp.display.drawFastHLine(0, i, mp.display.width(), TFT_DARKGREY);
						mp.display.drawFastHLine(0, i+1, mp.display.width(), TFT_DARKGREY);
						if(i % 4 == 0)
							mp.display.pushSprite(0,0);
						// delayMicroseconds(750);
					}
					mp.display.drawBitmap(26, 15, gameover, TFT_DARKGREY);
					mp.display.drawBitmap(28, 13, gameover, TFT_BLACK);
					destroyed->stop();
					removeTrack(destroyed);
					addTrack(gameoverMusic);
					if(mp.mediaVolume == 0)
						gameoverMusic->setVolume(0);
					else
						gameoverMusic->setVolume(map(mp.mediaVolume, 0, 14, 100, 300));
					gameoverMusic->play();
					gameoverMusic->setRepeat(1);
					while(!mp.buttons.released(BTN_A) && !mp.buttons.released(BTN_B))
						mp.update();
					while(!mp.update());
					simState = ProgState::DataEntry;
				}
			}
			break;

			case ProgState::DataEntry:
			{
				// shoot->stop();
				bgmusic->stop();
				collide->stop();
				// hit->stop();
				gameoverMusic->stop();
				// removeTrack(shoot);
				removeTrack(bgmusic);
				removeTrack(collide);
				// removeTrack(hit);
				removeTrack(gameoverMusic);
				file = SD.open(highscoresPath);
				JsonArray &hiscores1 = mp.jb.parseArray(file);
				file.close();
				for (JsonObject& element : hiscores1)
				{
					if(element["Rank"] == 1)
						tempScore = element["Score"].as<int>();
				}	
				enterInitials();
				file = SD.open(highscoresPath);
				JsonArray &hiscores = mp.jb.parseArray(file);
				file.close();
				JsonObject &newHiscore = mp.jb.createObject();
				newHiscore["Name"] = name;
				newHiscore["Score"] = score;
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
						if(score >= (uint16_t)(hiscores[i]["Score"]))
						{
							Serial.println("ENTERED");
							delay(5);
							if((uint16_t)(newHiscore["Rank"]) >  (uint16_t)(hiscores[i]["Rank"]))
								newHiscore["Rank"] = (uint16_t)(hiscores[i]["Rank"]);
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
				simState = ProgState::DataDisplay;
			}
			break;

			case ProgState::DataDisplay:
			{
				// shoot->stop();
				titleMusic->stop();
				collide->stop();
				// hit->stop();
				// removeTrack(shoot);
				// removeTrack(titleMusic);
				// removeTrack(collide);
				// removeTrack(hit);
				mp.jb.clear();
				file = SD.open(highscoresPath);
				JsonArray &hiscores = mp.jb.parseArray(file);
				file.close();
				const char* nameArray[6];
				memset(nameArray, 0, 6);
				uint16_t scoreArray[6];
				memset(scoreArray, 0, 6);
				uint16_t hiscoresSize = hiscores.size();
				for(uint8_t i = 0; i < 6; i++)
				{
					for(JsonObject& element:hiscores)
					{
						if(element["Rank"] == i)
						{
							nameArray[i] = element["Name"];
							scoreArray[i] = element["Score"];
						}
					}
				}
						
				while(1)
				{
					mp.display.drawIcon(backdrop, 0, 0, 160, 128);
					mp.display.setCursor(32, 0);
					mp.display.setTextSize(1);
					mp.display.setTextFont(2);
					mp.display.setTextColor(TFT_WHITE);
					mp.display.printCenter("HIGHSCORES");
					mp.display.setCursor(4, 112);
					mp.display.print("Erase");
					for(int i = 1; i < 6;i++)
					{
						mp.display.setCursor(27, i * 19);
						if(i <= hiscores.size())
							mp.display.printf("%d.   %.3s    %04d", i, nameArray[i], scoreArray[i]);
						else
							mp.display.printf("%d.    ---   ----", i);
					}
					if(mp.buttons.released(BTN_B))
					{
						while (!mp.update());          
						resetSim();
						break;
					}
					if(mp.buttons.released(BTN_FUN_LEFT))
					{
						simState = ProgState::DataErasure;
						break;
					}
					mp.update();
				}
				// addTrack(shoot);
				// addTrack(collide);
				// addTrack(hit);
				// addTrack(titleMusic);
			}
			break;

			case ProgState::Pause:
				mp.display.drawIcon(backdrop, 0, 0, 160, 128);
				mp.display.setCursor(32, mp.display.height()/2 - 25);
				mp.display.setTextSize(2);
				mp.display.setTextFont(2);
				mp.display.setTextColor(TFT_WHITE);
				mp.display.printCenter(F("PAUSE"));
				mp.display.setTextSize(1);
				mp.display.setCursor(110,110);
				mp.display.printCenter("A: resume       B: quit");
				if (mp.buttons.released(BTN_A))
				{
					simState = ProgState::Simulation;
					bgmusic->resume();
				}
				if (mp.buttons.released(BTN_B))
					resetSim();
			break;

			case ProgState::DataErasure:
				while(1)
				{
					mp.display.drawIcon(backdrop, 0, 0, 160, 128);
					mp.display.setTextFont(2);
					mp.display.setTextColor(TFT_WHITE);
					mp.display.setCursor(4, 5);
					mp.display.printCenter("ARE YOU SURE?");
					mp.display.setCursor(4, 25);
					mp.display.printCenter("This cannot be reverted");

					mp.display.fillRect(110, 102, 45, 9*2, TFT_RED);
					mp.display.fillRect(5, 102, 45, 9*2, TFT_GREENYELLOW);
					mp.display.setTextColor(TFT_BLACK);
					mp.display.setCursor(17, 103);
					mp.display.print("Yes");
					mp.display.setCursor(126, 103);
					mp.display.print("No");

					if (mp.buttons.released(BTN_B) || mp.buttons.released(BTN_FUN_RIGHT)) //BUTTON BACK
					{
						Serial.println("Go back");
						while (!mp.update());
						simState = ProgState::DataDisplay;
						break;
					}
					if (mp.buttons.released(BTN_A) || mp.buttons.released(BTN_FUN_LEFT)) // DELETE
					{
						while(!mp.update());
						for(int i = 0; i< 4; i++)
						{
							if(tracks[i] != nullptr)
							removeTrack(tracks[i]);
						}
						JsonArray &empty = mp.jb.createArray();
						file = SD.open(highscoresPath, "w");
						empty.prettyPrintTo(file);
						file.close();
						// addTrack(shoot);
						addTrack(collide);
						// addTrack(hit);
						addTrack(titleMusic);
						resetSim();
						simState = ProgState::Main;
						break;
					}
					mp.update();
				}
			break;
		}
	}
}