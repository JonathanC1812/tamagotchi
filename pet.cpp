#include "pet.h"
#include <algorithm>
using std::max;
using std::min;
using std::string;

Pet initPet(const string &name, DifficultyLevel diff)
{
    Pet p;
    // default attributes of pet
    p.name = name;
    p.hunger = 80;
    p.happiness = 70;
    p.health = 100;
    p.energy = 100;
    p.cleanliness = 80;
    p.weight = 10;
    p.age = 0;
    p.gold = 20;
    p.isSick = false;
    p.isSleeping = false;
    p.refusesFood = false;
    p.refusedFoodName = "";
    p.stage = EGG;
    p.lastSaveTime = time(nullptr);

    // assign random personality
    p.personality = static_cast<PersonalityType>(rand() % 4);

    // food preferences (food options are the only ones that are available in shop)
    static const char *foods[] = {
        "Rice Ball", "Apple", "Cake", "Salad", "Fish", "Candy"};
    // differs favorite and disliked food
    int favIdx = rand() % 6;
    int dislikeIdx = rand() % 5;
    if (dislikeIdx >= favIdx)
        dislikeIdx++; // avoid collision
    p.favoriteFoodName = foods[favIdx];
    p.dislikedFoodName = foods[dislikeIdx];

    return p;
}
void clampStats(Pet &p)
{
    p.hunger = max(0, min(100, p.hunger));
    p.happiness = max(0, min(100, p.happiness));
    p.health = max(0, min(100, p.health));
    p.energy = max(0, min(100, p.energy));
    p.cleanliness = max(0, min(100, p.cleanliness));
    p.weight = max(5, min(50, p.weight));
    p.gold = max(0, p.gold);
}
void decayStats(Pet &pet, DifficultyLevel diff)
{
    if (pet.stage == EGG || pet.stage == DEAD)
        return;

    int dr = drainRate(diff);

    // adding effect on drain based on personality
    int happyDrain = dr;
    int energyDrain = dr;
    int hungerDrain = dr * 2;
    int weightGain = 0; // extra weight accumulation

    switch (pet.personality)
    {
    case CHEERFUL:
        happyDrain = max(0, dr - 1); // happiness fades slower
        break;
    case GRUMPY:
        happyDrain = dr + 1;              // happiness fades faster
        hungerDrain = max(1, dr * 2 - 1); // hunger drains slower
        break;
    case LAZY:
        energyDrain = max(0, dr - 1); // energy drains slower
        weightGain = 1;               // but gets heavier over time
        break;
    case ANXIOUS:
        // mo drain change, but penalty applied below when stats are low
        break;
    }

    pet.hunger -= hungerDrain;
    pet.happiness -= happyDrain;
    pet.energy -= energyDrain;
    pet.cleanliness -= dr;
    pet.weight += weightGain;

    // adding health consequences
    if (pet.hunger <= 0)
        pet.health -= dr * 2;
    if (pet.isSick)
        pet.health -= dr * 2;
    if (pet.energy <= 0)
        pet.health -= dr;
    if (pet.weight > 40)
        pet.health -= 1;
    if (pet.happiness <= 10)
        pet.health -= 1;
    if (pet.cleanliness <= 0)
        pet.health -= 1;

    // ANXIOUS penalty: extra health drain when multiple stats are low
    if (pet.personality == ANXIOUS)
    {
        int lowCount = (pet.hunger < 30 ? 1 : 0) + (pet.happiness < 30 ? 1 : 0) + (pet.energy < 30 ? 1 : 0);
        if (lowCount >= 2)
            pet.health -= dr; // anxiety compounds neglect
    }

    clampStats(pet);
}
bool checkEvolution(Pet &pet)
{
    const LifeStage prev = pet.stage;
    if (pet.stage == EGG && pet.age >= 1)
        pet.stage = BABY;
    else if (pet.stage == BABY && pet.age >= 3 && pet.hunger > 50 && pet.health > 50)
        pet.stage = CHILD;
    else if (pet.stage == CHILD && pet.age >= 6 && pet.hunger > 55 && pet.happiness > 55 && pet.health > 60)
        pet.stage = TEEN;
    else if (pet.stage == TEEN && pet.age >= 10 && pet.hunger > 60 && pet.happiness > 60 && pet.health > 65)
        pet.stage = ADULT;
    else if (pet.stage == ADULT && pet.age >= 15)
        pet.stage = SENIOR;
    return pet.stage != prev;
}
bool checkDeath(Pet &pet, string &cause)
{
    if (pet.health <= 0)
    {
        pet.stage = DEAD;
        cause = (pet.hunger <= 0) ? "Starvation" : (pet.isSick ? "Illness" : "Neglect");
        return true;
    }
    return false;
}
void agePet(Pet &pet) { pet.age++; }
string stageName(LifeStage s)
{
    const char *n[] = {"Egg", "Baby", "Child", "Teen", "Adult", "Senior", "Dead"};
    return (s >= EGG && s <= DEAD) ? n[s] : "Unknown";
}
void applyTimedDecay(Pet &pet, DifficultyLevel diff, long secs)
{
    if (secs <= 0 || pet.stage == EGG || pet.stage == DEAD)
        return;
    long turns = min(secs / 120L, (long)100);
    for (long i = 0; i < turns; i++)
    {
        decayStats(pet, diff);
        if (pet.health <= 0)
            break;
    }
    pet.age += (int)(secs / 86400);
}

string personalityName(PersonalityType pt)
{
    const char *n[] = {"Cheerful", "Grumpy", "Lazy", "Anxious"};
    return (pt >= CHEERFUL && pt <= ANXIOUS) ? n[pt] : "Unknown";
}
