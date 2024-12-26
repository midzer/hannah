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

#include "Bonus.h"

class Bonus;

Bonus::Bonus(const char *filename, int x, int y, int speed,AnimationFactory* af, char type):
	Ghost2(filename, x, y, speed, af){
	this->lifeforce = 300;
	this->alive = true;
	this->type = type;
};

void Bonus::decLife(){
	if(lifeforce>0) lifeforce--;
};
