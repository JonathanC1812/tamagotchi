#ifndef COMMON_H
#define COMMON_H
#include <string>
#include <vector>
#include <ctime>
enum DifficultyLevel { EASY=0,NORMAL=1,HARD=2 };
enum LifeStage { EGG=0,BABY=1,CHILD=2,TEEN=3,ADULT=4,SENIOR=5,DEAD=6 };
enum ItemType  { FOOD=0,TOY=1,MEDICINE=2,GIFT=3 };
struct Item {
    std::string name; ItemType type;
    int hungerMod,happyMod,healthMod,energyMod,weightMod,price;
};
//adding personality effect to weight, health, energy, hunger, happiness
enum PersonalityType
{
    CHEERFUL = 0, // happiness decays slower, play gives +bonus
    GRUMPY = 1,   // happiness decays faster, hunger drains slower
    LAZY = 2,     // prone to gain weight faster, energy drains slower
    ANXIOUS = 3   // health drops faster when any stat is low
};
struct Pet {
    std::string name;
    int hunger,happiness,health,energy,weight,age,gold;
    bool isSick,isSleeping,refusesFood;
    std::string refusedFoodName;
    LifeStage stage;
    time_t lastSaveTime;
    std::vector<Item> inventory;
    std::vector<std::string> tricksLearned;
// personalityrelated
    PersonalityType personality;
    std::string favoriteFoodName;
    std::string dislikedFoodName;
};
struct GameState {
    Pet pet; DifficultyLevel difficulty;
    int turnCount; bool running; std::string message;
};
inline int  drainRate(DifficultyLevel d) { return d==EASY?1:d==HARD?4:2; }
inline int  eventChance(DifficultyLevel d){ return d==EASY?5:d==HARD?20:12; }
inline int  shopDiscount(DifficultyLevel d){ return d==EASY?75:d==HARD?150:100; }
inline bool isPermadeath(DifficultyLevel d){ return d!=EASY; }
#endif
