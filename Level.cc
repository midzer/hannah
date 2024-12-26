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

#include "Level.h"
#include <iostream>

class Level;

Level::Level(){
	px = rx = bx = py = ry = by = pillsleft = 0;
    
};

void Level::load(string filename){
    std::string path = Global::getPath(filename.c_str(), "maps", "");
    
	pillsleft = 0;
	ifstream br(path.c_str());
	char buffer[SIZEX+1];
    
	// open file, read a line
	if(!br.is_open()){
		printf("Could not open map file %s!\n",path.c_str());
	} else {	
		int x = 0;
		int y = 0;

		// map needs to be read BEFORE passing to ghosts, or they
		// just get NULL and therefore have no way to hit a wall.
	
		for(y=0; y<SIZEY; y++){
			br.getline(buffer,SIZEX+1);
			for(x=0; x<SIZEX; x++){
                map[x][y].type = buffer[x];
				if(map[x][y].type == '.') pillsleft++;
				map[x][y].t = (y*SIZET) + 50;
				map[x][y].b = (y*SIZET) + 50 + 49;
				map[x][y].l = (x*SIZET) + 50;
				map[x][y].r = (x*SIZET) + 50 + 49;
                
                if(map[x][y].type == '.'){
                    gridpos pos = {x, y};
                    freecells.push_back(pos);
                }
			}
		}

        // After the level layout are lines for the ghosts and carrots

		char tempx[5]; char tempy[5]; char spd[5];
		int tempxi; int tempyi; // bit of geordi there :D
		int tempspdi;
	
		// Kill old ghosts and free mem
		// before creating new ones
			
		br.getline(buffer, SIZEX+1); // SIZEX+1 just to use all allocated space
        string pinkline(buffer);
		sscanf(pinkline.c_str(),"%*s %s %s",tempx,tempy);
		tempxi = atoi(tempx);
		tempyi = atoi(tempy);
		px = tempxi;
		py = tempyi;
        
		br.getline(buffer, SIZEX+1);
		string blueline(buffer);
		sscanf(blueline.c_str(),"%*s %s %s",tempx,tempy);
		tempxi = atoi(tempx);
		tempyi = atoi(tempy);
		bx = tempxi;
		by = tempyi;
	
		br.getline(buffer, SIZEX+1);
		string redline(buffer);
		sscanf(redline.c_str(),"%*s %s %s",tempx,tempy);
		tempxi = atoi(tempx);
		tempyi = atoi(tempy);
		rx = tempxi;
		ry = tempyi;

		// Read file til end. Each line is a food item
		while (br.getline(buffer, SIZEX+1)){
			string foodline(buffer);
			sscanf(foodline.c_str(),"%*s %s %s %s",tempx,tempy,spd);
			tempxi = atoi(tempx);
			tempyi = atoi(tempy);
			tempspdi = atoi(spd);
			foodinfo fd;
			fd.sx = tempxi;
			fd.sy = tempyi;
			fd.speed = tempspdi;
			foods.push_back(fd);
		}
		
		// kill any old ghosts before creating new ones
		br.close();
    }
};

gridpos Level::getFreeCell(){
    //select a random cell from the available free cells; 
    int freecell = rand() % freecells.size();
    return freecells[freecell];
};

Level::~Level(){
};
