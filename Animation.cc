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
 
#include "Animation.h"
#include "Global.h"
#include <string>
#include <fstream>
#include <iostream>

#define delay_set 3

Animation::Animation(SDL_Renderer* renderer, const char* spritename, const char* filename, bool loop, const char* knownAs, bool invis){
	this->currentframe = 0;
    this->name = filename;  
    this->renderer = renderer;
	
	this->loop = loop;
	this->finished = false;
	this->delay = delay_set;
	numframes = 0;

    if(invis){
        std::string ka(knownAs);
        ka.append("-invis");
        this->knownAs = ka.c_str();    
    } else {
        this->knownAs = knownAs;
    }
    
    //cout << this->knownAs;
    
    std::string fpath(spritename);
    fpath.append("-");
    fpath.append(filename);
    fpath.append(".png");
    if((!invis || !(this->loadInvis(spritename, fpath.c_str())))){
        if(!this->loadSheet(spritename, fpath.c_str())){
            //printf(fpath.c_str());
            this->load(spritename);    
        }    
    }    
};

// Load separate files to form an animation
void Animation::load(const char* spritename){
    //cout << "Loading old..." << endl;
    
    std::string path = Global::getPath("sprite.dat", spritename, this->name.c_str());
    
	ifstream br(path.c_str());
	char buffer[30];
	if(!br.is_open()){
		printf("Could not open spritefile %s\n", spritename);
	} else {

		br.getline(buffer,30);
		numframes = atoi(buffer);

		int i = 0;
		for(i = 0; i < numframes; i++){
			br.getline(buffer,30);

            std::string filepath = Global::getPath(buffer, spritename, this->name.c_str());

			SDL_Surface* s;
            SDL_Surface* tmp = IMG_Load(filepath.c_str());
			s = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGBA32, 0);
            SDL_FreeSurface(tmp);

            SDL_Texture* tex = SDL_CreateTextureFromSurface(this->renderer, s);

			frames.push_back(tex);
            
            SDL_FreeSurface(s);
		}		
		numframes = frames.size();
		br.close();
	}
}

bool Animation::loadSheet(const char* spritename, const char* filename){
   
    std::string path = Global::getPath(filename, spritename);
    
	ifstream br(path.c_str());
	
	if(!br.is_open()){
        //cout << "New sprite not found" << endl;
        return false;
    } else {
        br.close();       
        SDL_Surface* tmp = IMG_Load(path.c_str());
        //SDL_SetAlpha(tmp, SDL_SRCALPHA, Uint8(90));
        SDL_SetSurfaceAlphaMod(tmp, 255);
        SDL_Surface* img = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGBA32, 0);
        int nframes = (int)(img->w / SIZET);
        SDL_FreeSurface(tmp);
        
        
        SDL_Rect r;
        SDL_Rect d;
        d.x = 0;
        d.y = 0;
        d.h = SIZET;
        d.w = SIZET;
        
        for(int i = 0; i<nframes; i++){
            //cout << "frame" << i << endl;
            r.x = i*SIZET;
            r.y = 0;
            r.w = SIZET;
            r.h = SIZET;
            SDL_Surface* aframe = SDL_CreateRGBSurface(0 , SIZET, SIZET, 32, rmask, gmask, bmask, amask);
            
            //SDL_FillRect(aframe, NULL, 0x00ffffff);
            
            //SDL_SetSurfaceBlendMode(aframe, SDL_BLENDMODE_ADD);
            //SDL_SetAlpha(img, SDL_SRCALPHA, Uint8(200));
            //SDL_BlitSurface(img, &r, aframe, NULL);
            
            // Can't blit two transparent surfaces together without this
            
            //SDL_Surface* aframe2 = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, SIZET, SIZET, SDL_GetVideoInfo()->vfmt->BitsPerPixel, rmask, gmask, bmask, amask);
            //SDL_FillRect(aframe2, NULL, 0xaa000000);
            //SDL_SetAlpha(aframe2, SDL_SRCALPHA, Uint8(90));
            //SDL_gfxBlitRGBA(aframe2, NULL, aframe, NULL);
            
            
            
            //SDL_gfxBlitRGBA(img, &r, aframe, &d);
            
            SDL_BlitSurface(img, &r, aframe, &d);
            
            //SDL_FillRect(aframe, NULL, 0xAA000000);
            
            SDL_Texture* tex = SDL_CreateTextureFromSurface(this->renderer, aframe);
            
            //SDL_SetAlpha(aframe, SDL_SRCALPHA | SDL_RLEACCEL, 90);
            frames.push_back(tex);
            
            SDL_FreeSurface(aframe);
            //j++;
            //if(j >= numframes){
            //    j = 0;
            //}
            //i++;
        }
        
        SDL_FreeSurface(img);
                
        numframes = nframes;
        
        return true;
    }
}

