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

#ifndef _ANIMATION_FACTORY_
#define _ANIMATION_FACTORY_

#include "Defines.h"
#include "Animation.h"
#include "Global.h"
#include <string>
#include <map>

class AnimationFactory{
	public:
		AnimationFactory(SDL_Renderer* renderer);
		virtual ~AnimationFactory();
		Animation* getByName(std::string name);
		//Animation* getByNumber(unsigned int n);
		Animation* loadAnimation(const char* spriteName, const char* path, bool loop, const char* knownAs, bool invis = false);
		void loadScenery(const char* sceneryName, const char* apath, bool trans);
        SDL_Texture* getScenery(const char* sceneryName);
        //Animation* getLast();
		int size();
		void list();
	private:
		std::map<std::string, Animation*> store;
        std::map<std::string, SDL_Texture*> scenery;
        SDL_Renderer* renderer;
};

#endif
