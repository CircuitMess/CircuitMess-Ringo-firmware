/*
    Invaderz
    Copyright (C) 2019 CircuitMess
    original game:
    Invaders by Yoda Zhang
    http://www.yodasvideoarcade.com/images/gamebuino-invaders.zip
    Ported to MAKERphone by CircuitMess
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
//----------------------------------------------------------------------------    
// define variables and constants
//----------------------------------------------------------------------------    
#include <MAKERphone.h>
#include "Star.h"
MAKERphone mp;
uint32_t pixelsTimer=0;
bool pixelsState=0;
String gamestatus;
int score;
int lives;
int gamelevel;
int shipx;
int invaderx[55];
int invadery[55];
int invaders[55];
int invaderframe[55];
int invaderanz;
int invaderctr;
int invadersound;
int checkdir;
int nextxdir;
int nextydir;
int invaderxr;
int invaderyr;
int invadershotx[4];
int invadershoty[4];
int invadershotframe;
int invadershots;
int invadershottimer;
int bunkers[4];
int shotx;
int shoty;
int checkl;
int checkr;
int checkt;
int checkb;
int yeahtimer;
int infoshow;
int deadcounter;
int saucerx;
int saucerdir;
int saucers;
int saucertimer;
int saucerwait;
int delayBip;


// MPTrack *shootSound;
// MPTrack *invaderDestroyed;
// MPTrack *mainMusic;
// MPTrack *titleMusic;
// MPTrack *playerDestroyed;
// MPTrack *ufoSound;
// MPTrack *gameoverMusic;
Oscillator *shoot;
Oscillator *destroyed;
const char *highscoresPath = "/Invaderz/hiscores.sav";
bool savePresent = 0;
uint16_t tempScore = 0;
String name = "";
uint8_t cursor = 0;
char key = NO_KEY;
#define STAR_COUNT 50            // Number of stars on the screen. Arduino UNO maxes out around 250.
#define BACKGROUND_COLOR 0x0000   // Background color in hex. 0x0000 is black.
#define STAR_SPEED_MIN 1          // Minimum movement in pixels per update. (value is inclusive)
#define STAR_SPEED_MAX 3         // Maximum movement in pixels per update. (value is inclusive)
#define STAR_COLOR 0xffff  
Star stars[STAR_COUNT]; 
void starsSetup()
{
    // Loop through each star.
    for(int i = 0; i < STAR_COUNT; i++)
    {
        // Randomize its position and speed.
        stars[i].randomize(0, mp.display.width(), 0, mp.display.height(), STAR_SPEED_MIN, STAR_SPEED_MAX);
    }
} 

//----------------------------------------------------------------------------    
// define images & sounds
//----------------------------------------------------------------------------    
extern const byte gamelogo[];
extern const byte invader[8][7];
extern const byte playership[3][6];
extern const byte bunker[5][7];
extern const byte bomb[2][6];
extern const byte saucer[2][10];
extern const int soundfx[8][8];

//ported images
//----------------------------------------------------------------------------
const unsigned short titleLogo[0x5FD] PROGMEM ={
  0xFFFF, 0x7BCF, 0x7BCF, 0x7BCF, 0xFFFF, 0xFFFF, 0x7BCF, 0x7BCF, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x7BCF, 0x7BCF, 0x7BCF,   // 0x0010 (16)
  0xFFFF, 0x7BCF, 0x7BCF, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x7BCF, 0x7BCF, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0x7BCF, 0x7BCF,   // 0x0020 (32)
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0x7BCF,   // 0x0030 (48)
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xFFFF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xFFFF, 0xFFFF,   // 0x0040 (64)
  0xFFFF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800,   // 0x0050 (80)
  0xF800, 0x7BCF, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0060 (96)
  0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0x7BCF, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800,   // 0x0070 (112)
  0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800,   // 0x0080 (128)
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0x7BCF, 0x7BCF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0090 (144)
  0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800,   // 0x00A0 (160)
  0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800,   // 0x00B0 (176)
  0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0x7BCF,   // 0x00C0 (192)
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x00D0 (208)
  0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF,   // 0x00E0 (224)
  0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF,   // 0x00F0 (240)
  0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800,   // 0x0100 (256)
  0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0110 (272)
  0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0120 (288)
  0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0x7BCF, 0xFFFF,   // 0x0130 (304)
  0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF,   // 0x0140 (320)
  0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800,   // 0x0150 (336)
  0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF,   // 0x0160 (352)
  0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800,   // 0x0170 (368)
  0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800,   // 0x0180 (384)
  0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0x7BCF, 0xFFFF,   // 0x0190 (400)
  0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF,   // 0x01A0 (416)
  0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x01B0 (432)
  0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x01C0 (448)
  0x7BCF, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0x7BCF,   // 0x01D0 (464)
  0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF,   // 0x01E0 (480)
  0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF,   // 0x01F0 (496)
  0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800,   // 0x0200 (512)
  0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF,   // 0x0210 (528)
  0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0220 (544)
  0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800,   // 0x0230 (560)
  0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0x7BCF, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF,   // 0x0240 (576)
  0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800,   // 0x0250 (592)
  0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0x7BCF, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800,   // 0x0260 (608)
  0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF,   // 0x0270 (624)
  0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800,   // 0x0280 (640)
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF,   // 0x0290 (656)
  0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800,   // 0x02A0 (672)
  0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0x7BCF, 0xFFFF,   // 0x02B0 (688)
  0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800,   // 0x02C0 (704)
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF,   // 0x02D0 (720)
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800,   // 0x02E0 (736)
  0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF,   // 0x02F0 (752)
  0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800,   // 0x0300 (768)
  0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0310 (784)
  0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800,   // 0x0320 (800)
  0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0x7BCF, 0xF800,   // 0x0330 (816)
  0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800,   // 0x0340 (832)
  0x7BCF, 0xFFFF, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800,   // 0x0350 (848)
  0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800,   // 0x0360 (864)
  0xF800, 0x7BCF, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF,   // 0x0370 (880)
  0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800,   // 0x0380 (896)
  0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0x7BCF, 0x7BCF, 0xFFFF,   // 0x0390 (912)
  0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF,   // 0x03A0 (928)
  0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800,   // 0x03B0 (944)
  0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800,   // 0x03C0 (960)
  0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800,   // 0x03D0 (976)
  0xF800, 0xF800, 0x7BCF, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800,   // 0x03E0 (992)
  0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF,   // 0x03F0 (1008)
  0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800,   // 0x0400 (1024)
  0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF,   // 0x0410 (1040)
  0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0420 (1056)
  0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF,   // 0x0430 (1072)
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800,   // 0x0440 (1088)
  0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF,   // 0x0450 (1104)
  0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0x7BCF,   // 0x0460 (1120)
  0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF,   // 0x0470 (1136)
  0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800,   // 0x0480 (1152)
  0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0490 (1168)
  0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF,   // 0x04A0 (1184)
  0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800,   // 0x04B0 (1200)
  0xF800, 0xF800, 0x7BCF, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800,   // 0x04C0 (1216)
  0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF,   // 0x04D0 (1232)
  0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xF800, 0xF800,   // 0x04E0 (1248)
  0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x04F0 (1264)
  0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800,   // 0x0500 (1280)
  0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800,   // 0x0510 (1296)
  0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xF800, 0xF800, 0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0520 (1312)
  0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF,   // 0x0530 (1328)
  0xF800, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF,   // 0x0540 (1344)
  0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0x7BCF, 0x7BCF, 0xF800, 0xFFFF, 0xF800, 0xFFFF,   // 0x0550 (1360)
  0xF800, 0xFFFF, 0xF800, 0x7BCF, 0x7BCF, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF,   // 0x0560 (1376)
  0xF800, 0xFFFF, 0xF800, 0xF800, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0x7BCF, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0570 (1392)
  0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF,   // 0x0580 (1408)
  0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0590 (1424)
  0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF,   // 0x05A0 (1440)
  0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800,   // 0x05B0 (1456)
  0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x05C0 (1472)
  0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800,   // 0x05D0 (1488)
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF,   // 0x05E0 (1504)
  0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x05F0 (1520)
  0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF, 0xF800, 0xFFFF,
};
//----------------------------------------------------------------------------
const byte invader[8][7] PROGMEM = {
  {8,5, B00111000,B11010110,B11111110,B00101000,B11000110},
  {8,5, B00111000,B11010110,B11111110,B01000100,B00101000},
  {8,5, B00010000,B10111010,B11010110,B01111100,B00101000},
  {8,5, B00111000,B01010100,B11111110,B10010010,B00101000},
  {8,5, B00111000,B01010100,B01111100,B01000100,B00101000},
  {8,5, B00111000,B01010100,B01111100,B00101000,B01010100},
  {8,5, B01010100,B10000010,B01010100,B10000010,B01010100},
  {8,5, B01010100,B10000010,B01010100,B10000010,B01010100},
};
//----------------------------------------------------------------------------
const byte playership[3][6] PROGMEM = {
  {8,4, B00010000,B01111100,B11111110,B11111110},
  {8,4, B10001010,B01000000,B00000100,B10010010},
  {8,4, B10010010,B00000100,B01000000,B10001010},
};
//----------------------------------------------------------------------------
const byte bunker[5][7] PROGMEM = {
  {8,5, B01111110,B11111111,B11111111,B11100111,B11000011},
  {8,5, B01111110,B11011011,B11111111,B10100101,B11000011},
  {8,5, B01101110,B11011011,B01110110,B10100101,B11000011},
  {8,5, B01100110,B11011001,B01010110,B10100101,B01000010},
  {8,5, B00100010,B10001001,B01010010,B10100101,B01000010},
};
//----------------------------------------------------------------------------
const byte bomb[2][6] PROGMEM = {
  {2,4, B10000000,B01000000,B10000000,B01000000},
  {2,4, B01000000,B10000000,B01000000,B10000000},
};
//----------------------------------------------------------------------------
const byte saucer[2][10] PROGMEM = {
  {11,4, B00011111,B00000000,B01101010,B11000000,B11111111,B11100000,B01100100,B11000000},
  {11,4, B01011101,B11000000,B01010101,B01000000,B01010101,B01000000,B01011101,B11000000},
};
//----------------------------------------------------------------------------




//ported nonstandard
//----------------------------------------------------------------------------
void newgame() {
	score = 0;
	lives = 3;
	gamelevel = 0;
	shipx = 80;
	shotx = -1;
	shoty = -1;
	deadcounter = -1;
	saucers = -1;
  // removeTrack(titleMusic);
  // addTrack(mainMusic);
  // mainMusic->play();
  // addTrack(shootSound);
  // addTrack(invaderDestroyed);
  // addTrack(playerDestroyed);
  starsSetup();
	for (int i = 0; i < 4; i++) {
		invadershotx[i] = -1;
		invadershoty[i] = -1;
	}
	gamestatus = "newlevel";
}
//----------------------------------------------------------------------------
void newlevel() {
	invaderanz = 40;
	invaderctr = 39;
	invaderxr = 4;
	invaderyr = 4;
	checkdir = 0;
	nextxdir = 4;
	nextydir = 0;
	yeahtimer = 0;
	delayBip = 0;
	invadershottimer = 60;
	saucertimer = 240;
	int down = gamelevel;
	if (gamelevel > 8) { down = 16*2; }
	for (int i = 0; i < 8; i++) {
		invaderx[i] = 10 + i * 8*2;
		invaderx[i + 8] = 10 + i * 8*2;
		invaderx[i + 16] = 10 + i * 8*2;
		invaderx[i + 24] = 10 + i * 8*2;
		invaderx[i + 32] = 10 + i * 8*2;
		invadery[i] = 14 + down;
		invadery[i + 8] = 13*2 + down;
		invadery[i + 16] = 19*2 + down;
		invadery[i + 24] = 25*2 + down;
		invadery[i + 32] = 31*2 + down;
		invaders[i] = 4;
		invaders[i + 8] = 2;
		invaders[i + 16] = 2;
		invaders[i + 24] = 0;
		invaders[i + 32] = 0;
		invaderframe[i] = 0;
		invaderframe[i + 8] = 0;
		invaderframe[i + 16] = 0;
		invaderframe[i + 24] = 0;
		invaderframe[i + 32] = 0;
	}
	for (int i = 0; i < 4; i++) {
		bunkers[i] = 0;
		if (gamelevel > 5) { bunkers[i] = -1; }
	}
	gamestatus = "running";
}
//----------------------------------------------------------------------------
void showscore() {
	if (infoshow == 1 and saucers == -1) {
		if (lives > 1) { mp.display.drawBitmap(0, 0, playership[0], TFT_WHITE, 2); }
		if (lives > 2) { mp.display.drawBitmap(18, 0, playership[0], TFT_WHITE, 2); }
		mp.display.cursor_x= 42*2 - 4 * (score > 9) - 4 * (score > 99) - 4 * (score > 999);
		mp.display.cursor_y = 10;
		mp.display.print(score);
		mp.display.cursor_x = 144;
		mp.display.cursor_y = 10;
		mp.display.print(gamelevel + 1);
	}
}
//----------------------------------------------------------------------------
void nextlevelcheck() {
	// increment timer after all invaders killed
	if (invaderanz == 0) {
		yeahtimer++;
		if (yeahtimer >= 90) {
			gamelevel++;
			gamestatus = "newlevel";
		}
	}
}
//----------------------------------------------------------------------------
void handledeath() {
	deadcounter--;
	if (deadcounter == 0) {
		deadcounter = -1;
		lives--;
		shipx = 0;
		if (lives == 0) { gamestatus = "gameover"; }
	}
}


//ported specific
//----------------------------------------------------------------------------
void checkbuttons() {
	if (shipx < 0) shipx = 0;

	if (mp.buttons.repeat(BTN_LEFT, 1) && shipx > 0 && deadcounter == -1) { shipx-=2; }
	if (mp.buttons.repeat(BTN_RIGHT, 1) and shipx < 143 and deadcounter == -1) { shipx+=2; }

	if (mp.buttons.pressed(BTN_A) and shotx == -1 and deadcounter == -1) {
		shotx = shipx + 6;
		shoty = 106;
		// shoot->setADSR(10,10,15,20);
		shoot->note(70, 0.05);
		// shootSound->play();
		for (uint8_t i = 0; i < 8; i++)
		{
			mp.leds[i] = CRGB(255,0,0);
			pixelsTimer=millis();
			pixelsState=1;  
	 	}
	}
  if(millis()-pixelsTimer >= 50 && pixelsState==1){
    FastLED.clear();
  }
}
//----------------------------------------------------------------------------
void drawplayership() {
	if (deadcounter == -1) {
		mp.display.drawBitmap(shipx, 110, playership[0], TFT_WHITE, 2);
	}
	else {
		mp.display.drawBitmap(shipx, 110, playership[1 + invadershotframe], TFT_YELLOW, 2);
		handledeath();
	}
}
//----------------------------------------------------------------------------
void drawplayershot() {
	if (shotx != -1) {
		shoty = shoty - 4;
		mp.display.drawLine(shotx, shoty, shotx, shoty + 6, TFT_YELLOW);
    mp.display.drawLine(shotx+1, shoty, shotx+1, shoty + 6, TFT_YELLOW);
		if (shoty < 0) {
			shotx = -1;
			shoty = -1;
		}
	}
}
//----------------------------------------------------------------------------
void invaderlogic() {
	// increment invader counter
	if (invaderanz > 0) {
		checkdir = 0;
		do {
			invaderctr++;
			if (invaderctr > 39) {
				invaderctr = 0;
				checkdir = 1;
				invadersound = ++invadersound % 4;
				if (delayBip <= 0) {
					//gb.sound.fx(inv_move);
					if (invaderanz < 6) {
						delayBip = 5;
					}
					else if (invaderanz < 11) {
						delayBip = 3;
					} if (invaderanz < 21) {
						delayBip = 2;
					}
				}
				else { delayBip--; }
			}
		} while (invaders[invaderctr] == -1);

		// change direction?
		if (checkdir == 1) {
			if (nextydir != 0) {
				invaderxr = 0;
				invaderyr = 2;
			}
			else {
				invaderxr = nextxdir;
				invaderyr = 0;
			}
			checkdir = 0;
		}

		// change invader position
		invaderx[invaderctr] = invaderx[invaderctr] + invaderxr;
		invadery[invaderctr] = invadery[invaderctr] + invaderyr;

		// determine bunker removal if invaders are too low
		if (invadery[invaderctr] > 80) {
			for (int i = 0; i < 4; i++) {
				bunkers[i] = -1;
			}
		}

		// determine game over if invaders reach bottom
		if (invadery[invaderctr] > 100) {
			gamestatus = "gameover";
		}

		// determine screen border hit -> go down, then change direction
		if (invaderx[invaderctr] > 140 and invaderxr > 0) {
			nextxdir = -4;
			nextydir = 4;
		}
		if (invaderx[invaderctr] < 4 and invaderxr < 0) {
			nextxdir = 4;
			nextydir = 4;
		}
		if (invaderyr != 0) { nextydir = 0; }

		//change invader shape
		invaderframe[invaderctr] = ++invaderframe[invaderctr] % 2;

		// remove killed invader
		if (invaders[invaderctr] == 6) {
			invaders[invaderctr] = -1;
			invaderanz--;
		}

		// release invadershoot
		if (invadershottimer <= 0 and invadershots < gamelevel + 1 and invadershots < 4 and invadery[invaderctr] < 80) {
			invadershottimer = 40 - gamelevel * 10;
			invadershots++;
			int flag = 0;
			for (int u = 0; u < 4; u++) {
				if (flag == 0 and invadershotx[u] == -1) {
					invadershotx[u] = invaderx[invaderctr] + 2;
					invadershoty[u] = invadery[invaderctr];
					flag = 1;
				}
			}
		}
	}
}
//----------------------------------------------------------------------------
void drawinvaders() {
	infoshow = 1;
	for (int i = 0; i < 40; i++) {
		if (invaders[i] != -1) {
			if (invaders[i] == 0) mp.display.drawBitmap(invaderx[i], invadery[i], invader[invaders[i] + invaderframe[i]], TFT_ORANGE, 2);
			if (invaders[i] == 2) mp.display.drawBitmap(invaderx[i], invadery[i], invader[invaders[i] + invaderframe[i]], TFT_PINK, 2);
			if (invaders[i] == 4) mp.display.drawBitmap(invaderx[i], invadery[i], invader[invaders[i] + invaderframe[i]], TFT_BLUE, 2);
			if (invaders[i] == 6) mp.display.drawBitmap(invaderx[i], invadery[i], invader[invaders[i] + invaderframe[i]], TFT_YELLOW, 2);
			
			if (invadery[i] < 5) {
				infoshow = 0;
			}
		}

		// determine collission: invader & player shoot
		checkl = invaderx[i];
		checkr = invaderx[i] + 12;
		checkt = invadery[i];
		checkb = invadery[i] + 8;
		if (invaders[i] == 4) {
			checkl++;
			checkr--;
		}
		if (invaders[i] != -1 and invaders[i] != 6 and shotx >= checkl and shotx <= checkr and shoty + 2 >= checkt and shoty <= checkb) {
			score = score + invaders[i] * 10 + 10;
			invaders[i] = 6;
			shotx = -1;
			shoty = -1;
			destroyed->note(10, 0.05);
			// invaderDestroyed->play();
		}
	}
}
//----------------------------------------------------------------------------
void invadershot() {
	// handle invadershoot timer & framecounter
	invadershottimer--;
  
	invadershotframe = ++invadershotframe % 2;

	// move invadershots, draw & check collission
	for (int i = 0; i < 4; i++) {
		if (invadershotx[i] != -1) {
			invadershoty[i] = invadershoty[i] + 2;
			mp.display.drawBitmap(invadershotx[i], invadershoty[i], bomb[invadershotframe], TFT_RED, 2);

			// check collission: invadershot & bunker
			for (int u = 0; u < 4; u++) {
        
				checkl = 22 + u * 36;
        checkr = 22 + u * 36 + 14;
        checkt = 90;
        checkb = 100;
				if (bunkers[u] != -1 and invadershotx[i] + 1 >= checkl and invadershotx[i] <= checkr and invadershoty[i] + 3 >= checkt and invadershoty[i] <= checkb) {
					bunkers[u]++;
					if (bunkers[u] > 4) { bunkers[u] = -1; }
					invadershotx[i] = -1;
					invadershoty[i] = -1;
					invadershots--;
				}
			}

			// check collission: invadershot & player
			checkl = shipx;
			checkr = shipx + 12;
			checkt = 107;
			checkb = 110;
			if (deadcounter == -1 and invadershotx[i] + 1 >= checkl and invadershotx[i] <= checkr and invadershoty[i] + 3 >= checkt and invadershoty[i] <= checkb) {
				deadcounter = 60;
				destroyed->note(10, 0.05);
				// playerDestroyed->play();
			}

			//check collission: invadershot & playershoot
			checkl = invadershotx[i];
			checkr = invadershotx[i] + 1;
			checkt = invadershoty[i];
			checkb = invadershoty[i] + 3;
			if (shotx >= checkl and shotx <= checkr and shoty + 2 >= checkt and shoty <= checkb) {
				shotx = -1;
				shoty = -1;
				invadershotx[i] = -1;
				invadershoty[i] = -1;
				invadershots--;
			}

			// invadershoot on bottom off screen?
			if (invadershoty[i] > 128) {
				invadershotx[i] = -1;
				invadershoty[i] = -1;
				invadershots--;
			}
		}
	}
}
//----------------------------------------------------------------------------
void drawbunkers() {
	for (int i = 0; i < 4; i++) {
		checkl = 22 + i * 36;
		checkr = 22 + i * 36 + 14;
		checkt = 90;
		checkb = 100;
		if (bunkers[i] != -1 and shotx >= checkl and shotx <= checkr and shoty + 2 >= checkt and shoty <= checkb) {
			bunkers[i]++;
			shotx = -1;
			shoty = -1;
			if (bunkers[i] > 4) { bunkers[i] = -1; }
		}

		if (bunkers[i] != -1) {
			mp.display.drawBitmap(22 + i * 36, 90, bunker[bunkers[i]], TFT_GREEN, 2);
		}
	}
}
//----------------------------------------------------------------------------
void saucerappears() {
	saucertimer--;
	if (saucertimer <= 0) {
		saucertimer = 240;
		if (infoshow == 1 and saucers == -1) {
      // mainMusic->pause();
      // removeTrack(mainMusic);
      // addTrack(ufoSound);
      // ufoSound->setSpeed(2);
      // ufoSound->setRepeat(1);
      // ufoSound->play();
      
			saucers = 0;
			int i = random(2);
			if (i == 0) {
				saucerx = 0;
				saucerdir = 2;
			}
			else {
				saucerx = 146;
				saucerdir = -2;
			}
		}
	}
}
//----------------------------------------------------------------------------
void movesaucer() {
	if (saucers == 0) {
		saucerx = saucerx + saucerdir;
		if (saucerx <= 0 or saucerx >= 146) {
      // removeTrack(ufoSound);
      // mainMusic->resume();
      // addTrack(mainMusic);
			saucers = -1;
		}

		// check collission: player shot & saucer  
		checkl = saucerx;
		checkr = saucerx + 20;
		checkt = 0;
		checkb = 6;
		if (shotx >= checkl and shotx <= checkr and shoty + 2 >= checkt and shoty <= checkb) {
			score += 100;
			saucers = 1;
			shotx = -1;
			shoty = -1;
			saucerwait = 30;
      // removeTrack(ufoSound);
      // invaderDestroyed->play();
      // mainMusic->resume();
      // addTrack(mainMusic);
		}
	}
}
//----------------------------------------------------------------------------
void drawsaucer() {
	if (saucers != -1) {
		mp.display.drawBitmap(saucerx, 0, saucer[saucers], TFT_RED, 2);
		if (saucers == 1) {
			saucerwait--;
			if (saucerwait <= 0) {
        // removeTrack(ufoSound);
        // addTrack(mainMusic);
        // mainMusic->play();
        // mainMusic->setRepeat(1);
				saucers = -1;
			}
		}
	}
}

//----------------------------------------------------------------------------
void eraseData()
{
	unsigned long elapsedMillis = millis();
	bool blinkState = 1;
	while(1)
	{
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setTextFont(2);

		if (millis() - elapsedMillis >= multi_tap_threshold) {
			elapsedMillis = millis();
			blinkState = !blinkState;
		}

		mp.display.setTextColor(TFT_WHITE);
		mp.display.setCursor(4, 5);
		mp.display.printCenter("ARE YOU SURE?");
		mp.display.setCursor(4, 25);
		mp.display.printCenter("This cannot be reverted");

		if (blinkState){
			mp.display.drawRect((mp.display.width() - 60)/2, 102, 30*2, 9*2, TFT_RED);
			mp.display.setTextColor(TFT_RED);
			mp.display.setCursor(28*2, 103);
			mp.display.printCenter("DELETE");
		}
		else {
			mp.display.fillRect((mp.display.width() - 60)/2, 102, 30*2, 9*2, TFT_RED);
			mp.display.setTextColor(TFT_WHITE);
			mp.display.setCursor(28*2, 103);
			mp.display.printCenter("DELETE");
		}
		if (mp.buttons.released(BTN_B)) //BUTTON BACK
		{
			Serial.println("Go back");
      		mp.buttons.update();
			break;
		}
		if (mp.buttons.released(BTN_A)) // DELETE
		{
      		mp.buttons.update();
			JsonArray &empty = mp.jb.createArray();
			File file = SD.open(highscoresPath, "w");
			empty.prettyPrintTo(file);
			file.close();
			gamestatus = "title";
			break;
		}
		mp.update();
	}
	while(!mp.update());
}

void dataDisplay()
{
	// mp.homePopupEnable(0);
	// shootSound->stop();
	// titleMusic->stop();
	// invaderDestroyed->stop();
	// playerDestroyed->stop();
	// removeTrack(shootSound);
	// removeTrack(titleMusic);
	// removeTrack(invaderDestroyed);
	// removeTrack(playerDestroyed);
	mp.jb.clear();
	File file = SD.open(highscoresPath);
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
		mp.display.fillScreen(TFT_BLACK);
		mp.display.setCursor(32, -2);
		mp.display.setTextSize(1);
		mp.display.setTextFont(2);
		mp.display.setTextColor(TFT_RED);
		mp.display.printCenter("HIGHSCORES");
		mp.display.setCursor(3, 110);
		for (int i = 1; i < 6;i++)
		{
			mp.display.setCursor(24, i * 20);
			if(i <= hiscores.size())
				mp.display.printf("%d.   %.3s    %04d", i, nameArray[i], scoreArray[i]);
			else
				mp.display.printf("%d.    ---   ----", i);
		}
		mp.display.setCursor(2, 115);
		mp.display.print("Erase");
		if (mp.buttons.released(BTN_B) || mp.buttons.released(BTN_A))
		{
      mp.buttons.update();
			gamestatus = "title";
			break;
		}
		if (mp.buttons.released(BTN_FUN_LEFT))
		{
      mp.buttons.update();

			eraseData();
			break;
		}
		mp.update();
	}
	mp.homePopupEnable(1);
	// addTrack(titleMusic);
	// titleMusic->play();
	// titleMusic->setRepeat(1);
}
void showtitle() {
	cursor = 0;
	uint32_t blinkMillis = millis();
	bool blinkState = 0;
	while(!mp.buttons.released(BTN_A))
	{
		mp.display.fillScreen(TFT_BLACK);
		for(int i = 0; i < STAR_COUNT; i++)
		{
			// Remove the star from the screen by changing its pixel to the background color.
			mp.display.drawPixel(stars[i].x, stars[i].y, BACKGROUND_COLOR);

			// Update the position of the star.
			stars[i].update();

			// If the star's Y position is off the screen after the update.
			if(stars[i].y >= mp.display.height())
			{
				// Randomize its position.
				stars[i].randomize(0, mp.display.width(), 0, mp.display.height(), STAR_SPEED_MIN, STAR_SPEED_MAX);
				// Set its Y position back to the top.
				stars[i].y = 0;
			}

			// Draw the star at its new coordinate.
			mp.display.fillRect(stars[i].x, stars[i].y, 2, 2, STAR_COLOR);
		}
		if(millis() - blinkMillis >= 250)
		{
			blinkMillis = millis();
			blinkState = !blinkState;
		}
		mp.display.setTextColor(TFT_WHITE);
		mp.display.drawIcon(titleLogo, 7, 10, 73, 21, 2, TFT_WHITE);
		mp.display.setTextColor(TFT_RED);
		mp.display.setFreeFont(TT1);
		mp.display.setTextSize(2);
		mp.display.setCursor(10, 80);
		mp.display.printCenter("START");
		mp.display.setCursor(10, 100);
		mp.display.printCenter("HIGHSCORES");
		mp.display.setCursor(10, 120);
		mp.display.printCenter("QUIT");
		if(blinkState)
		{
			mp.display.drawRect(25, 67 + cursor * 20, 110, 16, TFT_RED);
			mp.display.drawRect(24, 66 + cursor * 20, 112, 18, TFT_RED);
		}
		if(mp.buttons.released(BTN_UP) && cursor > 0)
		{
			cursor--;
      mp.buttons.update();
			blinkMillis = millis();
			blinkState = 1;
		}
		if(mp.buttons.released(BTN_DOWN) && cursor < 2)
		{
			cursor++;
      mp.buttons.update();
			blinkMillis = millis();
			blinkState = 1;
		}		
		mp.update();
	}
	while(!mp.update());
	switch (cursor)
	{
		case 0:
			gamestatus = "newgame";
			break;
		case 1:
			dataDisplay();
			break;
		case 2:
			mp.loader();
			break;
	}
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
    
    mp.display.fillScreen(TFT_BLACK);
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
  }
  while(!mp.update());
}

/*
//----------------------------------------------------------------------------
// setup
//----------------------------------------------------------------------------
*/

