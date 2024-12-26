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

#ifndef _ANIMATION_H_
#define _ANIMATION_H_

//#define WIN32

#include "SDL.h"
#include "SDL_image.h"
//#include "SDL_gfxBlitFunc.h"
#include "Defines.h"
#include "Global.h"
#include <vector>

using namespace std;

class Animation{
	public:
		Animation(SDL_Renderer* renderer, const char* spritename, const char *filename, bool loop, const char* knownAs, bool invis = false);
		virtual ~Animation();
		void load(const char* spritename);
        bool loadInvis(const char* spritename, const char* filename);
        bool loadSheet(const char* spritename, const char* filename);
        SDL_Texture* getFrame();
		SDL_Texture* getFrame(int fnum);
		const char* getKnown();
		void reset();
		bool loop;
		bool finished;
		std::string name;
		std::string knownAs;
		int numframes;
		int currentframe;
		vector<SDL_Texture*> frames;
		int delay;
        SDL_Renderer* renderer;
};

#endif
