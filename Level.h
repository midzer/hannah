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

#ifndef _Level_H_
#define _Level_H_

#include "Defines.h"
#include "Global.h"
#include <fstream>

using namespace std;

class Level{
	public:
		Level();
		virtual ~Level();
        void load(string filename);
		gridcell map[SIZEX][SIZEY]; // Map data
		int px,py;		// Starting position, pink ghost
		int rx,ry;		// Red ghost
		int bx,by;		// Blue ghost
		int pillsleft;		// Pills on level
		vector<foodinfo> foods; // Food on level
        gridpos getFreeCell();
    private:
        vector<gridpos> freecells;
};

#endif
