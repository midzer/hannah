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

#include <iostream>
#include <cstdio>
#include <sstream>
#ifndef WIN32
    #include <argp.h>
#endif
#include "Player.h"
#include "Level.h"
#include "Ghost2.h"
#include "Ghost3.h"
#include "Food.h"
#include "Bonus.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "AnimationFactory.h"
#include "ScoreLib.h"
#include "Menu.h"

#ifndef WIN32
    #include <sys/stat.h>
    #include <sys/types.h>
#else
    #include <ctime>
    #include <direct.h>
#endif

#define pilltime 200

SDL_Window* screen = NULL;
SDL_Renderer* renderer = NULL;

// Now for some global sprites - better done as classes but Oh Well!
SDL_Texture* current_hedge = NULL;
SDL_Texture* current_grass = NULL;
SDL_Texture* tex = NULL;

TTF_Font* subgamefont = NULL;

Mix_Chunk* eatpill = NULL;
Mix_Music* bgm1 = NULL;
Mix_Chunk* eatmagic = NULL;
Mix_Chunk* eatfood = NULL;
Mix_Chunk* scream = NULL;
Mix_Chunk* chomp = NULL;
Mix_Chunk* yeah = NULL;
Mix_Chunk* uhoh = NULL;
Mix_Chunk* woohoo = NULL;
Mix_Chunk* clunk = NULL;

AnimationFactory* af = NULL;

Player* hannah;
Ghost* red = NULL;
Ghost* pink = NULL;
Ghost* blue = NULL;
Sprite* coin = NULL;

vector<Ghost*> ghosties;

unsigned int level = 1;

// Should be part of the Player object
int lives = 4;
int score = 0;
int numlives = 4; // number of lives to start each game with

bool fullscreen = false;
bool music = true;
bool nojoy = false;

int pillsleft = 0;
bool ted = false; // Invincibility -ted

bool Global::local = false;
std::vector<scoreentry> ScoreLib::hiscore;
std::map<int, TTF_Font*> Global::fonts;
Uint32 Global::HannahEventType;

effecttype effect = NONE;
int bonustimer = 0;

// Should be local?
Uint32 lastticks;
Uint32 delta_tick;
Uint32 tick;

Level lev;

Menu* menu;

// Should be part of Level
vector<Food*> levelfood;

using namespace std;

vector<Level> maps;

Bonus* the_bonus; // Limit to one bonus at a time

void incScore(int delta, GameState* game){
    int oldScore = score;
    score = score + delta;
    for(int i=1; i <= 10; i++){
        int checkScore = i * game->difficulty.lifeboundary;
        if((oldScore < checkScore) && (score >= checkScore)){
            lives = lives + 1;
            Mix_PlayChannel(-1, woohoo, 0);
        }
    }
}


void setTimer(int time, effecttype e){
	effect = e;
	bonustimer = time;
	if(e==INVIS){
        hannah->setEffect(effecttype::INVIS);
	} else if(hannah->effect == effecttype::INVIS){
        hannah->setEffect(effecttype::NONE);
	}
};

void timer(){
	if(bonustimer>0){
		bonustimer--;
	} else {
		effect = effecttype::NONE;
		if(hannah->effect == effecttype::INVIS){
            hannah->setEffect(effecttype::NONE);
        }
	}
};

bool checkDirection(int dir){
	// get pacman's actual x y grid position
	int ax = (hannah->xpos / SIZET) - 1;
	int ay = (hannah->ypos / SIZET) - 1;

    switch(dir){
        case 1: ay--; break;
        case 2: ay++; break;
        case 3: ax--; break;
        case 4: ax++; break;
        case 5: return false;
    }

    char obst = lev.map[ax][ay].type;
    
    return (obst == '#') || (obst == '$') || (obst == '~') || ((obst == '-') && (dir != 1));

};

void PlayMusic(bool play){
    if(play){
        if(!Mix_PlayingMusic()){
            Mix_PlayMusic(bgm1,-1);
        }
    } else {
        if(Mix_PlayingMusic()){
            Mix_HaltMusic();
        }
    }
}

void ToggleMusic(){
    if(Mix_PlayingMusic()){
        Mix_HaltMusic();
    } else {
        Mix_PlayMusic(bgm1,-1);
    }
}

void createBonus(bool cheat = false){
	if(the_bonus==NULL && effect==NONE){
        if(cheat){
            gridpos pos = lev.getFreeCell();
            the_bonus = new Bonus("robber",pos.x,pos.y,1,af,'r');
            the_bonus->setLevel(lev, pos.x,pos.y);
        }
		if((SDL_GetTicks() % 200 == 0) && (rand() % 2 == 0)){
            gridpos pos = lev.getFreeCell();
                        
            int r = (rand() % 4) + 1;
            switch(r){
                case 1: 
                    the_bonus = new Bonus("freeze",pos.x,pos.y,1,af,'f'); 
                    break;
                case 2: 
                    the_bonus = new Bonus("invis",pos.x,pos.y,1,af,'i'); 
                    break;
                case 3: 
                    the_bonus = new Bonus("invert",pos.x,pos.y,1,af,'s'); 
                    break;
                case 4: 
                    the_bonus = new Bonus("robber",pos.x, pos.y, 2, af, 'r'); 
                    break;
            };
            the_bonus->setLevel(lev,pos.x,pos.y);
		}
	}
};

void moveBonus(){
	if(the_bonus!=NULL){
        the_bonus->move(hannah->xpos,hannah->ypos);
        Global::blit(renderer, the_bonus->frame(), the_bonus->x(), the_bonus->y());
        the_bonus->decLife();
        if(the_bonus->lifeforce == 0){
            delete the_bonus;
            the_bonus = NULL;
        }
	}
};

void check_bonus_pacman(){
	if(the_bonus!=NULL){
			if((the_bonus->x() < hannah->xpos+30) && (the_bonus->x()+30 > hannah->xpos) && (the_bonus->y()+30 > hannah->ypos) && (the_bonus->y() < hannah->ypos+30)){
				// Pacman gets pill
				switch(the_bonus->type){
					case 'f':	setTimer(200,FREEZE); Mix_PlayChannel(-1,yeah,0); break;
					case 's':	setTimer(200,INVERT); Mix_PlayChannel(-1,uhoh,0); break;
					case 'i':	setTimer(200,INVIS); Mix_PlayChannel(-1,yeah,0); break;
                    case 'r':   setTimer(200,ROBBED); Mix_PlayChannel(-1,uhoh,0); break;
				};
				delete the_bonus;
				the_bonus = NULL;
			}
	}
};

