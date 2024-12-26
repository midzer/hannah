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
 
#ifndef _PLAYER_
#define _PLAYER_

#include "Sprite.h"
#include "Defines.h"

class Player : public Sprite{
    public:
        Player(const char* name, AnimationFactory* af);
        //virtual ~Player();
        SDL_Texture* bonusframe();
        void move();
        void respawn();
        void init(int x, int y);
        void setEffect(effecttype effect);
        void blit(SDL_Renderer* screen);
        void setAnimation(const char* name);
        Animation* bonus;
        int lives;
        int direction;
        int nextdir;
        int bonusframenum;
        effecttype effect = effecttype::NONE;
};

#endif