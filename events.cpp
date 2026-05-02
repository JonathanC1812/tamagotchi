#include "events.h"
#include "pet.h"
#include <cstdlib>
#include <vector>
#include <string>

static Item makeDrop(const std::string& name, ItemType type,
                     int hng, int hpy, int hp, int en, int wt) {
    Item it;
    it.name      = name;
    it.type      = type;
    it.hungerMod = hng;
    it.happyMod  = hpy;
    it.healthMod = hp;
    it.energyMod = en;
    it.weightMod = wt;
    it.price     = 0;
    return it;
}

std::string rollEvent(GameState& gs) {
    if (rand() % 100 >= eventChance(gs.difficulty)) return "";

    std::string msg;

    if (rand() % 100 < 8) {
        if (applyStageEvent(gs.pet, msg)) {
            clampStats(gs.pet);
            return msg;
        }
    }

    int typeRoll = rand() % 100;
    if (typeRoll < 40)      applyNegativeEvent(gs.pet, gs.difficulty, msg);
    else if (typeRoll < 70) applyPositiveEvent(gs.pet, msg);
    else                    applyNeutralEvent(gs.pet, msg);

    clampStats(gs.pet);
    return msg;
}

void applyNegativeEvent(Pet& pet, DifficultyLevel diff, std::string& msg) {
    int dr = drainRate(diff);
    int ev = rand() % 10;

    switch (ev) {
        case 0:
            pet.isSick  = true;
            pet.health -= 10 * dr / 2;
            msg = "Oh no! " + pet.name + " caught a cold and is now sick!";
            break;
        case 1:
            pet.happiness -= 20 * dr / 2;
            pet.energy    -= 10;
            msg = pet.name + " had a terrible nightmare and woke up upset!";
            break;
        case 2:
            pet.hunger += 30;
            pet.weight += 5;
            msg = pet.name + " raided the fridge while you weren't looking! (+weight)";
            break;
        case 3:
            pet.happiness -= 20 * dr / 2;
            msg = pet.name + " is extremely bored. Happiness dropped sharply!";
            break;
        case 4:
            pet.health    -= 15 * dr / 2;
            pet.happiness -= 5;
            msg = pet.name + " got into a scuffle and came back hurt!";
            break;
        case 5:
            if (!pet.inventory.empty()) {
                std::string lostItem = pet.inventory.back().name;
                pet.inventory.pop_back();
                msg = pet.name + " accidentally broke the " + lostItem + "! Item lost.";
            } else {
                pet.happiness -= 10;
                msg = pet.name + " has nothing to play with and feels lonely.";
            }
            break;
        case 6:
            pet.health    -= 10 * dr / 2;
            pet.happiness -= 10;
            msg = pet.name + " has a stomachache! Maybe ate too much recently.";
            break;
        case 7:
            pet.energy    -= 25 * dr / 2;
            pet.happiness -= 5;
            msg = pet.name + " had a restless night and barely slept.";
            break;
        case 8:
            pet.happiness -= 15 * dr / 2;
            pet.energy    -= 5;
            msg = pet.name + " is throwing a tantrum! Won't cooperate today.";
            break;
        case 9: {
            int lost = 5 + rand() % (10 * dr);
            lost = std::min(lost, pet.gold);
            pet.gold      -= lost;
            pet.happiness -= 5;
            msg = pet.name + " lost " + std::to_string(lost) + " gold somehow!";
            break;
        }
    }
}