void setup() {
	Serial.begin(115200);
	// shootSound = new MPTrack("/Invaderz/shoot.wav");
	// invaderDestroyed = new MPTrack("/Invaderz/invaderDestroyed.wav");
	// mainMusic = new MPTrack("/Invaderz/main.wav");
	// titleMusic = new MPTrack("/Invaderz/title.wav");
	// playerDestroyed = new MPTrack("/Invaderz/playerDestroyed.wav");
	// gameoverMusic = new MPTrack("/Invaderz/gameover.wav");
	// ufoSound = new MPTrack("/Invaderz/ufo.wav");
	mp.begin();
	mp.display.fillScreen(TFT_BLACK);
	starsSetup();
	gamestatus = "title";
	File file = SD.open(highscoresPath);
	JsonArray &hiscores = mp.jb.parseArray(file);
	file.close();
	if(!SD.exists("/Invaderz"))
		SD.mkdir("/Invaderz");
	if(hiscores.success())
		savePresent = 1;
	else
	{
		Serial.println("No save present");
		JsonArray &hiscores = mp.jb.createArray();
		JsonObject &test = mp.jb.createObject();
		File file = SD.open(highscoresPath, "w");
		hiscores.prettyPrintTo(file);
		file.close();
		mp.readFile(highscoresPath);
		// mp.saveJSONtoSAV(highscoresPath, hiscores);
	}
	hiscores.prettyPrintTo(Serial);
	shoot = new Oscillator(SQUARE);
	destroyed = new Oscillator(NOISE);
	shoot->setVolume(mp.oscillatorVolumeList[mp.mediaVolume]);
	destroyed->setVolume(mp.oscillatorVolumeList[mp.mediaVolume]/2);
	addOscillator(shoot);
	addOscillator(destroyed);

	// shoot->play();
  // shootSound->setVolume(256*mp.volume/14);
  // invaderDestroyed->setVolume(256*mp.volume/14);
  // mainMusic->setVolume(256*mp.volume/14);
  // titleMusic->setVolume(256*mp.volume/14);
  // playerDestroyed->setVolume(256*mp.volume/14);
  // gameoverMusic->setVolume(256*mp.volume/14);
  // ufoSound->setVolume(256*mp.volume/14);
  // gameoverMusic->setRepeat(1);
  // mainMusic->setRepeat(1);
  // addTrack(titleMusic);
  // titleMusic->setRepeat(1);
  // titleMusic->play();

	
}

