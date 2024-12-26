/* Copyright 2020 Stephen Branley
 * 
 * This file is part of Help Hannah's Horse.
 *
 * Help Hannah's Horse is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Help Hannah's Horse is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Help Hannah's Horse.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _DEFINES_
#define _DEFINES_

#include <vector>
#include <string>

#define TOP 50
#define BOT 550
#define LEFT 50
#define RIGHT 550

#define SIZEX 15 // Map is 15 tiles wide, by 15 down
#define SIZEY 15
#define SIZET 30 // Tile Size (they're square)

#define SPEED 15

#define SCREENW 800
#define SCREENH 600

#define delay_set 3

#define OFFSET 50

using namespace std;

#if SDL_BYTE_ORDER == SDL_BIG_ENDIAN
    const int rmask = 0xff000000;
    const int gmask = 0x00ff0000;
    const int bmask = 0x0000ff00;
    const int amask = 0x000000ff;
#else
    const int rmask = 0x000000ff;
    const int gmask = 0x0000ff00;
    const int bmask = 0x00ff0000;
    const int amask = 0xff000000;
#endif

enum effecttype {NONE,FREEZE,INVERT,SLOW,INVIS,ROBBED};
enum maingamestate {STORY=0, RUNNING=1, MENU=2, HOWTOPLAY=3, HISCORE=4, CREDITS=5, ENDGAME=6, TITLE=7}; 
enum direction {NORTH=1, SOUTH=2, WEST=3, EAST=4, STATIC=5};
enum keyaction {KA_UP, KA_DOWN, KA_LEFT, KA_RIGHT, KA_BUTTON, KA_ESCAPE, KA_SPACE, KA_NONE, KA_PAUSE};
enum gamedifficulty {EASY = 0, HARD = 1, INSANE = 2};

typedef struct{
	int sx,sy,speed;
} foodinfo;

typedef struct{
	char type;
	int l,r,t,b;
} gridcell;

typedef struct {
	int score = 0;
	std::string name = "";
    int difficulty; // 0 = easy, 1 = hard, 2 = insane
} scoreentry;

typedef struct{
    int x;
    int y;
} gridpos;

// Default to the easy values
typedef struct{
    // Max number of power pills
    int maxpills = 4;
    // Enemy speed
    int ghostspeed = 5;
    // Bonus speed
    int bonusspeed = 1;
    // Good bonuses do not evade Hannah
    bool dodgebonus = false;
    // Chance of a good bonus 
    int goodchance = 60; 
    gamedifficulty value = gamedifficulty::EASY;
    int lifeboundary = 5000;
} Difficulty;

typedef struct{
    maingamestate gamestate;
    int subgamestate;
    Difficulty difficulty;
} GameState;

const int flashframes = 10;

#endif



