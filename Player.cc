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

#include "Player.h"

Player::Player(const char *name, AnimationFactory* af) : Sprite(name, af){
    this->direction = direction::STATIC;
    this->nextdir = direction::STATIC;
    this->effect = effecttype::NONE;
    this->bonusframenum = 0;
};

void Player::move(){
	if(xpos < nextxpos){
		xpos = xpos + speed;
	} else if(xpos > nextxpos){
		xpos = xpos - speed;
	} else if(ypos < nextypos){
		ypos = ypos + speed;
	} else if(ypos > nextypos){
		ypos = ypos - speed;
	}
};

void Player::respawn(){
	setAnimation("down");
    this->effect = effecttype::NONE;
    this->bonusframenum = 0;
};

void Player::init(int x, int y){
	this->nextxpos = 50 + (SIZET*x);
	this->nextypos = 50 + (SIZET*y);
	this->ypos = this->nextypos;
	this->xpos = this->nextxpos;
	this->direction = direction::STATIC; // 1, 2, 3, 4, 5 (u,d,l,r,stat)
	this->nextdir = direction::STATIC;
    this->speed = 6;
    this->effect = effecttype::NONE;
    this->bonusframenum = 0;
};

void Player::setEffect(effecttype effect){
    this->effect = effect;
    // Do other things, like changing the animations or whatever
};

void Player::blit(SDL_Renderer* screen){
    // For now...
    if(this->effect != effecttype::INVIS){
        Sprite::blit(screen);   
    } else {
        SDL_Rect dest;
        dest.x = this->x();
        dest.y = this->y();

        SDL_Texture* tmp = this->bonusframe();
        SDL_QueryTexture(tmp, NULL, NULL, &(dest.w), &(dest.h));
        
        //SDL_BlitSurface(tmp, NULL, screen, &dest);
        SDL_RenderCopy(screen, tmp, NULL, &dest);
    }
    
};

void Player::setAnimation(const char* name){
	alive = true;
	string s(name);
	string nn(this->name);
	s = nn + "-" + s;
    
    Animation* tmp = af->getByName(s);
    current = tmp;
    currentframe = 0;
    
    // Get current invisible animations frame...
    int pbframe = 0;
    if(bonus!=NULL){
        pbframe = bonusframenum % flashframes;
    }
    
    Animation* btmp = af->getByName(s+"-invis");
    if(btmp!=NULL){
        bonus = btmp;
        // We'll come back to this... 
        bonusframenum = pbframe;
    } else {
        bonusframenum = 0;
    }
    
};

SDL_Texture* Player::bonusframe(){
	
	SDL_Texture* tmp = bonus->getFrame(bonusframenum);
	delay--;
    
	if(delay==0 && bonusframenum < bonus->numframes){
		bonusframenum++;
		delay = delay_set;
	}
    
	if(bonusframenum>=bonus->numframes && bonus->loop){
		bonusframenum = 0;
	}
    
	return tmp;
};