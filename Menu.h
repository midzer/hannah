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

#ifndef _MENU_
#define _MENU_

#include <string>
#include <vector>
#include <iostream>
#include "SDL.h"
#include "Global.h"

#define SPACING 40
#define LEFTPOS 220
#define TOPOFFSET 120

class MenuItem{
    public:
        MenuItem(std::string name, int tag);
        std::string name;
        int tag;
        void Select();
        // Something to record the action
};

class MenuRow{
    public:
        MenuRow();
        void AddItem(std::string name, int tag);
        virtual ~MenuRow();
        std::vector<MenuItem*> items;
        unsigned int currentitem;
        std::string description;
};

class Menu{
    public:
        Menu();
        void AddRow(MenuRow* row);
        void Draw(SDL_Renderer* renderer);
        void MovePrevious();
        void MoveNext();
        void MoveLeft();  // Previous item in row
        void MoveRight(); // Next item in row
        void Select();
        virtual ~Menu();
    private:
        std::vector<MenuRow*> rows;
        unsigned int currentrow;
};

#endif // _MENU_
