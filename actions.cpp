#include "actions.h"
#include "pet.h"
#include <sstream>
bool canAct(const Pet &p) { return p.stage != DEAD && !p.isSleeping; }
std::string actionFeed(GameState &gs, int idx)
{
    Pet &p = gs.pet;
    if (!canAct(p))
        return "Pet cannot eat now.";
    if (p.happiness <= 10)
        return p.name + " is too sad to eat! Play first.";
    if (p.hunger >= 95)
        return p.name + " is already full!";
    if (idx < 0 || idx >= (int)p.inventory.size())
        return "Invalid selection.";
    Item &it = p.inventory[idx];
    if (it.type != FOOD)
        return "That is not food!";
    if (p.refusesFood && it.name == p.refusedFoodName)
        return p.name + " refuses " + it.name + " today!";
    p.hunger += it.hungerMod;
    p.happiness += it.happyMod;
    p.health += it.healthMod;
    p.energy += it.energyMod;
    p.weight += it.weightMod;
    std::string n = it.name;
    // adding food preferences
    std::string msg = "Fed " + p.name + " a " + n + ".";

    // --- Personality-aware food preference reactions ---
    if (n == p.favoriteFoodName)
    {
        // Extra happiness burst for favorite food
        p.happiness += 10;
        switch (p.personality)
        {
        case CHEERFUL:
            msg += " " + p.name + " squeals with delight! (fav +10 happiness)";
            break;
        case GRUMPY:
            msg += " Even " + p.name + " cracks a tiny smile. (fav +10 happiness)";
            break;
        case LAZY:
            msg += " " + p.name + " perks up immediately! (fav +10 happiness)";
            break;
        case ANXIOUS:
            msg += " " + p.name + " relaxes a little. (fav +10 happiness)";
            break;
        }
    }
    else if (n == p.dislikedFoodName)
    {
        // Unhappiness penalty for disliked food
        p.happiness -= 8;
        switch (p.personality)
        {
        case CHEERFUL:
            msg += " " + p.name + " tries to stay positive but... ew. (-8 happiness)";
            break;
        case GRUMPY:
            msg += " " + p.name + " shoots you a LOOK. (-8 happiness)";
            break;
        case LAZY:
            msg += " " + p.name + " couldn't be bothered to enjoy that. (-8 happiness)";
            break;
        case ANXIOUS:
            msg += " " + p.name + " stress-eats it anyway. (-8 happiness)";
            break;
        }
    }
    else
    {
        msg += " Yum!";
    }

    p.inventory.erase(p.inventory.begin() + idx);
    p.refusesFood = false;
    clampStats(p);
    return msg;
}
std::string actionPlay(GameState &gs, int idx)
{
    Pet &p = gs.pet;
    if (!canAct(p))
        return "Pet cannot play now.";
    if (p.energy <= 10)
        return p.name + " is too tired! Let them sleep.";
    if (idx == -1)
    {
        p.happiness += 10;
        p.energy -= 8;
        p.weight -= 1;
        clampStats(p);
        return "You played with " + p.name + "!";
    }
    if (idx >= (int)p.inventory.size())
        return "Invalid item.";
    Item &toy = p.inventory[idx];
    if (toy.type != TOY)
        return "That is not a toy!";
    p.happiness += toy.happyMod;
    p.energy -= 8;
    p.weight -= 2;
    std::string tn = toy.name;
    p.inventory.erase(p.inventory.begin() + idx);
    clampStats(p);
    return p.name + " played with the " + tn + "!";
}
std::string actionSleep(GameState &gs)
{
    Pet &p = gs.pet;
    if (p.stage == DEAD)
        return "Pet is gone.";
    if (p.energy >= 90)
        return p.name + " is not tired!";
    int r = 100 - p.energy;
    p.energy = 100;
    p.health += 5;
    if (p.isSick && gs.difficulty != HARD && rand() % 100 < 40)
        p.isSick = false;
    clampStats(p);
    return p.name + " slept well! Energy +" + std::to_string(r) + ".";
}
std::string actionHeal(GameState &gs, int idx)
{
    Pet &p = gs.pet;
    if (!canAct(p))
        return "Pet cannot be treated now.";
    if (idx < 0 || idx >= (int)p.inventory.size())
        return "Invalid item.";
    Item &med = p.inventory[idx];
    if (med.type != MEDICINE)
        return "That is not medicine!";
    p.health += med.healthMod;
    p.happiness += med.happyMod;
    p.isSick = false;
    std::string mn = med.name;
    p.inventory.erase(p.inventory.begin() + idx);
    clampStats(p);
    return "Gave " + p.name + " " + mn + ". Health restored!";
}