void drawMap(GameState* game){
	int mapx = 0;
	int mapy = 0;
    SDL_Texture* coinf = coin->frame();
	for(mapy = 0; mapy < SIZEY; mapy++){
        for(mapx = 0; mapx < SIZEX; mapx++){
            switch(lev.map[mapx][mapy].type){
                case '#': 
                    Global::blit(renderer, current_hedge,(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET); 
                    break;
                case '.': 
                    Global::blit(renderer, current_grass,(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET); 
                    // The extra game loop in entering a high score makes this go mad,
                    // so let's just not draw it.
                    if(game->subgamestate != 3){
                        Global::blit(renderer, coinf, (mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET);
                    }
                    break;
                case '_': 
                    Global::blit(renderer, current_grass,(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET); 
                    break;
                // Power pills can now be limited by difficulty
                case '1' ... '4': 
                    Global::blit(renderer, current_grass,(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET);
                    {
                        int intval = (int)(lev.map[mapx][mapy].type) - 48;
                        if(intval <= game->difficulty.maxpills){
                            Global::blit(renderer, af->getScenery("powerpill"),(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET); 
                        } else {
                            Global::blit(renderer, coinf, (mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET);
                            lev.map[mapx][mapy].type = '.';
                            pillsleft++;
                        }
                    }
                    break;
                case '-':	
                    Global::blit(renderer, current_grass,(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET);
                    Global::blit(renderer, af->getScenery("gate"),(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET); 
                    break;
                case '~':	
                    Global::blit(renderer, current_grass,(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET); 
                    Global::blit(renderer, af->getScenery("oneway"),(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET); 
                    break;
                case 'x':	
                    Global::blit(renderer, current_grass,(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET);
                    Global::blit(renderer, af->getScenery("cross"),(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET); 
                    break;
                case '$':	
                    Global::blit(renderer, current_grass,(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET);
                    Global::blit(renderer, af->getScenery("red_door"),(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET); 
                    break;
                case 'r':	
                    Global::blit(renderer, current_grass,(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET);
                    Global::blit(renderer, af->getScenery("red_key"),(mapx*SIZET)+OFFSET,(mapy*SIZET)+OFFSET); 
                    break;
            };
        }
	}
    
    int hs = 25600;
    if(ScoreLib::hiscore.size() > 0){
        hs = ScoreLib::hiscore[0].score;
    } 
    
    // I'll leave this here as an example of how it used to be done
    int size = snprintf(NULL, 0, "Top %d", hs);
    char* str = (char*)calloc(size, sizeof(char) + 1);
    sprintf(str, "Top %d", hs);
    
    Global::drawTextAt(renderer, str, 101, 11, 40, {0,0,0}, true);
    Global::drawTextAt(renderer, str, 100, 10, 40, {255,255,255}, true);
    free(str);
    
};

void level_init_food(Level levl){
	// While there are more foods in lev.foods create add to vector of food
	lev.foods.clear();
	unsigned int i = 0;
	for(i=0; i<levl.foods.size(); i++){
		foodinfo tmpfi = (foodinfo)levl.foods[i];
		Food *tmpfood = new Food("carrot",tmpfi.sx,tmpfi.sy,tmpfi.speed,af);
		tmpfood->setLevel(levl, tmpfi.sx, tmpfi.sy);
		levelfood.push_back(tmpfood);
		pillsleft++;
	}
};

//---------- All of this should be in Level.cc

void level_move_food(){
	unsigned	int i = 0;
	for(i=0; i<levelfood.size(); i++){
		Food *tmpfood = (Food*)levelfood[i];
		tmpfood->move(hannah->xpos,hannah->ypos);
	}
};

void level_free_food(){
	unsigned int i = 0;

	for(i=0; i<levelfood.size(); i++){
		Food* tmp = (Food*)levelfood[i];
		delete tmp;
	}
	levelfood.clear();
};

void level_draw_food(){
	unsigned int i = 0;
	for(i=0; i<levelfood.size(); i++){
		Food* tmp = (Food*)levelfood[i];
		if(tmp->alive == true){
			Global::blit(renderer, tmp->frame(),tmp->x(),tmp->y());
		}
	}
};

// ------------------------------

Ghost* check_baddies_pacman(){
	// Check pacman for collision with ghosts
	// Is it possible to enumerate through ghosts?
	// Encapsulate ghosts in vector to make iteration easier
	// Return true if collision

	if(effect!=INVIS){
		Ghost* tmp = NULL;	
		unsigned int i=0;
		for(i=0; i<ghosties.size(); i++){
			tmp = (Ghost*)ghosties[i];

			if((tmp->x() < hannah->xpos+24) && (tmp->x()+30 > hannah->xpos+6) && (tmp->y()+30 > hannah->ypos+6) && (tmp->y() < hannah->ypos+24) && (!ted)){
				return tmp;
			} 
		}
	}
	return NULL;
};

void drawbaddies(){
	Global::blit(renderer, red->frame(),red->x(),red->y());
	Global::blit(renderer, pink->frame(),pink->x(),pink->y());
	Global::blit(renderer, blue->frame(),blue->x(),blue->y());
	level_draw_food();
};

void readMap(string filename){
	Level levl;	
	
    levl.load(filename);
    
    maps.push_back(levl);
};

void move(){
		
	int ax = (hannah->xpos / SIZET) - 1;
	int ay = (hannah->ypos / SIZET) - 1;

    bool nextDirectionCollision = checkDirection(hannah->nextdir);
    bool currentDirectionCollision = checkDirection(hannah->direction);

	if(!nextDirectionCollision && hannah->xpos==hannah->nextxpos && hannah->ypos==hannah->nextypos){
        // Next direction might be the same as current direction, but whatever it is, we've reached our target
        // position so we need to pick the new target position.
		
        // OK, but what if the current mode is invis?
        // Need to set that direction invis instead
        if(hannah->direction != hannah->nextdir){
            switch(hannah->nextdir){
                case 1: hannah->setAnimation("up"); break;
                case 2: hannah->setAnimation("down"); break;
                case 3: hannah->setAnimation("left"); break;
                case 4: hannah->setAnimation("right"); break;
            }
        }
        
        hannah->direction = hannah->nextdir;
        switch(hannah->nextdir){
                case 1:    ay--; break;
                case 2:    ay++; break;
                case 3:    ax--; break;
                case 4:    ax++; break;
        };
        hannah->nextxpos = lev.map[ax][ay].l;
        hannah->nextypos = lev.map[ax][ay].t;
	} else if(nextDirectionCollision && hannah->xpos==hannah->nextxpos && hannah->ypos==hannah->nextypos){
		// Don't like this. If the next direction is the current direction and there's
        // a collision, pick a new direction, maybe? How does pacman do it...?
        if(hannah->nextdir==hannah->direction){
			hannah->direction = direction::STATIC;
			hannah->nextdir = direction::STATIC;
		} else if(hannah->nextdir!=hannah->direction && currentDirectionCollision){
			hannah->direction = direction::STATIC;
			hannah->nextdir = direction::STATIC;
		} else { // nextdir != direction but no collision in the current direction, keep going!
			switch(hannah->direction){
				case 1: hannah->nextypos = lev.map[ax][ay-1].t; break;
				case 2: hannah->nextypos = lev.map[ax][ay+1].t; break;
				case 3: hannah->nextxpos = lev.map[ax-1][ay].l; break;
				case 4: hannah->nextxpos = lev.map[ax+1][ay].l; break;
            };
		}
	}

	if(hannah->nextxpos!=hannah->xpos){
		switch(hannah->direction){
			case 3: hannah->xpos = hannah->xpos - hannah->speed; break;
			case 4: hannah->xpos = hannah->xpos + hannah->speed; break;
		};
	} else if(hannah->nextypos!=hannah->ypos){
		switch(hannah->direction){
			case 1: hannah->ypos = hannah->ypos - hannah->speed; break;
			case 2: hannah->ypos = hannah->ypos + hannah->speed; break;
		};
	}		

};


void drawscore(){
    int life;
    if(lives == 0){
        life = 0;
    } else {
        life = lives-1;
    }
        
    std::ostringstream os;
    
    os.str("");
    os << score;
    Global::drawTextAt(renderer, os.str(), 595, 255, 36);
    
    os.str("");
	os << "Level " << level; 
    Global::drawTextAt(renderer, os.str(), 595, 345, 36);

    os.str("");
    os << "Lives " << life;
    Global::drawTextAt(renderer, os.str(), 595, 435, 36);
};


void setHiScore(int entryAt, GameState* game){

	// One array for each initial
	char first[][27] = {
        {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',' '},
        {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',' '},
        {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',' '}
    };
	int counter = 0;
	
    int c[] = {0, 0, 0};
    
    int current = 0;
	bool done = false;
	char tt[9] = "aaa     ";
	int indx = 300;
	int indy = 350;
	
    keyaction ka = KA_NONE;
    
    SDL_Event e;
	// Why not have this outside the function call, and have the function return a result when it's done?
	// So do... draw backgrounds, etc, while(setHiScore() != 1)... would have to set the variables above outside the loop 
	while(!done){
		while(SDL_PollEvent (&e)){
            ka = KA_NONE;
            // Convert the SDL event into my custom
            // event type so that I only have to handle
            // one thing in the switch/case below
            if (e.type == SDL_KEYDOWN){
                if(e.key.keysym.sym == SDLK_DOWN){
                    ka = KA_DOWN;
                } else if(e.key.keysym.sym == SDLK_UP){
                    ka = KA_UP;
                } else if(e.key.keysym.sym == SDLK_LEFT){
                    ka = KA_LEFT;
                } else if(e.key.keysym.sym == SDLK_RIGHT){
                    ka = KA_RIGHT;
                } else if(e.key.keysym.sym == SDLK_SPACE){
                    ka = KA_SPACE;
                } else if(e.key.keysym.sym == SDLK_RETURN){
                    ka = KA_SPACE;
                }
            } else if (e.type == SDL_JOYAXISMOTION){
                if(e.jaxis.axis == 0){
                    // X-Axis
                    if(e.jaxis.value > 8000){
                        ka = KA_RIGHT;
                    } else if (e.jaxis.value < -8000){
                        ka = KA_LEFT;
                    }
                } else {
                    // Y-Axis
                    if(e.jaxis.value > 8000){
                        ka = KA_DOWN;
                    } else if (e.jaxis.value < -8000){
                        ka = KA_UP;
                    }
                }
            } else if (e.type == SDL_JOYBUTTONDOWN){
                ka = KA_SPACE;
            }
            
            switch(ka){
                case KA_SPACE:
                    if(current == 2){
                        done = true; 
                        ScoreLib::hiscore[entryAt].name = tt;
                        ScoreLib::hiscore[entryAt].difficulty = game->difficulty.value;
                    } else {
                        current++;
                        counter = c[current];
                    }
                    break;
                case KA_RIGHT:
                    current++;
                    if(current == 3) current = 0;
                    // Make the new initial equal to where the counter left off last time
                    counter = c[current];
                break;
                case KA_LEFT:
                    current--;
                    if(current == -1) current = 2;
                    counter = c[current];
                break;
                case KA_UP:
                    counter++;
                    if(counter>26) counter = 0;
                    c[current] = counter;
                break;
                case KA_DOWN:
                    counter--;
                    if(counter<0) counter = 26;
                    c[current] = counter;
                break;
                default: break;
            };
            switch(current){
                case 0: indx = 300; break;
                case 1: indx = 375; break;
                case 2: indx = 450; break;
            };
            sprintf(tt,"%c%c%c",first[0][c[0]],first[1][c[1]],first[2][c[2]]);
		}
    
        // If we don't draw all the background stuff it throws a total
        // fit in fullscreen mode
        Global::blit(renderer, af->getScenery("bg"),0, 0);
        Global::blit(renderer, af->getScenery("bg2"),550, 0);
        drawMap(game);
        drawscore();

		Global::blit(renderer, af->getScenery("border"),190,100);
		Global::drawTextAt(renderer, "Enter your initials",250,150,24, {0,0,0}, true);
		Global::drawTextAt(renderer, "And press <Enter>",250,180,24, {0,0,0}, true);
        Global::drawTextAt(renderer, tt,300,250,108);
		Global::blit(renderer, af->getScenery("powerpill"),indx,indy);
        SDL_RenderPresent(renderer);
    }
};

void drawCredits(){
	Global::drawTextAt(renderer,"Help Hannah's Horse",210,120,18, {0,0,0}, true);
	Global::drawTextAt(renderer,"(c)2006 - 2020 Steve Branley",210,170,18, {0,0,0}, true);
	Global::drawTextAt(renderer,"Graphics:    Steve Branley",210,220,18);
	Global::drawTextAt(renderer,"Programming: Steve Branley",210,240,18);
	Global::drawTextAt(renderer,"Music:       Steve Branley",210,260,18);
    
    Global::drawTextAt(renderer,"If you enjoy playing this game,",210,300,18);
    Global::drawTextAt(renderer,"please consider buying me a coffee:",210,320,18);
    //Global::drawTextAt(renderer,"donation towards it. Go to: ", 210, 340,20);
    //Global::drawTextAt(renderer,"at",210,360,20);
    Global::drawTextAt(renderer,"https://buymeacoffee.com/retrojunkies",210,380,16);
    
	Global::drawTextAt(renderer,"This game is released under",210,450,18);
	Global::drawTextAt(renderer,"the terms of the GPL v3",210,470,18);
};

void drawsubgamestate(int subgamestate){
	// 1 = "Level 1 - Get Ready!", 2 = "Get Ready!", 3 = "Game Over!"
	// Display "Level 1" / "Game Over!" / "Get Ready!"
	char tt[100];
	switch(subgamestate){
		case 1: sprintf(tt, "Level %d - Get Ready!", level); break;
		case 2: sprintf(tt, "Get Ready!"); break;
		case 3: sprintf(tt, "Game Over!"); break;
	};

    SDL_Rect r;
    SDL_Rect rin;
    
    if(subgamestate == 1){
        r = {100, 220, 350, 50};
        rin = {110, 225, 330, 40};
    } else {
        r = {140, 220, 270, 50};
        rin = {150, 225, 250, 40};
    }
    
    SDL_SetRenderDrawColor(renderer, 0x53, 0x9c, 0x9d, 0x00);
    SDL_RenderFillRect(renderer, &r);
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0x00);
    SDL_RenderFillRect(renderer, &rin);
    
    Global::drawTextAt(renderer, tt, 0, 232, 24, {0, 0, 0}, false, true);
    	
};

void initLevel(int levelnum, GameState* game){
	if(levelnum < 4){
		current_grass = af->getScenery("grass");
		current_hedge = af->getScenery("hedge");
	} else if(levelnum < 7){
		current_grass = af->getScenery("grass2");
		current_hedge = af->getScenery("hedge2");
	} else if(levelnum < 10){
		current_grass = af->getScenery("grass3");
		current_hedge = af->getScenery("hedge3");
	} else if(levelnum < 13){
		current_grass = af->getScenery("grass4");
		current_hedge = af->getScenery("hedge4");
	} else {
		current_grass = af->getScenery("grass");
		current_hedge = af->getScenery("hedge");
	}
	// Remove any existing ghosts 
	// before creating new ones for new level
	ghosties.clear();
	if(the_bonus!=NULL){
		delete the_bonus;
		the_bonus = NULL;
	}
	
	game->subgamestate = 1;
	setTimer(0,NONE);

	lev = (Level)maps.at(levelnum-1);

	red->setLevel(lev, lev.rx, lev.ry);
	red->x(lev.rx);
	red->y(lev.ry);
	red->setAnimation("default");
	red->timer(0);

	pink->setLevel(lev, lev.px, lev.py);
	pink->x(lev.px);
	pink->y(lev.py);
	pink->setAnimation("default");
	pink->timer(0);

	blue->setLevel(lev, lev.bx, lev.by);
	blue->x(lev.bx);
	blue->y(lev.by);
	blue->setAnimation("default");
	blue->timer(0);

	pillsleft = lev.pillsleft;
	level_init_food(lev);

	ghosties.push_back(red);
	ghosties.push_back(pink);
	ghosties.push_back(blue);

    hannah->init(7,7);
};

bool check_food_pacman(GameState* game){
	// Check pacman for collision with ghosts
	// Is it possible to enumerate through ghosts?
	// Encapsulate ghosts in vector to make iteration easier
	// Return true if collision
	
	unsigned int i=0;
	for(i=0; i<levelfood.size(); i++){
		Food* tmp = (Food*)levelfood[i];
		if((tmp->x() < hannah->xpos+30) && (tmp->x()+30 > hannah->xpos) && (tmp->y()+30 > hannah->ypos) && (tmp->y() < hannah->ypos+30) && tmp->alive==true){
			Mix_PlayChannel(-1,eatfood,0);
			tmp->alive = false;
			incScore(50, game);
			pillsleft--;
			if(pillsleft==0){
				// set subgamestate to "Well Done!"
				// set level to level +1
				level++;
				game->subgamestate = 1;
				initLevel(level, game);
				
                hannah->init(7,7);
			}
			return true;
		}
	}
	return false;
};

void reInitLevel(GameState* game){
	// put ghosts and pacman back to start
	// after dying
	red->x(lev.rx);
	red->y(lev.ry);
	red->setRespawn(0);
    
	blue->x(lev.bx);
	blue->y(lev.by);
	blue->setRespawn(0);
    
	pink->x(lev.px);
	pink->y(lev.py);
	pink->setRespawn(0);

	red->setAnimation("default");
	pink->setAnimation("default");
	blue->setAnimation("default");
	
    red->timer(0);
	blue->timer(0);
	pink->timer(0);

	// Write "Get Ready!"	
	Global::blit(renderer, af->getScenery("bg2"),550,0);
    
	// Put Hannah back in the middle
	hannah->init(7,7);

	setTimer(0,NONE);

	drawMap(game);
	move();
    
    drawbaddies();
		
	drawsubgamestate(2);
    if(lives > 0){
        //blit(hannah->frame(),hannah->xpos,hannah->ypos);
        hannah->blit(renderer);
        hannah->respawn();
    }
	
    if(the_bonus!=NULL){
		delete the_bonus;
		the_bonus = NULL;
	}

    //SDL_RenderPresent(renderer);
};

void openRedDoor(){
	int ax = 0;
	int ay = 0;
	for(ax=0; ax<SIZEX; ax++){
		for(ay=0; ay<SIZEY; ay++){
			if(lev.map[ax][ay].type=='$'){
				lev.map[ax][ay].type='_';
                Mix_PlayChannel(-1, clunk, 0);
            }
		}
	}
};


void endGame(GameState* game){
	int bx = 150;
	int by = -500;
	int wx = 150;
	int wy = 650;
	bool bend = false;
	bool wend = false;
	bool end = false;
	SDL_Event e;
	// While banner and text are not in the right places
	while(!bend || !wend){
		if(by<100){
			by = by + 5;
		} else {
			bend = true;
		}
		if(wy>100){
			wy = wy - 5;
		} else {
			wend = true;
		}
		Global::blit(renderer, af->getScenery("gradient"),0,0);
		Global::blit(renderer, af->getScenery("banner"),bx,by);
		Global::blit(renderer, af->getScenery("words"),wx,wy);
		
        SDL_RenderPresent(renderer);
	}
	while(!end){
		while(SDL_PollEvent(&e)){
			if(e.key.state == SDL_PRESSED) end = true;	
		}
	}
	game->subgamestate = 3;
};


void checkpillcollision(GameState* game){
	int ax = (hannah->xpos / SIZET) - 1;
	int ay = (hannah->ypos / SIZET) - 1;

	if(lev.map[ax][ay].type=='.' && effect != effecttype::ROBBED){
		lev.map[ax][ay].type='_';
		incScore(10, game);
		Mix_PlayChannel(-1,eatpill,0);
		pillsleft--;
		// check for end of level
		if(pillsleft==0){
			// set subgamestate to "Well Done!"
			level++;
			level_free_food();
			if(level>maps.size()){
				// Game is over, get highscore
				endGame(game);
			} else {
				game->subgamestate = 1;
				// Free the foods from the last level
				initLevel(level, game);
				
                hannah->init(7,7);
			}
		}
	} else if(lev.map[ax][ay].type >= '1' &&  lev.map[ax][ay].type <= '4'){
		lev.map[ax][ay].type='_';
		incScore(100, game);
		Mix_PlayChannel(-1,eatmagic,0);

		red->timer(pilltime);
		blue->timer(pilltime);
		pink->timer(pilltime);

		red->setAnimation("flash");
		blue->setAnimation("flash");
		pink->setAnimation("flash");
	} else if(lev.map[ax][ay].type=='r'){
		lev.map[ax][ay].type='_';
		openRedDoor();
	} else if(lev.map[ax][ay].type=='_' && effect==effecttype::ROBBED){
        lev.map[ax][ay].type='.';
        pillsleft++;
    }

};

int loadSprites(){        
    af->loadScenery("hedge", "hedge.png", false);
    af->loadScenery("hedge2", "hedge2.png", false);
    af->loadScenery("hedge3", "hedge3.png", false);
    af->loadScenery("hedge4", "hedge4.png", false);

    af->loadScenery("grass", "grass.png", false);
    af->loadScenery("grass2", "grass2.png", false);
    af->loadScenery("grass3", "grass3.png", false);
    af->loadScenery("grass4", "grass4.png", false);
   
    af->loadScenery("pill", "pill.png", true);
    af->loadScenery("story", "story.png", true);
    af->loadScenery("powerpill", "powerpill.png", true);
    af->loadScenery("gate", "gate.png", true);
    
    af->loadScenery("bg", "bg_left.png", false);
    af->loadScenery("bg2", "bg_right2_1.png", false);
    
    af->loadScenery("oneway", "oneway.png", true);
    af->loadScenery("howto", "howtoplay.png", true);
    af->loadScenery("border", "border.png", true);
    af->loadScenery("cross", "cross.png", true);
    af->loadScenery("red_door", "red_door.png", false);
    af->loadScenery("red_key", "red_key.png", true);
    
    af->loadScenery("gradient", "gradient.png", true);
    af->loadScenery("banner", "end_banner.png", true);
    af->loadScenery("words", "congrats.png", true);
    
    af->loadScenery("torch", "torch.png", true);
    
    af->loadAnimation("coin", "default", true, "coin-default");
    coin = new Sprite("coin", af);
    coin->setAnimation("default");

    return 0;
};

void loadsounds(){
    eatpill = Mix_LoadWAV(Global::getPath("DripC.wav", "sounds").c_str());
    eatmagic = Mix_LoadWAV(Global::getPath("gulp.wav", "sounds").c_str());
    eatfood = Mix_LoadWAV(Global::getPath("crunch.wav", "sounds").c_str());
    scream = Mix_LoadWAV(Global::getPath("scream.wav", "sounds").c_str());
    chomp = Mix_LoadWAV(Global::getPath("chomp.wav", "sounds").c_str());
    uhoh = Mix_LoadWAV(Global::getPath("uh-oh.wav", "sounds").c_str());
    yeah = Mix_LoadWAV(Global::getPath("yeah.wav", "sounds").c_str());
    woohoo = Mix_LoadWAV(Global::getPath("woohoo.wav", "sounds").c_str());
    clunk = Mix_LoadWAV(Global::getPath("keyclunk.wav", "sounds").c_str());
};

void cleanUp(){
	TTF_CloseFont(subgamefont);
	
	Mix_HaltChannel(-1);
	Mix_FreeChunk(eatpill);
	Mix_FreeMusic(bgm1);
	Mix_FreeChunk(eatmagic);
	Mix_FreeChunk(eatfood);
	Mix_FreeChunk(scream);
	Mix_FreeChunk(chomp);
	Mix_FreeChunk(yeah);
	Mix_FreeChunk(uhoh);
    Mix_FreeChunk(woohoo);
    Mix_FreeChunk(clunk);

	if(the_bonus!=NULL){
        delete the_bonus; 
        the_bonus = NULL;
    }
    
    // Loop through and release any created fonts
    std::map<int, TTF_Font*>::iterator an = Global::fonts.begin();
    while(an != Global::fonts.end()){
        TTF_Font* fon = an->second;
        TTF_CloseFont(fon);
        an++;
    }
    
	
	delete(hannah);
	delete(red);
	delete(pink);
	delete(blue);
	delete(af);
    delete(coin);

    delete(menu);

	Mix_CloseAudio();
	TTF_Quit();
    
	SDL_Quit();
};

void initGame(){

	bgm1 = Mix_LoadMUS(Global::getPath("hannahtheme.ogg").c_str());

	if(!bgm1) printf("Mix_LoadMUS(\"bgm1.mp3\"): %s\n", Mix_GetError());

	subgamefont=TTF_OpenFont(Global::getPath("subgamefont.ttf").c_str(), 24);
    
	if(!subgamefont) printf("Unable to load font! %s \n", TTF_GetError());

    readMap("map1.dat");
    readMap("map2.dat");
    readMap("map3.dat");
    readMap("map4.dat");
    readMap("map5.dat");
    readMap("map6.dat");
    readMap("map7.dat");
    readMap("map8.dat");
    readMap("map9.dat");
    readMap("map10.dat");
    readMap("map11.dat");
    readMap("map12.dat");
    readMap("map13.dat");
    readMap("map14.dat");
    readMap("map15.dat");

	// Load extra anims for sprites
	af->loadAnimation("hannah","die",false,"hannah-die");
	af->loadAnimation("hannah","down",true,"hannah-down");
    af->loadAnimation("hannah","down",true,"hannah-down", true);
	
    af->loadAnimation("hannah", "up", true, "hannah-up");
    af->loadAnimation("hannah", "up", true, "hannah-up", true);
    af->loadAnimation("hannah", "left", true, "hannah-left");
    af->loadAnimation("hannah", "left", true, "hannah-left", true);
    af->loadAnimation("hannah", "right", true, "hannah-right");
    af->loadAnimation("hannah", "right", true, "hannah-right", true);

	af->loadAnimation("red","default",true,"red-default");
	af->loadAnimation("pink","default",true,"pink-default");
	af->loadAnimation("blue","default",true,"blue-default");

	af->loadAnimation("red","flash",true,"red-flash");
	af->loadAnimation("pink","flash",true,"pink-flash");
	af->loadAnimation("blue","flash",true,"blue-flash");

	af->loadAnimation("carrot","default",true,"carrot-default");

	af->loadAnimation("freeze","default",true,"freeze-default");
	af->loadAnimation("invis","default",true,"invis-default");
	af->loadAnimation("invert","default",true,"invert-default");
    af->loadAnimation("robber", "down", true, "robber-down");

	red = new Ghost1("red",0,0,5,af);
	pink = new Ghost2("pink",0,0,5,af);
	blue = new Ghost3("blue",0,0,5,af);

	hannah = new Player("hannah",af);
};

void createMenu(){
    menu = new Menu();
    MenuRow* menu1 = new MenuRow();
	menu1->AddItem("Play", 1);
	menu->AddRow(menu1);
	MenuRow* menu2 = new MenuRow();
	menu2->AddItem("How To Play", 2);
	menu->AddRow(menu2);
	MenuRow* menu3 = new MenuRow();
	menu3->AddItem("High Scores", 3);
	menu->AddRow(menu3);
	MenuRow* menu4 = new MenuRow();
	menu4->AddItem("Difficulty: Easy", 4);
	menu4->AddItem("Difficulty: Hard", 5);
	menu4->AddItem("Difficulty: Insane", 9);
	menu->AddRow(menu4);
    MenuRow* menu5 = new MenuRow();
    menu5->AddItem("Music On", 6);
    menu5->AddItem("Music Off", 7);
    menu->AddRow(menu5);
    MenuRow* menu6 = new MenuRow();
    menu6->AddItem("Credits/Donate", 8);
    menu->AddRow(menu6);
    MenuRow* menu7 = new MenuRow();
    menu7->AddItem("Quit", 0);
    menu->AddRow(menu7);
};

void doFlashlight(SDL_Texture* tex, GameState* game){
    if(game->difficulty.value != gamedifficulty::INSANE){
        return;
    }
        
    SDL_Rect dest = {50,50,450,450};
    
    // Destination rectangle must take into account hannah's position
    // Hannah's position, minus the offset of the maze, plus half her size, minus the radius of the circle
    int adj = (OFFSET + 60) - 15;
    
    int x = hannah->xpos - adj;
    int y = hannah->ypos - adj;
    
    SDL_Rect dest2 = {x,y,120,120};
    
    // Paint a black surface
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, tex);
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xed);
    SDL_RenderClear(renderer);
    
    SDL_SetTextureBlendMode(af->getScenery("torch"), SDL_BLENDMODE_NONE);
    SDL_SetTextureAlphaMod(af->getScenery("torch"), 0xed);
    SDL_RenderCopy(renderer, af->getScenery("torch"), NULL, &dest2);
    
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, tex, NULL, &dest);
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void mainGameLoop(GameState* game){
    // Draw bg and map, all bits need this					
    Global::blit(renderer, af->getScenery("bg"), 0,0);
    Global::blit(renderer, af->getScenery("bg2"),550,0);
    
    drawMap(game);
    drawscore();
    // Game is running
    if(game->subgamestate==0){
        timer();
        checkpillcollision(game);
        move();
        drawbaddies();
        createBonus();

        if(effect!=FREEZE){
            red->move(hannah->xpos, hannah->ypos);
            pink->move(hannah->xpos,hannah->ypos);
            blue->move(hannah->xpos,hannah->ypos);
        }

        level_move_food();
        moveBonus();
        check_bonus_pacman();
                                
        Ghost* tmpghost = check_baddies_pacman();
        if((tmpghost!=NULL) && (tmpghost->timer()==0) && (hannah->effect != effecttype::INVIS) && (!hannah->isAnimation("die"))){
            Mix_PlayChannel(-1,scream,0);
            hannah->setAnimation("die");
            hannah->direction = direction::STATIC;
        } else if(tmpghost!=NULL && tmpghost->timer()>0){
            // Powerpill is active, eat ghost
            tmpghost->timer(0);
            tmpghost->setAnimation("default");
            tmpghost->setRespawn(50);
            tmpghost->x(tmpghost->startx);
            tmpghost->y(tmpghost->starty);
            Mix_PlayChannel(-1,chomp,0);
            incScore(400, game);
        }

        unsigned int gn = 0;
        for(gn=0; gn<ghosties.size(); gn++){
            tmpghost = (Ghost*)ghosties[gn];
            if(tmpghost->timer() == 0 && tmpghost->isAnimation("flash")){
                tmpghost->setAnimation("default");
            }
        }

        if(hannah->isAnimation("die") && hannah->animationFinished()){
            lives--;
            if(lives == 0){
                level_free_food();
                game->subgamestate = 3;
            } else {
                reInitLevel(game);
                game->subgamestate = 2;
            }
                                     
        } else {

            if(lives > 0)
                hannah->blit(renderer);
            check_food_pacman(game);
        }

        doFlashlight(tex, game);

        SDL_RenderPresent(renderer);		

    // Game is saying "Level N - Get ready"
    } else if(game->subgamestate==1 || game->subgamestate==2){
        drawbaddies();
        drawscore();
        
        doFlashlight(tex, game);
        
        drawsubgamestate(game->subgamestate);
        if(lives > 0){
            hannah->blit(renderer);
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(2000);
        game->subgamestate = 0;
    // Game is saying "Game Over"
    } else if (game->subgamestate==3){
        drawscore();
        
        doFlashlight(tex, game);
        
        drawsubgamestate(game->subgamestate);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(2000);

        // check for hiscore
        int pos = ScoreLib::checkHiScores(score);
        if(pos > -1){
            Global::blit(renderer, af->getScenery("bg"), 0, 0);
            Global::blit(renderer, af->getScenery("bg2"), 550, 0);
            drawMap(game);
            drawscore();
            Global::blit(renderer, af->getScenery("border"),190,100);
            setHiScore(pos, game);
            SDL_RenderPresent(renderer);
            game->gamestate = maingamestate::STORY;
        }

        game->gamestate = maingamestate::STORY;

        lives = numlives;
        score = 0;
        level = 1;
    } else if(game->subgamestate==4){
        //pause
        drawbaddies();
        drawscore();
        
        doFlashlight(tex, game);
        
        
        SDL_RenderPresent(renderer);
    }
}

SDL_Window* init(bool fullscreen){

    Uint32 initflags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
    
    if(!nojoy){
        initflags = initflags | SDL_INIT_JOYSTICK;
    }
    
	SDL_Init(initflags);
    
    Uint32 flags = SDL_WINDOW_OPENGL;
    
    if(fullscreen){
        flags = flags | SDL_WINDOW_FULLSCREEN;
    }
	
    screen = SDL_CreateWindow("Help Hannah's Horse", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, flags);

    renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if(fullscreen){
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
    }
    
    SDL_RenderSetLogicalSize(renderer, 800, 600);

	if( !screen ) {
		
		fprintf(stderr, "Couldn't create a surface: %s\n", SDL_GetError());
		return NULL;

	} else {
		if(TTF_Init()==-1) {
			printf("TTF_Init: %s\n", TTF_GetError());
			return NULL;
		}

		if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
			printf("Mix_OpenAudio: %s\n", Mix_GetError());
			return NULL;
		}

		return screen;
	}
};

#ifndef WIN32
// Required by argp for parsing command line arguments
static int parse_opt (int key, char *arg, struct argp_state *state){
    switch (key){
        // Invincible
        case 'i': 
            ted = true;
        break; 
        // Fullscreen
        case 'f':
            fullscreen = true;
        break;
        // No music
        case 'n':
            music = false;
        break;
        // No joystick
        case 'j':
            nojoy = true;
        break;
        // Starting map
        case 'm':
            level = atoi(arg);
        break;
    }; 
    
    return 0; 
}
#endif

int main(int argc, char **argv){

    level = 1;
    
    // Because Windows is a steaming pile of shite, I don't
    // have the benefit of the lovely argp options parsing stuff
    #ifndef WIN32
    
        struct argp_option options[] = { 
            { "fullscreen", 'f', 0, 0, "Run game full screen"}, 
            { "nomusic", 'n', 0, 0, "Do not play music (still plays sounds)"},
            { "nojoy", 'j', 0, 0, "Disable joystick support"},
            { "invincible", 'i', 0, OPTION_HIDDEN, "Invincibility"},
            { "map", 'm', "NUM", OPTION_HIDDEN, "Start on level NUM"},
            { 0 } 
        };
        
        struct argp argp = { options, parse_opt };
        
        // Could use argp to parse the parameters
        argp_parse (&argp, argc, argv, 0, 0, 0);    
    
        // if installed file is not present, run locally
        std::string testfile = DATA_INSTALL_DIR;
        testfile.append("/resources/font.ttf");
        
        ifstream f(testfile.c_str());
        Global::local = !f.good();
    #else
        const char* opt[][2] = {
            {"f", "Run game full screen"}, 
            {"n", "Do not play music (still plays sounds)"},
            {"j", "Disable joystick support"},
            {"?", "Show this message"}
        };
        
        for(int i=0; i < argc; i++){
            if(strcmp(argv[i], "/f") == 0){
                fullscreen = true;
            }
            if(strcmp(argv[i], "/n") == 0){
                music = false;
            }
            if(strcmp(argv[i], "/j") == 0){
                nojoy = true;
            }
            if(strcmp(argv[i], "/?") == 0){
				ostringstream os;
				
                os << "Usage: hhh.exe [OPTION...]" << endl;
				os << endl;
                for(int j=0; j < (sizeof(opt)/sizeof(*opt)); j++){
                    os << "/" << opt[j][0] << "  " << opt[j][1] << endl;
                }
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Help Hannah's Horse", os.str().c_str(), NULL);				
                exit(1);
            }
        }
        
    #endif
  	
	screen = init(fullscreen);

    af = new AnimationFactory(renderer);

	score = 0;
    loadSprites();
    createMenu();
    initGame();
    
	srand(time(0));
	
	if(screen!=NULL){
		// Screen was assigned so carry on
		
        GameState game;
        
        SDL_Joystick* joy = NULL;
        
        if((!nojoy) && (SDL_NumJoysticks() > 0)){
            joy = SDL_JoystickOpen(0);
        }
        
		SDL_Event e;
        
		bool exit = false;

        tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 450, 450);

		Global::blit(renderer, af->getScenery("bg"),0,0);
		Global::blit(renderer, af->getScenery("bg2"),550,0);
		
		game.subgamestate = 1; // used for displaying "Get Ready" etc at start of level

		loadsounds();
		
        // Register custom event for menu presses
        Global::HannahEventType = SDL_RegisterEvents(1);
        
        ScoreLib::loadHiScores();
		
        if(music == true){
            Mix_PlayMusic(bgm1, -1);
        }
            
		game.gamestate = maingamestate::TITLE; 

		while(!exit){
            
			// Some speed control
			tick = SDL_GetTicks();
			delta_tick = tick - lastticks;
			lastticks = tick;

			//limit to 40 frames per second (40*25ms = 1 second)
			if(delta_tick < 40){
				SDL_Delay(40 - delta_tick);
				tick = SDL_GetTicks();
				delta_tick = delta_tick + tick - lastticks;
				lastticks = tick;
			}

            while(SDL_PollEvent(&e)){
                // Convert keyboard and joystick events to a single action
                // for handling in the main loop (there's probably a better way...)
                keyaction ka = KA_NONE;
                
                if(e.type == SDL_QUIT){
                    ScoreLib::saveHiScores();
                    exit = 1; 
                } else if(e.type == Global::HannahEventType){
                    // These events are pushed into the queue
                    // by the menu selections
                    if(e.user.code == 0){
                        ScoreLib::saveHiScores();
                        exit = 1;
                    } else if (e.user.code == 1){
                        lives = numlives;
                        score = 0;
                        hannah->setAnimation("down");
                        game.gamestate = maingamestate::RUNNING;
                        Global::blit(renderer, af->getScenery("bg"),0,0);
                        initLevel(level, &game);
                    } else if (e.user.code == 6){
                        PlayMusic(true);
                    } else if (e.user.code == 7){
                        PlayMusic(false);
                    } else if (e.user.code == 2){
                        // How to play
                        game.gamestate=maingamestate::HOWTOPLAY;
                    } else if (e.user.code == 3){
                        // High scores
                        game.gamestate=maingamestate::HISCORE;
                    } else if (e.user.code == 8){
                        game.gamestate=maingamestate::CREDITS;
                    } else if (e.user.code == 4){
                        // Easy difficulty
                        game.difficulty.maxpills = 4;
                        game.difficulty.ghostspeed = 4;
                        game.difficulty.value = gamedifficulty::EASY;
                        game.difficulty.lifeboundary = 5000;
                        red->speed = 5;
                        pink->speed = 5;
                        blue->speed = 5;
                    } else if (e.user.code == 5){
                        // Hard difficulty
                        game.difficulty.maxpills = 2;
                        game.difficulty.ghostspeed = 5;
                        game.difficulty.value = gamedifficulty::HARD;
                        game.difficulty.lifeboundary = 7500;
                        red->speed = 5;
                        pink->speed = 5;
                        blue->speed = 6;
                    } else if (e.user.code == 9){
                        game.difficulty.maxpills = 0;
                        game.difficulty.ghostspeed = 6;
                        game.difficulty.value = gamedifficulty::INSANE;
                        game.difficulty.lifeboundary = 10000;
                        red->speed = 6;
                        pink->speed = 6;
                        blue->speed = 6;
                    } 
                } else if(e.type == SDL_KEYDOWN){               
                    switch(e.key.state){
                        case SDL_PRESSED:
                            if(e.key.keysym.sym==SDLK_p){
                                ka = KA_PAUSE;
                            } else if(e.key.keysym.sym == SDLK_UP){
                                ka = KA_UP;
                            } else if(e.key.keysym.sym == SDLK_DOWN){
                                ka = KA_DOWN;
                            } else if(e.key.keysym.sym == SDLK_LEFT){
                                ka = KA_LEFT;
                            } else if(e.key.keysym.sym == SDLK_RIGHT){
                                ka = KA_RIGHT;
                            } else if(e.key.keysym.sym == SDLK_ESCAPE){
                                ka = KA_ESCAPE;
                            } else {
                                ka = KA_SPACE;
                            }
                        break;
                    };
                } else if(e.type == SDL_JOYAXISMOTION){
                    if(e.jaxis.axis == 0){
                        // X-Axis
                        if(e.jaxis.value > 8000){
                            ka = KA_RIGHT;
                        } else if (e.jaxis.value < -8000){
                            ka = KA_LEFT;
                        }
                    } else {
                        // Y-Axis
                        if(e.jaxis.value > 8000){
                            ka = KA_DOWN;
                        } else if (e.jaxis.value < -8000){
                            ka = KA_UP;
                        }
                    }
                } else if(e.type == SDL_JOYBUTTONDOWN){
                    ka = KA_SPACE;
                }
                         
                // Check our current game state and respond accordingly.
                switch(game.gamestate){
                    case maingamestate::STORY:
                        if(ka != KA_NONE){
                            game.gamestate = maingamestate::MENU;
                        }
                    break;
                    case maingamestate::RUNNING: 
						if(ka == KA_RIGHT){	
							if(effect == INVERT){
								hannah->nextdir = 3; 
							} else { 
								hannah->nextdir = 4;		
							}
						}
						if (ka == KA_LEFT){
							if (effect == INVERT){
								hannah->nextdir = 4; 
							} else { 
								hannah->nextdir = 3;
							}		
						}
						if (ka == KA_UP){
							hannah->nextdir = 1;
						}
						if (ka == KA_DOWN){
							hannah->nextdir = 2;
						}
						if (ka == KA_PAUSE){
							if(game.subgamestate==0){
								game.subgamestate=4;
							} else if(game.subgamestate==4){
								game.subgamestate=0;
							}
						}
						if (ka == KA_ESCAPE){
							level_free_food();
							game.gamestate=maingamestate::STORY;
							level = 1;
						}
                    break;
                    case maingamestate::HOWTOPLAY: 
                        if((ka == KA_SPACE) || (ka == KA_ESCAPE)){
							game.gamestate=maingamestate::MENU;
							level = 1;
                        }
                    break;
                    case maingamestate::HISCORE: 
						if((ka == KA_SPACE) || (ka == KA_ESCAPE)){
							game.gamestate=maingamestate::MENU;
							level = 1;
						}
                    break;
                    case maingamestate::CREDITS: 
						if((ka == KA_SPACE) || (ka == KA_ESCAPE)){
							game.gamestate=maingamestate::MENU;
							level = 1;
							
						}
                    break;
                    case maingamestate::ENDGAME: break;
                    case maingamestate::TITLE: break;
                    case maingamestate::MENU: 
						if(ka == KA_UP){
							menu->MovePrevious();
						} else if(ka == KA_DOWN){
							menu->MoveNext();
						} else if(ka == KA_LEFT){
							menu->MoveLeft();
						} else if(ka == KA_RIGHT){
							menu->MoveRight();
						} else if(/*e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE*/ka == KA_SPACE){
							menu->Select();
						} else if(ka == KA_ESCAPE){
							ScoreLib::saveHiScores();
							exit = 1;
						} else if(ka != KA_NONE){
							game.gamestate = maingamestate::STORY;    
						}
                    break;
                };
                
                // Don't allow the player to move if Hannah is dying
                if(hannah->isAnimation("die")){
                    hannah->nextdir = 5;
                }
                                    
            }

            // game logic
            if(game.gamestate == maingamestate::STORY){
                Global::blit(renderer, af->getScenery("story"),0,0);
                
                const int storyleft = 220;
                
                // Draw story text - one day I'll translate this!
                Global::drawTextAt(renderer, "Hannah's horse, Harry, has", storyleft, 120, 20, {0, 0, 0}, false);
                Global::drawTextAt(renderer, "been stolen! Collect the coins", storyleft, 145, 20, {0, 0, 0}, false);
                Global::drawTextAt(renderer, "to pay the ransom and", storyleft, 170, 20, {0, 0, 0}, false);
                Global::drawTextAt(renderer, "help to save Harry! But watch", storyleft, 195, 20, {0, 0, 0}, false);
                Global::drawTextAt(renderer, "out... some nasty ghosts ", storyleft, 220, 20, {0, 0, 0}, false);
                Global::drawTextAt(renderer, "will do all they can to stop", storyleft, 245, 20, {0, 0, 0}, false);
                Global::drawTextAt(renderer, "you!", storyleft, 270, 20, {0, 0, 0}, false);
                Global::drawTextAt(renderer, "Press any key", 0, 330, 24, {0, 0, 0}, true);
                Global::drawTextAt(renderer, "https://www.retrojunkies.co.uk/hannah", 345, 580, 18, {255,255,255}, true);
                SDL_RenderPresent(renderer);
            } else if(game.gamestate == maingamestate::MENU){
                Global::blit(renderer, af->getScenery("story"),0,0);
                menu->Draw(renderer);
                Global::drawTextAt(renderer, "https://www.retrojunkies.co.uk/hannah", 345, 580, 18, {255,255,255}, true);
                SDL_RenderPresent(renderer);
            } else if(game.gamestate == maingamestate::RUNNING){
                mainGameLoop(&game); 
            } else if(game.gamestate == maingamestate::HOWTOPLAY){
                Global::blit(renderer, af->getScenery("howto"),190,100);
                SDL_RenderPresent(renderer);
            } else if(game.gamestate == maingamestate::HISCORE){
                Global::blit(renderer, af->getScenery("border"),190,100);
                ScoreLib::drawHiScore(renderer, af);
                SDL_RenderPresent(renderer);
            } else if(game.gamestate == maingamestate::CREDITS){
                Global::blit(renderer, af->getScenery("border"),190,100);
                drawCredits();
                SDL_RenderPresent(renderer);
            } else if(game.gamestate == maingamestate::ENDGAME){
                endGame(&game);
                game.gamestate = maingamestate::STORY;
            } else if(game.gamestate == maingamestate::TITLE){
                SDL_Texture *t = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREENW, SCREENH);
                SDL_SetRenderTarget(renderer, t);
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
                SDL_RenderFillRect(renderer, NULL);
                SDL_SetRenderTarget(renderer, NULL);
                SDL_RenderCopy(renderer, t, NULL, NULL);
                SDL_RenderPresent(renderer);
                
                Global::drawTextAt(renderer, "Fat Horse Games", 295, 230, 18, {255,255,255}, true);
                Global::drawTextAt(renderer, "Presents", 345, 260, 18, {255,255,255}, true);
                SDL_RenderPresent(renderer);
                
                SDL_Delay(2000);
                game.gamestate = maingamestate::STORY;
            }
		} // End SDL main loop
	
		// End of loop reached - cleanup
        if(joy != NULL){
            if(SDL_JoystickGetAttached(joy)){
                SDL_JoystickClose(joy);
            }
        }
        SDL_DestroyTexture(tex);
		cleanUp();

	} else {
		// Quit - do any cleanup
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(screen);
		SDL_Quit();
	}
    
    return 0;
};

