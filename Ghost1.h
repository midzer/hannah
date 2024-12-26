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

#ifndef _GHOST1_
#define _GHOST1_
#include "Ghost.h"
//#include <fstream>
//#include <vector>

using namespace std;

class Ghost1 : public Ghost{
	public:
		Ghost1(const char* filename,int x, int y, int speed, AnimationFactory* af);	// Load all sprites in directory
//		virtual ~Ghost1();
		//SDL_Surface* frame();
		//void x(int x);	// Set X
		//void y(int y);
		//int getfreedirections();
		//bool testdirection(int dir);
		//int x();	// Get X
		//int y();
		void move(int x, int y);
		//int nextxpos;
		//int nextypos;
		//int direction; // 1 = up, 2 = right, 3 = down, 4 = left, 5 = stationary
		//int numframes;
		//int nextdir;

	protected:
		//vector<SDL_Surface*> frames;
		//int xpos;
		//int ypos;
		//int currentframe;
		//int speed;
		//gridcell map[SIZEX][SIZEY];
};

#endif
