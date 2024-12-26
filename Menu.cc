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

#include "Menu.h"


/* ------ Menu ------ */

Menu::Menu(){
	this->currentrow = 0;
}

void Menu::AddRow(MenuRow* row){
    rows.push_back(row);
}

void Menu::Draw(SDL_Renderer* renderer){
    SDL_Color selcol = {0x53, 0x9c, 0x9d};
    SDL_Color col = {0, 0, 0};
    SDL_Color* c;
    
    // Draw a rectangular backround
    
    SDL_Rect r = {150,100,500,320};
    SDL_Rect rin = {155, 105, 490, 310};
    
    SDL_SetRenderDrawColor(renderer, 0x53, 0x9c, 0x9d, 0x00);
    SDL_RenderFillRect(renderer, &r);
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0x00);
    SDL_RenderFillRect(renderer, &rin);
    
    for(unsigned int i=0; i<this->rows.size(); i++){
        MenuRow* row = rows[i];
        if(i == this->currentrow){
            c = &selcol;
        } else {
            c = &col;
        }
        Global::drawTextAt(renderer, row->items[row->currentitem]->name.c_str(), LEFTPOS, TOPOFFSET+(i*SPACING), 36, *c, false);
    }
}

void Menu::MovePrevious(){
    // Must do the comparison before decrement
    // because unsigned ints CANNOT BE NEGATIVE!!! (muppet)
    if(currentrow-- == 0){
        currentrow = rows.size()-1;
    }
};

void Menu::MoveNext(){
    if(++currentrow == rows.size()){
        currentrow = 0;
    }
};

void Menu::MoveLeft(){
    if(rows[currentrow]->items.size() > 1){
        if(rows[currentrow]->currentitem-- == 0){
            rows[currentrow]->currentitem = rows[currentrow]->items.size() - 1;
        }
        
        rows[currentrow]->items[rows[currentrow]->currentitem]->Select();
    }
};

void Menu::MoveRight(){
    if(rows[currentrow]->items.size() > 1){
        if(++rows[currentrow]->currentitem == rows[currentrow]->items.size()){
            rows[currentrow]->currentitem = 0;
        }        
        
        rows[currentrow]->items[rows[currentrow]->currentitem]->Select();
    }
};

void Menu::Select(){
    rows[currentrow]->items[rows[currentrow]->currentitem]->Select();
};

Menu::~Menu(){
    // Delete each MenuRow
    for(unsigned int i = 0; i < this->rows.size(); i++){
        delete(this->rows[i]);
    }
}


/* ------ MenuRow ------ */

MenuRow::MenuRow(){
    this->currentitem = 0;
};

void MenuRow::AddItem(std::string name, int tag){
    MenuItem* m = new MenuItem(name, tag);
    items.push_back(m);
};

MenuRow::~MenuRow(){
    // Delete each MenuItem
    for(unsigned int i = 0; i < this->items.size(); i++){
        delete(this->items[i]);
    }
}


/* ----- MenuItem ------ */

MenuItem::MenuItem(std::string name, int tag){
    this->name = name;
    this->tag = tag;
}

void MenuItem::Select(){
    // Push an event onto the SDL_Event pump
    // so that we can catch it in the main loop
    // and perform the correct action accordingly
    SDL_Event event;
    SDL_zero(event);
    
    event.type = Global::HannahEventType;
    event.user.code = this->tag;
    
    SDL_PushEvent(&event);
}