// load a spritesheet
bool Animation::loadInvis(const char* spritename, const char* filename){
    //cout << "loadSheet " << filename << endl;
    
    std::string path = Global::getPath(filename, spritename);
    
	ifstream br(path.c_str());
	
	if(!br.is_open()){
        //cout << "New sprite not found" << endl;
        return false;
    } else {
        br.close();       
        SDL_Surface* tmp = IMG_Load(path.c_str());
        
        SDL_Surface* img = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGBA32, 0);
        int nframes = (int)(img->w / SIZET);
        SDL_FreeSurface(tmp);
        
        
        SDL_Rect r;
        SDL_Rect d;
        d.x = 0;
        d.y = 0;
        d.h = SIZET;
        d.w = SIZET;
        
        int i = 0;
        int j = 0;
        int k = 0;
        bool in = false;
        
        int lcm = Global::lcm(nframes, flashframes);
        
        //cout << lcm << endl;
        
        while(i<(lcm*2)){
        
        
        //for(int i = 0; i<nframes; i++){
            //cout << "frame" << i << endl;
            r.x = j*SIZET;
            r.y = 0;
            r.w = SIZET;
            r.h = SIZET;
            
            //cout << r.x << endl;
            
            SDL_Surface* aframe = SDL_CreateRGBSurface(0, SIZET, SIZET, 32 , rmask, gmask, bmask, amask);
            
            // Can't blit two transparent surfaces together without this
            
            //SDL_gfxBlitRGBA(img, &r, aframe, &d);
            SDL_BlitSurface(img, &r, aframe, &d);
            
            int amt = 0;
            amt = 255 - (25*k);
            
            //SDL_gfxMultiplyAlpha(aframe, Uint8(amt));            
            
            SDL_Texture* tex = SDL_CreateTextureFromSurface(this->renderer, aframe);
            
            SDL_SetTextureAlphaMod(tex, amt);
            
            frames.push_back(tex);
            
            SDL_FreeSurface(aframe);

            j++;
            if(j >= nframes){
                j = 0;
            }
            i++;
        
            if(in){
                k--;
                if(k==0){
                    in = false;
                }
            } else {
                k++;
                if(k==flashframes){
                    in = true;
                }
            }
        }
        
        SDL_FreeSurface(img);
                
        this->numframes = lcm*2;

        //cout << this->numframes << endl;
        
        return true;
    }
}

const char* Animation::getKnown(){
	return knownAs.c_str();
};

void Animation::reset(){
	currentframe=0;
	finished=false;
};

SDL_Texture* Animation::getFrame(int fnum){
	//cout << knownAs << " > " << fnum << "\n";
	if(fnum>=0 && fnum<numframes-1){
		return frames[fnum];
	} else {
		return frames[numframes-1];
	}
};

SDL_Texture* Animation::getFrame(){
	if(currentframe==numframes-1 && loop==true){
		currentframe = 0;
	}
	if(currentframe==numframes-1 && loop==false){
		currentframe = currentframe;
		finished = true;
	} 
	if(currentframe<numframes-1){
		delay--;
		if(delay==0){
			currentframe++;
			delay = delay_set;
		}
	}
	//printf("Return frame %d\n",currentframe);
	return frames[currentframe];
};

Animation::~Animation(){
	int current = 0;
	for(current=0; current<numframes; current++){
		//cout << "Deleting Animation Frame\n";
		SDL_DestroyTexture(frames[current]);
	}
};