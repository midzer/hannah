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

#include "Global.h"

class Global;

std::string Global::getPath(const char* path){
    std::string pp = "";
    #ifndef WIN32
        if(!Global::local){
            pp.append(DATA_INSTALL_DIR);
            pp.append("/resources/");
        } else {
            pp.append("./resources/");
        }
        pp.append(path);
    #else
        pp.append("resources\\");
        pp.append(path);
    #endif
    
    return pp;
};

std::string Global::getPath(const char* path, const char* subpath, const char* folder){
    std::string pp = "";
    #ifndef WIN32
        if(!Global::local){
            pp.append(DATA_INSTALL_DIR);
            pp.append("/resources/");
        } else {
            pp.append("./resources/");
        }
        pp.append(subpath);
        pp.append("/");
    #else
        pp.append("resources\\");
        pp.append(subpath);
        pp.append("\\");
    #endif
    
    std::string fold(folder);
    
    if(fold.compare("") != 0){
        #ifndef WIN32
        pp.append(folder);
        pp.append("/");
        #else
        pp.append(folder);
        pp.append("\\");
        #endif
    }
    
    pp.append(path);
    
    return pp;
};

int Global::lcm(const int a, const int b){
    int lcm = 0;

    if(a>b){
        lcm = a;
    } else {
        lcm = b;
    }
    
    int m = lcm;
    
    while(!(lcm % a == 0) && (lcm % b ==0)){
        lcm += m;
    }
    
    return lcm;
};

// Return the width of the text texture (relcentre = relative centre)
void Global::drawTextAt(SDL_Renderer* renderer, const std::string text, int x, int y, int size, SDL_Color color, bool centre, bool relcentre){
	SDL_Surface* textsurface;
    TTF_Font* fon;
    
    if(fonts.count(size) > 0) {
        fon = fonts.at(size);
    } else {
        fon = TTF_OpenFont(Global::getPath("font.ttf").c_str(), size);
        fonts[size] = fon;
    }
        
	textsurface = TTF_RenderText_Blended(fon,text.c_str(),color);
    
    int w;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, textsurface);
    SDL_QueryTexture(tex, NULL, NULL, &w, NULL);
  
    if(centre){
        x = int((int)((SCREENW) / 2) - (int)((w) / 2));
    } else if (relcentre){
        x = (int)(275 - (int)((w) / 2));
    }
  
	blit(renderer, tex,x,y);
    
	SDL_FreeSurface(textsurface);	

};

void Global::blit(SDL_Renderer* renderer, SDL_Texture* image, int x, int y){
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
    // Absolutely MUST specify full dimensions or weird scaling occurs
    SDL_QueryTexture(image, NULL, NULL, &(dest.w), &(dest.h));
    SDL_RenderCopy(renderer, image, NULL, &dest);
};