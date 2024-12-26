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

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "AnimationFactory.h"
#include <iostream>

using namespace std;

class Sprite{
	public:
		Sprite(const char* filename, AnimationFactory* af);	// Load all sprites in directory
		virtual ~Sprite();
		SDL_Texture* frame();
		//void loadAnimation(const char *animName, bool loop);
		bool isAnimation(const char* name);
		const char* getAnimation();
		virtual void setAnimation(const char *name);
		bool animationFinished();
        virtual void blit(SDL_Renderer* screen);
		void kill();
		void x(int x);	// Set X
		void y(int y);
		int x();	// Get X
		int y();
		void gridx(int x);
		void gridy(int y);
		void move();
		int nextxpos;
		int nextypos;
		int direction; // 1 = up, 2 = down, 3 = left, 4 = right, 5 = stationary
        int speed;
		bool alive;
        int xpos;
		int ypos;

	protected:
		vector<Animation*> animations;
		Animation* current;
		AnimationFactory* af;
		const char* name;
		int gridxpos;
		int gridypos;
		int currentframe;
		int delay;
};

#endif
