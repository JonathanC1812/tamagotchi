#include "items.h"
#include <iostream>
#include <iomanip>
using namespace std;

string getItemTypeName(ItemType t){
    switch(t){
        case FOOD:return "Food";
        case TOY:return "Toy";
        case MEDICINE:return "Medicine";
        case GIFT:return "Gift";
    }
    return "?";
}

vector<Item> getAllShopItems(){
    return {
        //foods
        {"Rice Ball",    FOOD,    30,5, 0,5, 3,8},
        {"Apple",        FOOD,    20,8, 2,3, 1,6},
        {"Cake",         FOOD,    40,15,0,5, 8,15},
        {"Salad",        FOOD,    25,5, 5,5,-2,10},
        {"Fish",         FOOD,    35,10,5,8, 2,12},
        {"Candy",        FOOD,    10,20,0,10,3,5},
        {"Bread",        FOOD,    25,10,0,5,3,18},
        {"Ice Cream",    FOOD,    20,15,8,3,3,22},

        //toys
        {"Ball",         TOY,     0,20, 0,-5,-2,10},
        {"Stuffed Bear", TOY,     0,25, 0,-3,-1,15},
        {"Puzzle",       TOY,     0,15, 0,-8,-2,8},
        {"Jump Rope",    TOY,     0,18, 0,-10,-3,12},
        {"Kite",         TOY,     0,16, 0,-4,-2,22},

        //medicine
        {"Cold Medicine",MEDICINE,0,5,30,0,0,20},
        {"Vitamin",      MEDICINE,0,5,15,10,0,18},
        {"Bandage",      MEDICINE,0,2,20,0,0,12},
        {"First Aid",    MEDICINE,0,3,25,2,0,15},

        //gift
        {"Flower",       GIFT,    0,30,5,0,0,14},
        {"Book",         GIFT,    0,20,0,5,0,16},
        {"Scarf",        GIFT,    0,15,3,0,0,12},
    };
}
void displayInventory(const Pet& pet){
    cout << "\n  === " << pet.name << "'s Inventory ===\n";
    if (pet.inventory.empty()){
        cout << "  (empty)\n"; 
        return;
    }
    for(size_t i=0; i < pet.inventory.size(); i++){
        const Item& it=pet.inventory[i];
        cout << "  [" << i << "] " << left << setw(16) << it.name
                 << "  (" << getItemTypeName(it.type) << ")  HNG+" << it.hungerMod
                 << " HPY+" << it.happyMod << " HP+" << it.healthMod << "\n";
    }
}
void displayShop(DifficultyLevel diff){
    auto cat=getAllShopItems(); int pm=shopDiscount(diff);
    cout << "\n  === TAMAGOTCHI SHOP ===\n";
    string cur="";
    for(size_t i=0; i< cat.size(); i++){
        string tn=getItemTypeName(cat[i].type);
        if (tn != cur){
            cout << "\n  -- " << tn << " --\n";
            cur=tn;
        }
        cout <<"  [" << i << "] " << left << setw(16) << cat[i].name << "  " << cat[i].price*pm/100 <<" gold\n";
    }
    cout << "\n";

    cout << "\n === RECOMMENDATIONS === \n";

    //hunger
    int bestHunger = 0;
    string bestHungerName = "";
    int bestHungerPrice = 0;

    for (const auto& item : cat){
        if (item.type == FOOD && item.hungerMod > bestHunger){
            bestHunger = item.hungerMod;
            bestHungerName = item.name;
            bestHungerPrice = item.price;
        }
    }
    cout << "BEST FOR HUNGER: " << bestHungerName <<  " (+" << bestHunger << " hunger, " << bestHungerPrice << " gold)\n";

    //happiness
    int bestHappy = 0;
    string bestHappyName = "";
    int bestHappyPrice = 0;

    for (const auto& item : cat) {
        if (item.type == TOY && item.happyMod > bestHappy) {
            bestHappy = item.happyMod;
            bestHappyName = item.name;
            bestHappyPrice = item.price;
        }
    }

    cout << "BEST FOR HAPPINESS: " << bestHappyName <<  " (+" << bestHappy << " happiness, " << bestHappyPrice << " gold)\n";

    //health
    int bestHealth = 0;
    string bestHealthName = "";
    int bestHealthPrice = 0;

    for (const auto& item : cat) {
        if (item.type == MEDICINE && item.healthMod > bestHealth) {
            bestHealth = item.healthMod;
            bestHealthName = item.name;
            bestHealthPrice = item.price;
        }
    }

    cout << "BEST FOR HEALTH: " << bestHealthName <<  " (+" << bestHealth << " health, " << bestHealthPrice << " gold)\n";

    //energy
    int bestEnergy = 0;
    string bestEnergyName = "";
    int bestEnergyPrice = 0;

    for (const auto& item : cat) {
        if (item.energyMod > bestEnergy) {
            bestEnergy = item.energyMod;
            bestEnergyName = item.name;
            bestEnergyPrice = item.price;
        }
    }

    cout << "BEST FOR ENERGY: " << bestEnergyName <<  " (+" << bestEnergy << " energy, " << bestEnergyPrice << " gold)\n";

    //weight
    int bestWeight = 0;
    string bestWeightName = "";
    int bestWeightPrice = 0;

    for (const auto& item : cat) {
        if (item.weightMod < bestWeight) {  // More negative = better for weight loss
            bestWeight = item.weightMod;
            bestWeightName = item.name;
            bestWeightPrice = item.price;
        }
    }

    if (bestWeight < 0) {
        cout << "BEST FOR WEIGHT: " << bestWeightName 
             << " (" << bestWeight << " weight, " << bestWeightPrice << " gold)\n";
    }

    //value
    int cheapest = 9999;
    string cheapestName = "";

    for (const auto& item : cat) {
        if (item.price < cheapest) {
            cheapest = item.price;
            cheapestName = item.name;
        }
    }

    cout << "CHEAPEST ITEM: " << cheapestName << " (" << cheapest << " gold)\n";
    cout << "\n";

}

string buyItem(Pet& pet,int idx,DifficultyLevel diff){
    auto cat=getAllShopItems();

    if (idx < 0 || idx >= (int)cat.size()) 
    return "Invalid item number.";

    Item chosen=cat[idx]; 
    int ap=chosen.price*shopDiscount(diff)/100;

    if (pet.gold < ap) 
    return "Not enough gold! Need "+to_string(ap);

    if (pet.inventory.size() >= 20) 
    return "Inventory full! (max 20)";

    pet.gold-=ap; 
    pet.inventory.push_back(chosen);
    return "Bought "+chosen.name+" for "+to_string(ap)+" gold!";
}
void giveStarterItems(Pet& pet){
    auto all=getAllShopItems();
    pet.inventory.push_back(all[0]);   // Rice Ball x2
    pet.inventory.push_back(all[0]);
    pet.inventory.push_back(all[13]);  // Cold Medicine
    pet.inventory.push_back(all[8]);   // Ball
}
