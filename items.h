#ifndef ITEMS_H
#define ITEMS_H
#include "common.h"
#include <vector>
std::vector<Item> getAllShopItems();
void displayInventory(const Pet& pet);
void displayShop(DifficultyLevel diff);
std::string buyItem(Pet& pet, int itemIndex, DifficultyLevel diff);
void giveStarterItems(Pet& pet);
std::string getItemTypeName(ItemType t);
#endif
