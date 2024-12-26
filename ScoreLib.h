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

#ifndef _SCORELIB_
#define _SCORELIB_

#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <cstring>
#include <sstream>
#include "Defines.h"
#include "Global.h"
#include "AnimationFactory.h"

#ifndef WIN32
    #include <sys/stat.h>
    #include <sys/types.h>
#else
    #include <ctime>
    #include <direct.h>
#endif

using namespace std;

class ScoreLib
{
    public:
        static void createDefaultHiScores();
        static int checkHiScores(int score);
        static void loadHiScores();
        static void saveHiScores();
        static void drawHiScore(SDL_Renderer* renderer, AnimationFactory* af);
        static vector<scoreentry> hiscore;
        static std::string getHiscoreFile();
};

#endif // _SCORELIB_