std::string actionWork(GameState &gs, int left, int right, char op, int answer)
{
    Pet &p = gs.pet;
    if (!canAct(p))
        return "Pet cannot work now.";
    if (p.energy < 15)
        return p.name + " is too tired to work!";
    if (p.happiness < 10)
        return p.name + " is too sad to work!";

    int correct = 0;
    switch (op)
    {
    case '+':
        correct = left + right;
        break;
    case '-':
        correct = left - right;
        break;
    case '*':
        correct = left * right;
        break;
    default:
        return "Invalid work equation.";
    }

    p.energy -= 10;
    p.happiness -= 2;
    p.cleanliness -= 3;

    if (answer == correct)
    {
        int reward = 8 + rand() % 8;
        p.gold += reward;
        clampStats(p);
        return "Correct! " + p.name + " earned " + std::to_string(reward) + " gold.";
    }

    clampStats(p);
    return "Wrong answer. The correct answer was " + std::to_string(correct) + ". No gold earned.";
}

std::string actionBath(GameState &gs)
{
    Pet &p = gs.pet;
    if (!canAct(p))
        return "Pet cannot take a bath now.";
    if (p.cleanliness >= 95)
        return p.name + " is already clean!";

    int gained = 100 - p.cleanliness;
    p.cleanliness = 100;
    p.happiness += 3;
    p.energy -= 5;
    clampStats(p);
    return p.name + " took a bath! Cleanliness +" + std::to_string(gained) + ".";
}
std::string actionStatus(const GameState &gs)
{
    const Pet &p = gs.pet;
    auto bar = [](int v) -> std::string
    {
        std::string b = "[";
        for (int i = 0; i < 10; i++)
            b += (i < v / 10) ? '#' : '.';
        return b + "]";
    };
    std::ostringstream o;
    o << "\n  Name: " << p.name << "   Stage: " << stageName(p.stage)
      << "   Age: " << p.age << " day(s)\n"
      // adding fav food and disliked food
      << "  Personality: " << personalityName(p.personality) << "\n"
      << "  Fav food: " << p.favoriteFoodName
      << "   Dislikes: " << p.dislikedFoodName << "\n"
      << "  Weight: " << p.weight << "g   Gold: " << p.gold << "\n\n"
      << "  Hunger:    " << bar(p.hunger) << "  " << p.hunger << "%\n"
      << "  Happiness: " << bar(p.happiness) << "  " << p.happiness << "%\n"
      << "  Health:    " << bar(p.health) << "  " << p.health << "%\n"
      << "  Energy:    " << bar(p.energy) << "  " << p.energy << "%\n";
      << "  Clean:     " << bar(p.cleanliness) << "  " << p.cleanliness << "%\n";
    if (p.isSick)
        o << "  !! SICK !! Use medicine!\n";
    if (!p.tricksLearned.empty())
    {
        o << "  Tricks: ";
        for (size_t i = 0; i < p.tricksLearned.size(); i++)
        {
            if (i)
                o << ", ";
            o << p.tricksLearned[i];
        }
        o << "\n";
    }
    o << "  Inventory: " << p.inventory.size() << " item(s)\n";
    return o.str();
}
