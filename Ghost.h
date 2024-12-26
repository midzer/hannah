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
 
#ifndef _GHOST_
#define _GHOST_

#include "SDL.h"
#include "SDL_image.h"
#include "Level.h"
#include "Sprite.h"
#include <fstream>
#include <vector>

using namespace std;

class Ghost : public Sprite{
	public:
		Ghost(const char* filename, int x, int y, int speed, AnimationFactory* af);	// Load all sprites in directory
		virtual ~Ghost();
		void x(int x);	// Set X
		void y(int y);
		void setLevel(Level lev, int startx, int starty);
		int getfreedirections();
		bool testdirection(int dir);
		void setRespawn(long time);
		void decTimer();
		int x();	// Get X
		int y();
		virtual void move(int x, int y) = 0; // Pure virtual
		long timer();
		void timer(long timeleft);
		int nextxpos;
		int nextypos;
		int direction; // 1 = up, 2 = right, 3 = down, 4 = left, 5 = stationary
		//int numframes;
		int nextdir;
		long timeleft;
		char effect; // Used for timeleft, effects are 'flash', 'stop', 'fast', 'slow' etc...
		long respawntime;
		int startx;
		int starty;
		Level lev;
};

#endif
