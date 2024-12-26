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

#include "ScoreLib.h"

void ScoreLib::createDefaultHiScores(){
    std::string names[] = {"ted", "joe", "zak", "sab", "lux", "ted", "jtr", "ler", "rik", "jim"};
    int scores[] = {25600, 12800, 6400, 3200, 1600, 800, 400, 200, 100, 50};
    int difficulties[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    hiscore.clear();
    for(int i = 0; i < 10; i++){
        
        scoreentry tmp;
        tmp.score = scores[i];
        tmp.name = names[i];
        tmp.difficulty = difficulties[i];

        hiscore.push_back(tmp);
    } 
};


int ScoreLib::checkHiScores(int score){ // return the entry location of the new score
	unsigned int i =0;
	int entryAt = 0;
    //cout << hiscore.size() << endl;
	for(i=0;i<hiscore.size();i++){
		if(score >= ((scoreentry)hiscore[i]).score){
			entryAt = i;
			int j = hiscore.size() - 1;
			while(j > entryAt){
				hiscore[j] = hiscore[j-1];
				j--;
			}
			scoreentry news;
			news.score = score;
            news.name = "god";
            news.difficulty = 0;
			
			hiscore[entryAt] = news;
			return entryAt;
		}
	}
	return -1;
};

void ScoreLib::loadHiScores(){
	#ifndef WIN32
        char *scoredir;
        scoredir = getenv("USER");
        
        if(scoredir==NULL){
            createDefaultHiScores();
            return;
        }

        std::string p("/home/");
        p.append(scoredir);
        p.append("/.hannah/hiscore.dat");
        ifstream br(p.c_str());
        
        if(!br){
            std::string pth("/home/");
            pth.append(scoredir);
            pth.append("/.hannah");
            mkdir(pth.c_str(), 0777);
        }
        
	#else
        char *scoredir;
        scoredir = getenv("APPDATA");
        
        if(scoredir==NULL){
            createDefaultHiScores();
            return;
        }
        
        std::string p(scoredir);
        p.append("\\FatHorseGames");
        
        _mkdir(p.c_str());
        
        p.append("\\Hannah\\");
        _mkdir(p.c_str());
        
        p.append("hiscore.dat");
        
        ifstream br(p.c_str());
        //std::string p("hiscore.dat");
	#endif

	// If hiscore doesn't exist, create it (above)
    if(!br){
        ofstream os(p.c_str());
        //cout << "Creating Hiscore table - " << p.c_str() << "\n";
        os << "25600 ted 0\n12800 zjc 0\n6400 hjw 0\n3200 sab 0\n1600 zjc 0\n800 hjw 0\n400 sab 0\n200 zjc 0\n100 hjw 0\n50 sab 0\n";
        os.flush();
        os.close();
        
        br.open(p.c_str());
    }

    if(br){

        std::string line;
        
        while(getline(br, line)){
            istringstream ist(line);
            
            int nscore;
            std::string nname;
            int ndiff = 0;
            
            if(ist){
                ist >> nscore; 
            }
            
            if(ist){
                ist >> nname;
            }
            
            if(ist){
                ist >> ndiff;
            } else {
                ndiff = 0;
            }
            
            scoreentry tmp;
            tmp.score = nscore;
            tmp.name = nname;
            tmp.difficulty = ndiff;

            hiscore.push_back(tmp);
        }
        br.close();
    }
};

void ScoreLib::saveHiScores(){
	ofstream br;
		
    std::string p = ScoreLib::getHiscoreFile();
    remove(p.c_str());
	br.open(p.c_str());

    if(br){
        
        unsigned int i;
        for(i=0; i<hiscore.size(); i++){
            //printf("Saving entry %d\n",i);
            scoreentry tmp;
            tmp = (scoreentry)hiscore[i];
            int bufsize = snprintf(NULL, 0, "%d %s %d", tmp.score, tmp.name.c_str(), tmp.difficulty);
            //cout << bufsize << endl;
            char* buf = (char*)calloc(bufsize, sizeof(char) + 1);
            sprintf(buf,"%d %s %d",tmp.score,tmp.name.c_str(), tmp.difficulty);
            br << buf << "\n";
            free(buf);
        }
        //br.flush();
        br.close();
    }
	//printf("Finished saving scores\n");
};

void ScoreLib::drawHiScore(SDL_Renderer* renderer, AnimationFactory* af){
	// Read hi score in from file
	int i = 0;
	int yy = 140;

    if(hiscore.size() < 10){
        hiscore.clear();
        createDefaultHiScores();
    }

	for(i=0; i<10; i++){
		scoreentry tmp = (scoreentry)hiscore[i];
		//char buf[30];
		//sprintf(buf,"%d",tmp.score);
		std::ostringstream score;
        score << tmp.score;
        
        Global::drawTextAt(renderer, score.str().c_str(),220,yy,30);
		Global::drawTextAt(renderer, tmp.name.c_str(),420,yy,30);
       
        //score.str("");
        //score << tmp.difficulty;
        //Global::drawTextAt(renderer, score.str().c_str(), 500, yy, 30);
        for(int i = 0; i < tmp.difficulty; i++){
            Global::blit(renderer, af->getScenery("powerpill"), 520 + (i*32), yy);    
        }
        
		yy = yy + 30;
	}

	Global::drawTextAt(renderer, "Press any key",430,460,30, {0,0,0}, true);
};

std::string ScoreLib::getHiscoreFile(){
    #ifdef WIN32
        char *scoredir;
        scoredir = getenv("APPDATA");
               
        std::string p(scoredir);
        p.append("\\FatHorseGames");
        p.append("\\Hannah\\");
        p.append("hiscore.dat");
        
        return p;   
    #else
        char *scoredir;
        scoredir = getenv("USER");
        
        std::string p("/home/");
        p.append(scoredir);
        p.append("/.hannah/hiscore.dat");

        return p;
    #endif
}