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

#ifndef _GLOBAL_
#define _GLOBAL_

#include <string>
#include <map>
#include <cstdio>
#include <exception>
#include "SDL.h"
#include "SDL_ttf.h"
#include "Defines.h"

class Global{
    public:
        static bool local;
        static std::string getPath(const char* path);
        static std::string getPath(const char* path, const char* subpath, const char* folder = "");
        static int lcm(const int a, const int b);
        static void blit(SDL_Renderer* renderer, SDL_Texture* image, int x, int y);
        static void drawTextAt(SDL_Renderer* renderer, const std::string text, int x, int y, int size, SDL_Color color = {0,0,0}, bool centre = false, bool relcentre = false);
        // When a font size is requested, we check it has been used before
        // and if not, add it to this map so it can be tracked and used again
        static std::map<int, TTF_Font*> fonts;
        static Uint32 HannahEventType;
};

#endif