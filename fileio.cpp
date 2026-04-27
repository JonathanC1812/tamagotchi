#include "fileio.h"
#include "pet.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <iomanip>

bool saveGame(const GameState& gs){
    std::ofstream f(SAVE_FILE);
    if(!f.is_open()) return false;
    const Pet& p=gs.pet;
    f<<"name="<<p.name<<"\nhunger="<<p.hunger<<"\nhappiness="<<p.happiness
     <<"\nhealth="<<p.health<<"\nenergy="<<p.energy<<"\nweight="<<p.weight
     <<"\nage="<<p.age<<"\ngold="<<p.gold<<"\nisSick="<<p.isSick
     <<"\nstage="<<(int)p.stage<<"\ndifficulty="<<(int)gs.difficulty
     <<"\nturnCount="<<gs.turnCount<<"\nsaveTime="<<(long long)time(nullptr)<<"\n";
    f<<"trickCount="<<p.tricksLearned.size()<<"\n";
    for(auto& t:p.tricksLearned) f<<"trick="<<t<<"\n";
    f<<"itemCount="<<p.inventory.size()<<"\n";
    for(auto& it:p.inventory)
        f<<"iname="<<it.name<<"\nitype="<<(int)it.type
         <<"\nihng="<<it.hungerMod<<"\nihpy="<<it.happyMod
         <<"\nihp="<<it.healthMod<<"\nien="<<it.energyMod
         <<"\niwt="<<it.weightMod<<"\nipr="<<it.price<<"\n";
    return true;
}
static std::string rv(std::ifstream& f){
    std::string l; if(!std::getline(f,l)) return "";
    size_t e=l.find('='); return e==std::string::npos?"":l.substr(e+1);
}
bool loadGame(GameState& gs){
    std::ifstream f(SAVE_FILE);
    if(!f.is_open()) return false;
    Pet& p=gs.pet;
    long long savedTime=0;
    p.name=rv(f);
    p.hunger=std::stoi(rv(f)); p.happiness=std::stoi(rv(f));
    p.health=std::stoi(rv(f)); p.energy=std::stoi(rv(f));
    p.weight=std::stoi(rv(f)); p.age=std::stoi(rv(f));
    p.gold=std::stoi(rv(f));   p.isSick=std::stoi(rv(f));
    p.stage=(LifeStage)std::stoi(rv(f));
    gs.difficulty=(DifficultyLevel)std::stoi(rv(f));
    gs.turnCount=std::stoi(rv(f));
    savedTime=std::stoll(rv(f));
    p.isSleeping=false; p.refusesFood=false; p.refusedFoodName="";
    int tc=std::stoi(rv(f)); p.tricksLearned.clear();
    for(int i=0;i<tc;i++) p.tricksLearned.push_back(rv(f));
    int ic=std::stoi(rv(f)); p.inventory.clear();
    for(int i=0;i<ic;i++){
        Item it;
        it.name=rv(f); it.type=(ItemType)std::stoi(rv(f));
        it.hungerMod=std::stoi(rv(f)); it.happyMod=std::stoi(rv(f));
        it.healthMod=std::stoi(rv(f)); it.energyMod=std::stoi(rv(f));
        it.weightMod=std::stoi(rv(f)); it.price=std::stoi(rv(f));
        p.inventory.push_back(it);
    }
    long elapsed=(long)(time(nullptr)-savedTime);
    if(elapsed>0){
        std::cout<<"\n  ["<<elapsed/60<<" min passed while away]\n";
        applyTimedDecay(p,gs.difficulty,elapsed);
    }
    gs.running=true; gs.message="Welcome back, "+p.name+"!";
    return true;
}
bool saveFileExists(){ std::ifstream f(SAVE_FILE); return f.good(); }
bool deleteSaveFile(){ return std::remove(SAVE_FILE)==0; }
bool logDeath(const Pet& pet,const std::string& cause,DifficultyLevel diff){
    std::ofstream f(DEATH_LOG_FILE,std::ios::app);
    if(!f.is_open()) return false;
    time_t now=time(nullptr); char buf[32];
    strftime(buf,sizeof(buf),"%Y-%m-%d",localtime(&now));
    const char* d=diff==EASY?"Easy":diff==HARD?"Hard":"Normal";
    f<<buf<<" | "<<pet.name<<" | Age:"<<pet.age<<" | Stage:"<<stageName(pet.stage)
     <<" | Cause:"<<cause<<" | Diff:"<<d<<"\n";
    return true;
}
struct LB{ std::string name,date; int age,score; };
static std::vector<LB> readLB(){
    std::vector<LB> v; std::ifstream f(LEADERBOARD_FILE);
    if(!f.is_open()) return v;
    std::string l;
    while(std::getline(f,l)){
        if(l.empty()) continue;
        std::istringstream ss(l); LB e;
        std::string a,s;
        std::getline(ss,e.date,'|'); std::getline(ss,e.name,'|');
        std::getline(ss,a,'|');     std::getline(ss,s,'|');
        try{e.age=std::stoi(a);e.score=std::stoi(s);}catch(...){continue;}
        v.push_back(e);
    }
    return v;
}
bool updateLeaderboard(const Pet& pet){
    auto v=readLB();
    time_t now=time(nullptr); char buf[32];
    strftime(buf,sizeof(buf),"%Y-%m-%d",localtime(&now));
    LB e; e.name=pet.name; e.age=pet.age;
    e.score=pet.age*10+pet.health; e.date=buf;
    v.push_back(e);
    std::sort(v.begin(),v.end(),[](const LB& a,const LB& b){return a.score>b.score;});
    if((int)v.size()>MAX_LEADERBOARD) v.resize(MAX_LEADERBOARD);
    std::ofstream f(LEADERBOARD_FILE);
    if(!f.is_open()) return false;
    for(auto& x:v) f<<x.date<<"|"<<x.name<<"|"<<x.age<<"|"<<x.score<<"\n";
    return true;
}
void displayLeaderboard(){
    auto v=readLB();
    std::cout<<"\n  === TOP PETS LEADERBOARD ===\n";
    if(v.empty()){std::cout<<"  (no entries yet)\n\n";return;}
    for(size_t i=0;i<v.size();i++)
        std::cout<<"  "<<i+1<<". "<<std::left<<std::setw(12)<<v[i].name
                 <<"  Age:"<<std::setw(4)<<v[i].age<<"  Score:"<<v[i].score<<"\n";
    std::cout<<"\n";
}
void displayDeathLog(){
    std::ifstream f(DEATH_LOG_FILE);
    std::cout<<"\n  === DEATH LOG ===\n";
    if(!f.is_open()){std::cout<<"  (no deaths yet)\n\n";return;}
    std::string l; int c=0;
    while(std::getline(f,l)&&c<10){std::cout<<"  "<<l<<"\n";c++;}
    if(!c) std::cout<<"  (no deaths yet)\n";
    std::cout<<"\n";
}
