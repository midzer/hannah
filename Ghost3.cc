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

#include "Ghost3.h"
#include "time.h"
#include "math.h"
#include <string>

class Ghost3;

Ghost3::Ghost3(const char* filename, int x, int y, int speed, AnimationFactory* af) : Ghost(filename,x,y,speed, af){
	
	this->speed = speed;
	this->startx = x;
	this->starty = y;

    direction = 2;
    xpos = 50 + (x*SIZET);
    ypos = 50 + (y*SIZET);
    nextypos = ypos;
    nextxpos = xpos;
    // annoyingly must currently be a divisor of 30
    speed = 5;
    
    int n; int j;
    for(j=0;j<SIZEY;j++){
        for(n=0; n<SIZEX;n++){
            this->lev.map[n][j] = lev.map[n][j];
        }
    }
};

// x & y are pacman's current grid positions
void Ghost3::move(int x, int y){
	if(timeleft>0) timeleft--; else effect = ' ';
	if(respawntime>0){
		respawntime--;
	} else {
	// pacman's X / Y
	int pacx = (x / SIZET) - 1;
	int pacy = (y / SIZET) - 1;
	// ghost's X / Y
	int ax = (xpos / SIZET) - 1;
	int ay = (ypos / SIZET) - 1;

	bool infront = false; // pac is above
	bool left = false; // pac is left

	// determine which direction pacman is in
	switch(direction){
		case 1: if(pacy < ay){
							infront = true;
						} else {
							infront = false;
						}
						if(pacx < ax){
							left = true;
						} else {
							left = false;
						} break;
		case 2: if(pacy < ay){
							left = true;
						} else {
							left = false;
						}
						if(pacx < ax){
							infront = false;
						} else {
							infront = true;
						} break;
		case 3:	if(pacy < ay){
							infront = false;
						} else {
							infront = true;
						}
						if(pacx < ax){
							left = false;
						} else {
							left = true;
						} break;
		case 4: if(pacy < ay){
							left = false;
						} else {
							left = true;
						}
						if(pacx < ax){
							infront = true;
						} else {
							infront = false;
						} break;
	};

    // Run away if Hannah can eat us!
    if(this->isAnimation("flash")){
        left = !left;
        infront = !infront;
    }
    
	// need to ensure it keeps going forwards unless interrupted

	if(nextxpos == xpos && nextypos == ypos){
		// actual position matches wanted position, so 
		// pacman is in square, so move
		int lft = 0;
		int rit = 0;
		int back = 0;
		lft = direction - 1;		
		if(lft == 0) lft = 4;
		rit = direction + 1;
		if(rit == 5) rit = 1;
		back = direction - 2;
		if(back < 1) back = (0 - back) + (2 * direction);

		int newdirection = 0; // test left right and set as newdirection
		// temporarily. Then test if straight on. If straight on is ok,
		// take 50-50 chance between straight on and a turn.

		//if(testdirection(direction)==false){
			// cannot go forwards, try left and right...
			if(testdirection(lft) && !testdirection(rit)){
				// left is ok, right is blocked
				newdirection = lft;
			} else if(!testdirection(lft) && testdirection(rit)){
				// right is ok, left is blocked
				newdirection = rit;
			} else if(!testdirection(lft) && !testdirection(rit)){
				// left and right are blocked
				// ...go backwards
				newdirection = back;
			} else if(testdirection(lft) && testdirection(rit)){
				// left / right both ok. pick one at random.
				int biaseddirection = 0;
				
				if(left){ 
					biaseddirection = lft; 

				}
				if(!left){ 
					biaseddirection = rit; 

				}
				newdirection = biaseddirection;
			}

		if(!testdirection(direction)){
			direction = newdirection;
		} else if(testdirection(direction) && !testdirection(rit)  
					&& !testdirection(lft)){ 
			// only forward and back are available, go forwards.
			direction = direction;
		} else {
			// 50% move forwards, 50% turn
		
			int biaseddirection = 0;

			if(infront){ 
				biaseddirection = direction; 
			}
			if(!infront){ 
				biaseddirection = newdirection; 
			}

			// Again, ignore the random number and choose the best direction
			
			direction = biaseddirection;
		}
	
		switch(direction){
			case 1:	nextypos = lev.map[ax][ay-1].t; break;
			case 2: nextxpos = lev.map[ax+1][ay].l; break;
			case 3: nextypos = lev.map[ax][ay+1].t; break;
			case 4: nextxpos = lev.map[ax-1][ay].l; break;
		};

	} 
    
    if(nextxpos!=xpos){
		switch(direction){
			case 4: xpos = xpos - speed; break;
			case 2: xpos = xpos + speed; break;
		};
	} else if(nextypos!=ypos){
		switch(direction){
			case 1: ypos = ypos - speed; break;
			case 3: ypos = ypos + speed; break;
		};
	}
}
};

