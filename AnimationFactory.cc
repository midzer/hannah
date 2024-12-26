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

#include "AnimationFactory.h"
#include <iostream>

using namespace std;

AnimationFactory::AnimationFactory(SDL_Renderer* renderer){
    this->renderer = renderer;
};

AnimationFactory::~AnimationFactory(){
	//unsigned int i;
	//for(i=0; i<store.size(); i++){
	//	Animation* tmp = (Animation*)(store[i]);
		//cout << "Deleting Animation" << i << "\n";
	//	delete tmp;
	//}
    
    // Delete animations
    std::map<std::string, Animation*>::iterator an = store.begin();
    while(an != store.end()){
        Animation* anim = an->second;
        delete anim;
        an++;
    }
    
    // Delete scenery
    std::map<std::string, SDL_Texture*>::iterator it = scenery.begin();
    while(it != scenery.end()){
        SDL_Texture* tmp = it->second;
        SDL_DestroyTexture(tmp);
        it++;
    }
};

void AnimationFactory::list(){

};

int AnimationFactory::size(){
	return store.size();
};

Animation* AnimationFactory::getByName(std::string name){ 
    return (Animation*)store[name];
};

Animation* AnimationFactory::loadAnimation(const char* spriteName, const char* path, bool loop, const char* knownAs, bool invis){
    Animation* a = new Animation(renderer, spriteName, path, loop, knownAs, invis);	
	//store.push_back(a);
    store[a->knownAs] = a;
    return a;
};

void AnimationFactory::loadScenery(const char* sceneryName, const char* apath, bool trans){
    
    std::string path = Global::getPath(apath, "images");
   
    SDL_Surface* img = IMG_Load(path.c_str());
    
    SDL_Texture* tex = SDL_CreateTextureFromSurface(this->renderer, img);
    
    scenery[sceneryName] = tex;
    
    SDL_FreeSurface(img);
};

SDL_Texture* AnimationFactory::getScenery(const char* sceneryName){
    return scenery[sceneryName];
}