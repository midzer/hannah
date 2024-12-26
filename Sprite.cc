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

#include "Sprite.h"
#include <string>

class Sprite;

Sprite::Sprite(const char* spritename, AnimationFactory* af){
	//printf(">-------Creating sprite\n");
	this->name = spritename;
	this->af = af;
	this->delay = delay_set;
	this->currentframe = 0;
	string s(spritename);
	s = s + "-default";
    string d(spritename);
    d = d + "-down";
    
    if(af->getByName(s.c_str())){
        animations.push_back(af->getByName(s.c_str()));    
    } else if(af->getByName(d.c_str())) {
        animations.push_back(af->getByName(d.c_str()));    
    }
    
	
	current = animations[animations.size()-1];
	alive = true;
};

Sprite::~Sprite(){
	//int i;
	//printf("<-------Killing sprite\n");
	//for(i=0; i<animations.size(); i++){
	//	delete(animations[i]);
	//}
};

bool Sprite::animationFinished(){
	if(current->numframes == currentframe && !current->loop){
		return true;
	} else {
		return false;
	}
};

void Sprite::blit(SDL_Renderer* screen){
    SDL_Rect dest;
	dest.x = this->x();
	dest.y = this->y();

    SDL_Texture* tmp = this->frame();
    SDL_QueryTexture(tmp, NULL, NULL, &(dest.w), &(dest.h));

	//SDL_BlitSurface(tmp, NULL, screen, &dest);
    SDL_RenderCopy(screen, tmp, NULL, &dest);
}

bool Sprite::isAnimation(const char* name){
	if(strcmp(name,current->name.c_str())==0){
		return true;
	} else {
		return false;
	}
};

const char* Sprite::getAnimation(){
	return current->name.c_str();
};

void Sprite::setAnimation(const char* name){
	alive = true;
	string s(name);
	string nn(this->name);
	s = nn + "-" + s;
    Animation* tmp = af->getByName(s);
    current = tmp;
    currentframe = 0;
};

void Sprite::kill(){
	setAnimation("die");
};

SDL_Texture* Sprite::frame(){
	SDL_Texture* tmp = current->getFrame(currentframe);
	delay--;
    
	if(delay==0 && currentframe < current->numframes){
		currentframe++;
		delay = delay_set;
	}
    
	if(currentframe>=current->numframes && current->loop){
		currentframe = 0;
	}
    
	return tmp;
};

void Sprite::move(){
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

void Sprite::gridx(int x){
	gridxpos = x;
};

void Sprite::gridy(int y){
	gridypos = y;
};

void Sprite::x(int x){
	xpos = x;
};

void Sprite::y(int y){
	ypos = y;
};

int Sprite::x(){
	return xpos;
};

int Sprite::y(){
	return ypos;
};