void applyPositiveEvent(Pet& pet, std::string& msg) {
    int ev = rand() % 8;

    switch (ev) {
        case 0: {
            int coins = 10 + rand() % 21;
            pet.gold      += coins;
            pet.happiness += 5;
            msg = pet.name + " found " + std::to_string(coins) + " gold on the floor!";
            break;
        }
        case 1: {
            std::vector<std::string> tricks = {
                "backflip", "high-five", "spin", "wave", "bow", "wink", "somersault", "dance"
            };
            std::string trick = tricks[rand() % tricks.size()];
            bool alreadyKnows = false;
            for (const auto& t : pet.tricksLearned)
                if (t == trick) { alreadyKnows = true; break; }
            if (!alreadyKnows) {
                pet.tricksLearned.push_back(trick);
                pet.happiness += 10;
                msg = pet.name + " spontaneously learned to " + trick + "!";
            } else {
                pet.happiness += 8;
                msg = pet.name + " is feeling particularly talented today!";
            }
            break;
        }
        case 2:
            pet.happiness += 20;
            pet.energy    += 5;
            msg = pet.name + " woke up in an amazing mood today!";
            break;
        case 3:
            if (pet.inventory.size() < 20) {
                pet.inventory.push_back(makeDrop("Mystery Candy", FOOD, 15, 10, 0, 5, 2));
                msg = "A mystery candy appeared! Added to " + pet.name + "'s inventory.";
            } else {
                pet.gold += 5;
                msg = pet.name + " found a gift but inventory is full! Got 5 gold instead.";
            }
            break;
        case 4:
            pet.energy    += 25;
            pet.happiness += 5;
            msg = pet.name + " had an incredibly refreshing rest. Energy surging!";
            break;
        case 5:
            pet.health    += 15;
            pet.happiness += 5;
            if (pet.isSick && rand() % 100 < 30) {
                pet.isSick = false;
                msg = pet.name + " somehow recovered from illness on their own!";
            } else {
                msg = pet.name + " is glowing with health and vitality today!";
            }
            break;
        case 6:
            if (pet.inventory.size() < 20) {
                pet.inventory.push_back(makeDrop("Wild Herb", MEDICINE, 0, 5, 20, 5, 0));
                msg = pet.name + " found a wild herb! (Medicine added to inventory)";
            } else {
                pet.health += 10;
                msg = pet.name + " found a herb and ate it on the spot! Health +10.";
            }
            break;
        case 7: {
            int lost = 2 + rand() % 4;
            pet.weight    -= lost;
            pet.happiness += 5;
            msg = pet.name + " ran around and feels lighter! Weight -" + std::to_string(lost) + "g.";
            break;
        }
    }
}

void applyNeutralEvent(Pet& pet, std::string& msg) {
    int ev = rand() % 10;

    switch (ev) {
        case 0:
            pet.refusesFood     = true;
            pet.refusedFoodName = "Rice Ball";
            msg = pet.name + " refuses to eat Rice Ball today. Very picky!";
            break;
        case 1:
            pet.energy -= 10;
            msg = pet.name + " yawns dramatically. Feeling extra sleepy today.";
            break;
        case 2:
            pet.happiness -= 5;
            msg = pet.name + " is hiding under the bed for mysterious reasons.";
            break;
        case 3:
            pet.happiness += 5;
            msg = pet.name + " is singing a cheerful little tune!";
            break;
        case 4:
            msg = pet.name + " has been staring at the wall for 10 minutes. [???]";
            break;
        case 5:
            pet.happiness -= 5;
            msg = pet.name + " is whining. Really wants a Ball toy right now!";
            break;
        case 6:
            pet.hunger -= 3;
            msg = pet.name + " has the hiccups. *hic* *hic*";
            break;
        case 7:
            pet.happiness += 8;
            msg = pet.name + " found a shiny rock and is delighted by it.";
            break;
        case 8:
            msg = pet.name + " is sitting quietly, thinking about absolutely nothing.";
            break;
        case 9:
            if (pet.isSick) {
                pet.health -= 3;
                msg = pet.name + " sneezes loudly. The cold is getting worse!";
            } else {
                msg = pet.name + " sneezes unexpectedly. Probably fine.";
            }
            break;
    }
}

bool applyStageEvent(Pet& pet, std::string& msg) {
    switch (pet.stage) {
        case EGG:
            pet.happiness += 5;
            msg = pet.name + " wobbles! Something is stirring inside the egg...";
            return true;
        case BABY:
            pet.hunger    -= 10;
            pet.happiness -= 10;
            msg = pet.name + " is crying and won't stop! Feed and play ASAP.";
            return true;
        case CHILD:
            pet.happiness += 15;
            msg = pet.name + " made a new friend at the playground!";
            return true;
        case TEEN:
            if (rand() % 2 == 0) {
                pet.happiness -= 10;
                pet.health    -= 5;
                msg = pet.name + " is going through a rebellious phase. Came home late.";
            } else {
                pet.happiness += 15;
                pet.energy    -= 10;
                msg = pet.name + " snuck out and had a blast! (Don't tell anyone)";
            }
            return true;
        case ADULT: {
            int bonus = 10 + rand() % 11;
            pet.gold      += bonus;
            pet.happiness += 10;
            msg = pet.name + " accomplished something great and earned " + std::to_string(bonus) + " gold!";
            return true;
        }
        case SENIOR:
            if (rand() % 2 == 0) {
                pet.happiness += 10;
                msg = pet.name + " is reminiscing about the good old days.";
            } else {
                pet.health += 5;
                msg = pet.name + " takes a slow, mindful walk. Feeling better.";
            }
            return true;
        case DEAD:
            return false;
    }
    return false;
}