//----------------------------------------------------------------------------    
// loop
//----------------------------------------------------------------------------    
void loop() {
    if (gamestatus == "title") {
		showtitle();
		if (mp.buttons.released(BTN_B))
			mp.loader();
    }
    if (gamestatus == "newgame") { newgame(); } // new game

	if (gamestatus == "newlevel") { newlevel(); } // new level

	if (gamestatus == "running") { // game running loop
    mp.display.fillScreen(TFT_BLACK);
    for(int i = 0; i < STAR_COUNT; i++)
    {
      // Remove the star from the screen by changing its pixel to the background color.
      mp.display.drawPixel(stars[i].x, stars[i].y, BACKGROUND_COLOR);

      // Update the position of the star.
      stars[i].update();

      // If the star's Y position is off the screen after the update.
      if(stars[i].y >= mp.display.height())
      {
        // Randomize its position.
        stars[i].randomize(0, mp.display.width(), 0, mp.display.height(), STAR_SPEED_MIN, STAR_SPEED_MAX);
        // Set its Y position back to the top.
        stars[i].y = 0;
      }

      // Draw the star at its new coordinate.
      mp.display.fillRect(stars[i].x, stars[i].y, 2, 2, STAR_COLOR);
    }
    if (mp.buttons.released(BTN_B))
    {
      // if(mainMusic->isPlaying())
      // mainMusic->pause();
      // if(ufoSound->isPlaying())
      // ufoSound->pause();
      gamestatus = "paused";
      mp.buttons.update();
    }
    showscore();
    checkbuttons(); // check buttons and move playership
    drawplayership(); // draw player ship
    drawplayershot(); // move + draw player shoot
    invaderlogic(); // invader logic
    drawinvaders(); // draw invaders
    invadershot(); // invaders shoot
    nextlevelcheck(); // next level?
    drawbunkers(); // draw bunkers & check collission with player shot
    saucerappears(); // saucer appears?
    movesaucer(); // move saucer
    drawsaucer(); // draw saucer & remove if hit
    showscore(); // show lives, score, level
		// end of: gamestatus = running
  }
	if (gamestatus == "gameover") { // game over
		mp.display.setTextColor(TFT_RED);
		mp.display.setTextSize(2);
		mp.display.setTextFont(1);
		mp.display.drawRect(14, 45, 134, 38, TFT_WHITE);
		mp.display.drawRect(13, 44, 136, 40, TFT_WHITE);
		mp.display.fillRect(15, 46, 132, 36, TFT_BLACK);
		mp.display.setCursor(47, 57);
		mp.display.printCenter("GAME OVER");
		// removeTrack(mainMusic);
		// addTrack(gameoverMusic);
		// gameoverMusic->play();
		if(mp.buttons.released(BTN_B) || mp.buttons.released(BTN_A))
		{
			// removeTrack(gameoverMusic);
			// removeTrack(shootSound);
			// removeTrack(invaderDestroyed);
			// removeTrack(playerDestroyed);
			// removeTrack(shootSound);
			// removeTrack(invaderDestroyed);
			File file;
			file = SD.open(highscoresPath);
			JsonArray &hiscores = mp.jb.parseArray(file);
			file.close();
			for (JsonObject& element : hiscores)
			{
				if(element["Rank"] == 1)
					tempScore = element["Score"].as<int>();
			}
			Serial.println("HERE");
			delay(5);
			
			enterInitials();
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
			// mp.saveJSONtoSAV(highscoresPath, hiscores);
			// addTrack(titleMusic);
			// titleMusic->play();
			// titleMusic->setRepeat(1);
			while(!mp.update());
				dataDisplay();	
			gamestatus="title";
		}
	}
	if(gamestatus == "paused")
	{
		while (!mp.buttons.released(BTN_A)) {
			mp.display.fillScreen(TFT_BLACK);
			mp.display.setTextColor(TFT_RED);
			mp.display.setCursor(0, mp.display.height()/2 - 18);
			mp.display.setTextFont(2);
			mp.display.setTextSize(2);
			mp.display.printCenter("Paused");
			mp.display.setCursor(4, 110);
			mp.display.setFreeFont(TT1);
			mp.display.printCenter("A: RESUME       B: QUIT");
			if (mp.buttons.released(BTN_B))
			{
				// removeTrack(mainMusic);
				// removeTrack(shootSound);
				// removeTrack(playerDestroyed);
				// removeTrack(invaderDestroyed);
				// addTrack(titleMusic);
				// titleMusic->play();
				// titleMusic->setRepeat(1);
				gamestatus = "title";
				mp.buttons.update();
				break;
			}
			mp.update();
			gamestatus = "running";
		}
		// mainMusic->resume();
		// ufoSound->resume();
  }
  mp.update();
}